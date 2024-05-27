#ifndef MUTIL_H__
#define MUTIL_H__

#include <string>
#include <assert.h>
#include <stdlib.h>


#define MUTIL_FUNC_(prefix, func)   prefix ## _ ## func
/*Mmodify below two #define */
#define MUTIL_FUNC(func)   MUTIL_FUNC_(reomp_util, func)
#define MUTIL_PREFIX "REOMP"

extern int mutil_my_rank;
extern char mutil_hostname[256];

#define MUTIL_ASSERT(b)				\
  do {						\
    MUTIL_FUNC(assert)(b);			\
  } while(0)					\


/*if __VA_ARGS__ is empty, the previous comma can be removed by "##" statement*/
#define MUTIL_ERR(err_fmt, ...)				\
  do {							\
    MUTIL_FUNC(err)(" "					\
		    err_fmt				\
		    " (%s:%s:%d)",			\
		    ## __VA_ARGS__,			\
		    __FILE__, __func__, __LINE__);	\
  } while(0)

#define MUTIL_DBG(dbg_fmt, ...)			\
  do {						\
    MUTIL_FUNC(dbg)(" "				\
		    dbg_fmt			\
		    " (%s:%d)",			\
		    ## __VA_ARGS__,		\
		    __FILE__, __LINE__);	\
  } while(0)

#define MUTIL_PRT(prt_fmt, ...)					\
  do {								\
    MUTIL_FUNC(print)(" "	prt_fmt, ## __VA_ARGS__);	\
  } while(0)

/*if __VA_ARGS__ is empty, the previous comma can be removed by "##" statement*/
#define MUTIL_DBGI(i, dbg_fmt, ...)		\
  do {						\
    MUTIL_FUNC(dbgi)(i,				\
		     " "			\
		     dbg_fmt			\
		     " (%s:%d)",		\
		     ## __VA_ARGS__,		\
		     __FILE__, __LINE__);	\
  } while(0)

//#define ENABLE_TIMER
#define MUTIL_TIMER_START     (0)
#define MUTIL_TIMER_STOP      (1)
#define MUTIL_TIMER_GET_TIME  (2)
#ifdef ENABLE_TIMER
#define MUTIL_TIMER(mode, timerIndex, time) \
  do { \
    MUTIL_FUNC(timer)(mode, timerIndex, time, (char*)__FILE__, __LINE__);	\
  } while(0)
#else
#define MUTIL_TIMER(mode, timerIndex, time)
#endif

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

void MUTIL_FUNC(set_configuration)(int *argc, char ***argv);
void MUTIL_FUNC(assert)(int b);
void MUTIL_FUNC(init)(int r);
void MUTIL_FUNC(err)(const char* fmt, ...);
void MUTIL_FUNC(print)(const char* fmt, ...);
void MUTIL_FUNC(erri)(int r, const char* fmt, ...);
void MUTIL_FUNC(alert)(const char* fmt, ...);
void MUTIL_FUNC(dbg_log_print)();
void MUTIL_FUNC(dbg)(const char* fmt, ...);
void MUTIL_FUNC(dbgi)(int r, const char* fmt, ...);


  //string MUTIL_FUNC(btrace_string)();
void MUTIL_FUNC(btrace)();
int MUTIL_FUNC(btrace_get)(char*** addresses, int len);
size_t MUTIL_FUNC(btrace_hash)();
void MUTIL_FUNC(btrace_line)();
void MUTIL_FUNC(exit)(int no);
void* MUTIL_FUNC(malloc)(size_t size);
void* MUTIL_FUNC(realloc)(void *ptr, size_t size);
void MUTIL_FUNC(free)(void* addr);
double MUTIL_FUNC(get_time)(void);
void MUTIL_FUNC(timer)(int mode, int timerIndex, double *time, char* file, int line);
void MUTIL_FUNC(dbg_sleep_sec)(int sec);
void MUTIL_FUNC(sleep_sec)(int sec);
void MUTIL_FUNC(dbg_sleep_usec)(int sec);
void MUTIL_FUNC(sleep_usec)(int sec);
unsigned int MUTIL_FUNC(hash)(unsigned int original_val, unsigned int new_val);
unsigned int MUTIL_FUNC(hash_str)(const char* c, unsigned int length);
int MUTIL_FUNC(init_rand)(int seed);
int MUTIL_FUNC(init_ndrand)();
int MUTIL_FUNC(get_rand)(int max);
int MUTIL_FUNC(str_starts_with)(const char* base, const char* str);
int MUTIL_FUNC(str_ends_with)(const char* str, const char* suffix);
size_t Backtrace(void **frames, size_t size);
void PrintBacktrace(void);

#ifdef __cplusplus
}
#endif


#endif
