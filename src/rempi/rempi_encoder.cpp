/* ==========================ReMPI:LICENSE==========================================   
   Copyright (c) 2016, Lawrence Livermore National Security, LLC.                     
   Produced at the Lawrence Livermore National Laboratory.                            
                                                                                       
   Written by Kento Sato, kento@llnl.gov. LLNL-CODE-711357.                           
   All rights reserved.                                                               
                                                                                       
   This file is part of ReMPI. For details, see https://github.com/PRUNER/ReMPI       
   Please also see the LICENSE file for our notice and the LGPL.                      
                                                                                       
   This program is free software; you can redistribute it and/or modify it under the   
   terms of the GNU General Public License (as published by the Free Software         
   Foundation) version 2.1 dated February 1999.                                       
                                                                                       
   This program is distributed in the hope that it will be useful, but WITHOUT ANY    
   WARRANTY; without even the IMPLIED WARRANTY OF MERCHANTABILITY or                  
   FITNESS FOR A PARTICULAR PURPOSE. See the terms and conditions of the GNU          
   General Public License for more details.                                           
                                                                                       
   You should have received a copy of the GNU Lesser General Public License along     
   with this program; if not, write to the Free Software Foundation, Inc., 59 Temple   
   Place, Suite 330, Boston, MA 02111-1307 USA
   ============================ReMPI:LICENSE========================================= */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <vector>

#include "rempi_err.h"
#include "rempi_event.h"
#include "rempi_encoder.h"
#include "rempi_config.h"
#include "rempi_compression_util.h"



/* ============================================= */
/*  CLASS rempi_encoder_input_format_test_table  */
/* ============================================= */

void rempi_encoder_input_format_test_table::clear()
{
  int size = events_vec.size();
  for (int i = 0; i < size; i++) {
    //    delete events_vec[i];
  }
  events_vec.clear();

  if (!epoch_umap.empty()) epoch_umap.clear();
  epoch_rank_vec.clear();
  epoch_clock_vec.clear();
  
  /*With previous*/
  with_previous_vec.clear();
  with_previous_bool_vec.clear();
  compressed_with_previous_length = 0;
  compressed_with_previous_size   = 0;
  if (compressed_with_previous != NULL) {
    //    free(compressed_with_previous);
    compressed_with_previous = NULL;
  }

  /* Matched index */
  matched_index_vec.clear();
  compressed_matched_index_length = 0;
  compressed_matched_index_size   = 0;
  if (compressed_matched_index != NULL) {
    //free(compressed_matched_index);
    compressed_matched_index = NULL;
  }


  /*Unmatched */
  unmatched_events_id_vec.clear();
  unmatched_events_count_vec.clear();
  if(!unmatched_events_umap.empty()) unmatched_events_umap.clear();
  compressed_unmatched_events_id_size = 0;
  if (compressed_unmatched_events_id  != NULL) {
    //free(compressed_unmatched_events_id);
    compressed_unmatched_events_id = NULL;
  }
  compressed_unmatched_events_count_size = 0;
  if (compressed_unmatched_events_count  != NULL) {
    //    free(compressed_unmatched_events_count);
    compressed_unmatched_events_count = NULL;
  }

  /*Matched*/
  matched_events_id_vec.clear();
  matched_events_delay_vec.clear();
  matched_events_permutated_indices_vec.clear(); 
  replayed_matched_event_index = 0;
  compressed_matched_events_size = 0;
  if (compressed_matched_events != NULL) {
    //    free(compressed_matched_events);
    compressed_matched_events = NULL;
  }

  return;
}

/* ==================================== */
/*  CLASS rempi_encoder_input_format    */
/* ==================================== */

size_t rempi_encoder_input_format::length()
{
  return total_length;
}

void rempi_encoder_input_format::add(rempi_event *event)
{
  rempi_encoder_input_format_test_table *test_table;
  if (test_tables_map.find(0) == test_tables_map.end()) {
    test_tables_map[0] = new rempi_encoder_input_format_test_table(0);
  }
  test_table = test_tables_map[0];
  test_table->events_vec.push_back(event);
  total_length++;
  return;
}

void rempi_encoder_input_format::add(rempi_event *event, int test_id)
{
  rempi_encoder_input_format_test_table *test_table;
  if (test_tables_map.find(test_id) == test_tables_map.end()) {
    test_tables_map[test_id] = new rempi_encoder_input_format_test_table(test_id);
  }
  test_table = test_tables_map[test_id];
  test_table->events_vec.push_back(event);
  total_length++;
  return;
}

void rempi_encoder_input_format::format()
{
  /*Nothing to do*/
  return;
}

void rempi_encoder_input_format::clear()
{
  rempi_encoder_input_format_test_table *test_table;
  map<int, rempi_encoder_input_format_test_table*>::iterator test_tables_map_it;
  for (test_tables_map_it  = test_tables_map.begin();
       test_tables_map_it != test_tables_map.end();
       test_tables_map_it++) {
    test_table = test_tables_map_it->second;
    test_table->clear();
    delete test_table;
  }

  if(decompressed_record_char != NULL)  free(decompressed_record_char);
  write_queue_vec.clear();
  write_size_queue_vec.clear();
  return;
}

void rempi_encoder_input_format::debug_print()
{
  REMPI_DBG("debug print is not implemented");
  return;
}


/* ==================================== */
/*      CLASS rempi_encoder             */
/* ==================================== */

bool rempi_encoder::is_event_pooled(rempi_event* replaying_event)
{
  list<rempi_event*>::iterator matched_event_pool_it;
  for (matched_event_pool_it  = matched_event_pool.begin();
       matched_event_pool_it != matched_event_pool.end();
       matched_event_pool_it++) {
    rempi_event *pooled_event;
    pooled_event = *matched_event_pool_it;
    bool is_source = (pooled_event->get_source() == replaying_event->get_source());
    bool is_clock  = (pooled_event->get_clock()  == replaying_event->get_clock());
    if (is_source && is_clock) return true;
  }
  return false;
}

rempi_event* rempi_encoder::pop_event_pool(rempi_event* replaying_event)
{
  list<rempi_event*>::iterator matched_event_pool_it;
  for (matched_event_pool_it  = matched_event_pool.begin();
       matched_event_pool_it != matched_event_pool.end();
       matched_event_pool_it++) {
    rempi_event *pooled_event;
    pooled_event = *matched_event_pool_it;
    bool is_source = (pooled_event->get_source() == replaying_event->get_source());
    bool is_clock  = (pooled_event->get_clock()  == replaying_event->get_clock());
    if (is_source && is_clock) {
      matched_event_pool.erase(matched_event_pool_it);
      return pooled_event;
    }
  }
  return NULL;
}

void rempi_encoder::set_record_path(string path)
{
  this->record_path = path;
  return;
}


int ct = 0;
void rempi_encoder::open_record_file()
{

  //  REMPI_DBG("open: %s ||%d", record_path.c_str(), ct++);

  if (rempi_mode == REMPI_ENV_REMPI_MODE_RECORD) {
    record_fs.open(this->record_path.c_str(), ios::out);
  } else if (rempi_mode == REMPI_ENV_REMPI_MODE_REPLAY) {
    record_fs.open(this->record_path.c_str(), ios::in);
  } else {
    REMPI_ERR("Unknown record replay mode");
  }

  if(!record_fs.is_open()) {
    REMPI_ERR("Record file open failed: %s", record_path.c_str());
    REMPI_ASSERT(0);
  }
  
  return;
}

rempi_encoder_input_format* rempi_encoder::create_encoder_input_format()
{ 
  return new rempi_encoder_input_format();
}


/*  ==== 3 Step compression ==== 
    1. rempi_encoder.get_encoding_event_sequence(...): [rempi_event_list => rempi_encoding_structure]
    this function translate sequence of event(rempi_event_list) 
    into a data format class(rempi_encoding_structure) whose format 
    is sutable for compression(rempi_encoder.encode(...))
   

    2. repmi_encoder.encode(...): [rempi_encoding_structure => char*]
    encode, i.e., rempi_encoding_structure -> char*
     
    3. write_record_file: [char* => "file"]
    write the data(char*)
    =============================== */

// bool rempi_encoder::extract_encoder_input_format_chunk(rempi_event_list<rempi_event*> &events, rempi_encoder_input_format &input_format)
// {
//   rempi_event *event;
//   rempi_encoder_input_format_test_table *test_table;
//   bool is_extracted = false;
//   event = events.pop();
//   /*encoding_event_sequence get only one event*/
//   if (event != NULL) {
//     //    rempi_dbgi(0, "pusu event: %p", event);                                                              
//     input_format.add(event, 0);
//     is_extracted = true;
//   }
//   return is_extracted;
// }

bool rempi_encoder::extract_encoder_input_format_chunk(rempi_event_list<rempi_event*> &events, rempi_encoder_input_format *input_format)
{
  rempi_event *event_dequeued;
  bool is_ready_for_encoding = false;

  // if (this->input_format != NULL)  {
  //   REMPI_ERR("input_format is not freeed, and is not NULL");
  // }
  this->input_format = input_format;

  while (1) {
    /*Append events to current check as many as possible*/
    if (events.front() == NULL) break;
    event_dequeued = events.pop();
    //REMPI_DBG("event: source: %d", event_dequeued->get_source());
    input_format->add(event_dequeued);
  }

  //  REMPI_DBG("input_format->length() %d", input_format->length());
  if (input_format->length() == 0) {
    return is_ready_for_encoding; /*false*/
  }


  if (input_format->length() > rempi_max_event_length || events.is_push_closed_()) {
    /*If got enough chunck size, OR the end of run*/
    while (1) {
      /*Append events to current check as many as possible*/
      if (events.front() == NULL) break;
      event_dequeued = events.pop();
      input_format->add(event_dequeued);
    }
    input_format->format();
    is_ready_for_encoding = true;
  }
  return is_ready_for_encoding; /*true*/
}


void rempi_encoder::encode()
{
  size_t original_size, compressed_size;
  rempi_event *encoding_event;
  rempi_encoder_input_format_test_table *test_table;
  int *original_buff, original_buff_offset;
  int recoding_inputs_num = rempi_event::record_num; // count, flag, rank, with_next and clock

  test_table = input_format->test_tables_map[0];

  original_size = rempi_event::max_size * test_table->events_vec.size();
  original_buff = (int*)malloc(original_size);
  for (int i = 0; i < test_table->events_vec.size(); i++) {
    original_buff_offset = i * rempi_event::record_num;
    encoding_event = test_table->events_vec[i];    
    memcpy(original_buff + original_buff_offset, &encoding_event->mpi_inputs.front(), rempi_event::max_size);

    /*TODO
      Call "delete encoding_event" here to reduce memory footprint.
      Now, encoding_evet is freed by input_format->dealocate
     */ 

    // REMPI_DBG("Encoded  : (count: %d, flag: %d, rank: %d, with_next: %d, index: %d, msg_id: %d, gid: %d)", 
    // 	      encoding_event->get_event_counts(), 
    // 	      encoding_event->get_flag(),
    // 	      encoding_event->get_rank(),
    // 	      encoding_event->get_with_next(),
    // 	      encoding_event->get_index(),
    // 	      encoding_event->get_msg_id(),
    // 	      encoding_event->get_matching_group_id());

    // for (int j = 0; i < rempi_event::record_num; i++) {
    //   REMPI_DBG("  %d", *(original_buff + original_buff_offset + i));
    // }


#ifdef REMPI_DBG_REPLAY
    REMPI_DBG("Encoded  : (count: %d, with_next: %d, flag: %d, source: %d, clock: %d)", 
	       encoding_event->get_event_counts(), encoding_event->get_is_testsome(), encoding_event->get_flag(), 
	       encoding_event->get_source(), encoding_event->get_clock());
#endif 

  }

  if (rempi_gzip) {
    test_table->compressed_matched_events      = compression_util.compress_by_zlib((char*)original_buff, original_size, compressed_size);
    test_table->compressed_matched_events_size = compressed_size;
  } else {
    test_table->compressed_matched_events      = (char*)original_buff;
    test_table->compressed_matched_events_size = original_size;
  }

  return;
}

void rempi_encoder::write_record_file()
{
  char* whole_data;
  size_t whole_data_size;
  rempi_event *encoding_event;
  rempi_encoder_input_format_test_table *test_table;

  if (input_format->test_tables_map.size() > 1) {
    REMPI_ERR("test_table_size is %d", input_format->test_tables_map.size());
  }

  test_table = input_format->test_tables_map[0];
  whole_data      = test_table->compressed_matched_events;
  whole_data_size = test_table->compressed_matched_events_size;
  record_fs.write(whole_data, whole_data_size);
  //  write_size_vec.push_back(whole_data_size);
  total_write_size += whole_data_size;


  return;
}

void rempi_encoder::write_footer()
{}

void rempi_encoder::read_footer()
{}

int rempi_encoder::is_file_closed()
{
  return file_closed;
}

void rempi_encoder::close_record_file()
{
  // size_t total_write_size = 0;
  // for (int i = 0, n = write_size_vec.size(); i < n; i++) {
  //   total_write_size += write_size_vec[i];
  // }
  //  REMPI_DBG("EVAL Total I/O size: |%lu|", total_write_size);
  record_fs.close();
  file_closed = 1;
}

/*File to vector */
bool rempi_encoder::read_record_file(rempi_encoder_input_format *input_format)
{
  char* decoding_event_sequence;
  int* decoding_event_sequence_int;
  rempi_event* decoded_event;
  size_t size;
  bool is_no_more_record = false;

  this->input_format = input_format;

  /*TODO: 
    Currently read only one event at one this function call, 
    but this function will read an event chunk (multiple events) in future
   */
  decoding_event_sequence = (char*)malloc(rempi_event::record_num * rempi_event::record_element_size);
  record_fs.read(decoding_event_sequence, rempi_event::record_num * rempi_event::record_element_size);
  size = record_fs.gcount();
  if (size == 0) {
    free(decoding_event_sequence);
    is_no_more_record = true;
    return is_no_more_record;
  }
  decoding_event_sequence_int = (int*)decoding_event_sequence;
  
  // for (int i = 0; i < 8; i++) {
  //   fprintf(stderr, "int: %d   ==== ", decoding_event_sequence_int[i]);
  // }
  // REMPI_DBG("");

  decoded_event = rempi_event::create_test_event(
				       (int)decoding_event_sequence_int[0],
				       (int)decoding_event_sequence_int[1],
				       (int)decoding_event_sequence_int[2],
				       (int)decoding_event_sequence_int[3],
				       (int)decoding_event_sequence_int[4],
				       (int)decoding_event_sequence_int[5],
				       (int)decoding_event_sequence_int[6]
					);
#if 0
  REMPI_DBG( "Read    ; (count: %d, flag: %d, rank: %d, with_next: %d, index: %d, msg_id: %d, gid: %d)", 
	     (int)decoding_event_sequence_int[0],
	     (int)decoding_event_sequence_int[1],
	     (int)decoding_event_sequence_int[2],
	     (int)decoding_event_sequence_int[3],
	     (int)decoding_event_sequence_int[4],
	     (int)decoding_event_sequence_int[5],
	     (int)decoding_event_sequence_int[6]
	     );
#endif

  free(decoding_event_sequence);
  input_format->add(decoded_event, 0);
  is_no_more_record = false;
  total_write_size += size;
  return is_no_more_record;
}


void rempi_encoder::decode()
{
  if (rempi_gzip) {
    REMPI_ERR("gzip is not supported yet");
  }
  /*Do nothing*/
  return;
}

// /*vector to event_list*/
// void rempi_encoder::insert_encoder_input_format_chunk(rempi_event_list<rempi_event*> &recording_events, rempi_event_list<rempi_event*> &replaying_events, rempi_encoder_input_format &input_format)
// {
//   size_t size;
//   rempi_event *decoded_event;
//   rempi_encoder_input_format_test_table *test_table;

//   /*Get decoded event to replay*/
//   test_table = input_format->test_tables_map[0];
//   decoded_event = test_table->events_vec[0];
//   test_table->events_vec.pop_back();


// #ifdef REMPI_DBG_REPLAY
//   REMPI_DBGI(REMPI_DBG_REPLAY, "Decoded ; (count: %d, with_next: %d, flag: %d, source: %d, tag: %d, clock: %d)",
// 	     decoded_event->get_event_counts(), decoded_event->get_is_testsome(), decoded_event->get_flag(),
// 	     decoded_event->get_source(), decoded_event->get_tag(), decoded_event->get_clock());
// #endif

//   if (decoded_event->get_flag() == 0) {
//     /*If this is flag == 0, simply enqueue it*/
//     replaying_events.enqueue_replay(decoded_event, 0);
//     return;
//   }

  
//   rempi_event *matched_replaying_event;
//   if (decoded_event->get_flag()) {
//     while ((matched_replaying_event = pop_event_pool(decoded_event)) == NULL) { /*Pop from matched_event_pool*/
//       /*If this event is matched test, wait until this message really arrive*/
//       rempi_event *matched_event;
//       int event_list_status;
//       while (recording_events.front_replay(0) == NULL) { 
// 	/*Wait until any message arraves */
// 	usleep(100);
//       }
//       matched_event = recording_events.dequeue_replay(0, event_list_status);
//       matched_event_pool.push_back(matched_event);
// #ifdef REMPI_DBG_REPLAY
//       REMPI_DBGI(REMPI_DBG_REPLAY, "RCQ->PL ; (count: %d, with_next: %d, flag: %d, source: %d, tag: %d, clock: %d, msg_count: %d %p)",
// 		 matched_event->get_event_counts(), matched_event->get_is_testsome(), matched_event->get_flag(),
// 		 matched_event->get_source(), matched_event->get_tag(), matched_event->get_clock(), matched_event->msg_count, 
// 		 matched_event);
// #endif
//     }
//   } 

//   /* matched_replaying_event == decoded_event: this two event is same*/

//   decoded_event->msg_count = matched_replaying_event->msg_count;
// #ifdef REMPI_DBG_REPLAY
//   REMPI_DBGI(REMPI_DBG_REPLAY, "PL->RPQ ; (count: %d, with_next: %d, flag: %d, source: %d, tag: %d, clock: %d, msg_count: %d)",
// 	     decoded_event->get_event_counts(), decoded_event->get_is_testsome(), decoded_event->get_flag(),
// 	     decoded_event->get_source(), decoded_event->get_tag(), decoded_event->get_clock(), decoded_event->msg_count);
// #endif


//   replaying_events.enqueue_replay(decoded_event, 0);
//   delete matched_replaying_event;
//   return;    
// }


int rempi_encoder::progress_decoding(rempi_event_list<rempi_event*> *recording_events, rempi_event_list<rempi_event*> *replaying_events, int recv_test_id)
{
  //  REMPI_ERR("Not supported");
  return -1;
}

/*vector to event_list*/
void rempi_encoder::insert_encoder_input_format_chunk(rempi_event_list<rempi_event*> &recording_events, rempi_event_list<rempi_event*> &replaying_events)
{
  size_t size;
  rempi_event *decoded_event;
  rempi_encoder_input_format_test_table *test_table;

  /*Get decoded event to replay*/
  test_table = input_format->test_tables_map[0];
  decoded_event = test_table->events_vec[0];
  test_table->events_vec.pop_back();/*events_vec has only one event (pop_back == pop_front)*/

  // REMPI_DBGI(0, "Decoded  : (count: %d, flag: %d, rank: %d, with_next: %d, index: %d, msg_id: %d, gid: %d)", 
  //   	      decoded_event->get_event_counts(), 
  //   	      decoded_event->get_flag(),
  //   	      decoded_event->get_rank(),
  //   	      decoded_event->get_with_next(),
  //   	      decoded_event->get_index(),
  //   	      decoded_event->get_msg_id(),
  //   	      decoded_event->get_matching_group_id());

#ifdef REMPI_DBG_REPLAY
    REMPI_DBG("Decoded  : (count: %d, flag: %d, rank: %d, with_next: %d, index: %d, msg_id: %d, gid: %d)", 
	      decoded_event->get_event_counts(), 
	      decoded_event->get_flag(),
	      decoded_event->get_rank(),
	      decoded_event->get_with_next(),
	      decoded_event->get_index(),
	      decoded_event->get_msg_id(),
	      decoded_event->get_matching_group_id());
#endif

  if (decoded_event->get_flag() == 0) {
    /*If this is flag == 0, simply enqueue it*/
    replaying_events.enqueue_replay(decoded_event, 0);
    return;
  }

  replaying_events.enqueue_replay(decoded_event, 0);
  return;    
}



// void rempi_encoder::update_fd_next_clock(
// 					 int is_waiting_recv,
// 					 int num_of_recv_msg_in_next_event,
// 					 size_t interim_min_clock_in_next_event,
// 					 size_t enqueued_count,
// 					 int recv_test_id,
// 					 int is_after_recv_event)
// { 
//   //  REMPI_ERR("please remove this REMPI_ERR later");
//   return;
// }

void rempi_encoder::set_fd_clock_state(int flag)
{
  return;
}

void rempi_encoder::compute_look_ahead_recv_clock(size_t *local_min_id_clock, int *local_min_id_rank,  int recv_test_id)
{
  //  REMPI_ERR("please remove this REMPI_ERR later");
  return;
}


// char* rempi_encoder::read_decoding_event_sequence(size_t *size)
// {
//   char* decoding_event_sequence;

//   decoding_event_sequence = new char[rempi_event::max_size];
//   record_fs.read(decoding_event_sequence, rempi_event::max_size);
//   /*Set size read*/
//   *size = record_fs.gcount();
//   return decoding_event_sequence;
// }



// vector<rempi_event*> rempi_encoder::decode(char *serialized_data, size_t *size)
// {
//   vector<rempi_event*> vec;
//   int *mpi_inputs;
//   int i;

//   /*In rempi_envoder, the serialized sequence identicals to one Test event */
//   mpi_inputs = (int*)serialized_data;
//   vec.push_back(
//     new rempi_test_event(mpi_inputs[0], mpi_inputs[1], mpi_inputs[2], 
// 			 mpi_inputs[3], mpi_inputs[4], mpi_inputs[5])
//   );
//   delete mpi_inputs;
//   return vec;
// }








