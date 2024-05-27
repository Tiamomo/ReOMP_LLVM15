#include <stdio.h>
#include <stdlib.h>
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstIterator.h>
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


#include <list>
#include <vector>

#include "reomp_ir_pass.h"
#include "reomp.h"
#include "reomp_mem.h"
#include "reomp_drace.h"
#include "mutil.h"

//new
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#define REOMP_RR_RACY_LOAD  (1)
#define REOMP_RR_RACY_STORE (1)
#define REOMP_RR_RACY_CALLINST (1)
#define REOMP_RR_RACY_INVOKEINST (1)
#define REOMP_RR_CRITICAL (1)
#define REOMP_RR_OMP_LOCK (1)
#define REOMP_RR_OTHER_LOCK (1)
#define REOMP_RR_REDUCTION (1)
#define REOMP_RR_SINGLE (1)
#define REOMP_RR_MASTER (1)
#define REOMP_RR_ATOMICOP (1)
#define REOMP_RR_ATOMICLOAD (1)
#define REOMP_RR_ATOMICSTORE (1)
/* Flag for debugging flag */
#define REOMP_RR_FORK_CALL (0)



//-----------------------------------------------------------------------------
// New PM implementation
//-----------------------------------------------------------------------------
PreservedAnalyses ReOMP::run(Function& F, FunctionAnalysisManager&) {
    //errs() << "Function name is " << F.getName() << "!\n";
    //doInitialization
    if (!stop) {
        Module* M = F.getParent();
        errs() << "M name is " << (*M).getName() << "!\n";
        REOMP_M = M;
        REOMP_CTX = &((*M).getContext());
        REOMP_DL = new DataLayout(REOMP_M);
        init_inserted_functions((*M));
        reomp_drace_parse(REOMP_DRACE_LOG_ARCHER);
        stop = 1;
    }
    else {
        //errs() << "stop is not 0!\n";
    }
    //runOnFunction
    int modified_counter = 0;
    modified_counter += handle_function(F);
    for (BasicBlock& BB : F) {
        modified_counter += handle_basicblock(F, BB);
        for (Instruction& I : BB) {
            modified_counter += handle_instruction(F, BB, I);
        }
    }
    return PreservedAnalyses::all();
}

bool ReOMP::isRequired() { return true; }

void ReOMP::insert_func(Instruction* I, BasicBlock* BB, int offset, int control, Value* ptr, Value* size)
{
    //errs() << "======15.0-->Enter ReOMP::insert_func=======" << "\n";
    //MUTIL_DBG("======15.0-->Enter ReOMP::insert_func===offset: %d,control: %d ====", offset,control);
    //I->print(errs());
    //errs() << "\n";
    /*if (auto *const_int = llvm::dyn_cast<llvm::ConstantInt>(size)){
       size->print(llvm::outs());
       errs() << "\n";
    }*/
    vector<Value*> arg_vec;
    IRBuilder<> builder(I);
    builder.SetInsertPoint(BB, (offset) ? ++builder.GetInsertPoint() : builder.GetInsertPoint());
    FunctionCallee func = reomp_func_umap.at(REOMP_CONTROL_STR);
    arg_vec.push_back(REOMP_CONST_INT32TY(control));
    if (!ptr) ptr = REOMP_CONST_INT64PTRTY_NULL;// ConstantPointerNull::get(Type::getInt64PtrTy(*REOMP_CTX));
    arg_vec.push_back(ptr);
    if (!size) size = REOMP_CONST_INT32TY(0); //ConstantInt::get(Type::getInt64Ty(*REOMP_CTX), 0);
    arg_vec.push_back(size);
    builder.CreateCall(func, arg_vec);
    return;
}



/* Outer Handlers */
int ReOMP::handle_function(Function& F)
{
    int modified_counter = 0;
    modified_counter += handle_function_on_main(F);
    return modified_counter;
}

int ReOMP::handle_basicblock(Function& F, BasicBlock& BB)
{
    return 0;
}

int ReOMP::handle_function_on_main(Function& F)
{
    int modified_counter = 0;
    int is_hook_enabled = 0;
    size_t num_locks;
    int max_lock_id;
    if (F.getName() == "main") {
        for (BasicBlock& BB : F) {
            for (Instruction& I : BB) {
                if (!is_hook_enabled) {
                    num_locks = reomp_drace_get_num_locks();
		    //max_lock_id = max_of_lock_id()+1;
		    //MUTIL_DBG("####handle_function_on_main-> max_lock_id+1:%d", max_lock_id);
                    //insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(max_lock_id));
                    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(num_locks));
                    modified_counter++;
                    is_hook_enabled = 1;
                }

                if (dyn_cast<ReturnInst>(&I)) {
		    //insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(max_lock_id));
                    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_AFT_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(num_locks));
                    modified_counter++;
                }

            }
        }
    }
    return modified_counter;
}


int ReOMP::handle_instruction(Function& F, BasicBlock& BB, Instruction& I)
{

    int modified_counter = 0;
    modified_counter += handle_instruction_on_critical(F, BB, I); /* insert lock/unlock for omp_ciritcal/reduction*/

    modified_counter += handle_instruction_on_load_store(F, BB, I); /* */
    return modified_counter;
}


int ReOMP::handle_instruction_on_reduction(Function& F, BasicBlock& BB, Instruction& I)
{

    int modified_counter = 0;
    int is_after_reduction_begin = 0;
    string name;
    CallInst* CI = NULL;
    AtomicRMWInst* ARMWI;
    AtomicCmpXchgInst* ACXI;

    //  BasicBlock tmp_BB = BB;
    //  Instruction tmp_I = I;

    /* Instrument to before __kmpc_reduce and __kmpc_reduce_nowait */
    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_REDUCE_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), NULL);
    modified_counter++;
    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
    modified_counter++;

    /* Instrument to __kmpc_end_reduce and __kmpc_end_reduce_nowait
                   or atomic operations
    */
    Function::iterator BB_it = F.begin();
    Function::iterator BB_it_end = F.end();

    //
    //BasicBlock* B1 = BB_it->getBasicBlock();

    for (; BB_it != BB_it_end; BB_it++) {
        BasicBlock::iterator IN_it = BB_it->begin();
        BasicBlock::iterator IN_it_end = BB_it->end();
        for (; IN_it != IN_it_end; IN_it++) {
            if ((CI = dyn_cast<CallInst>(&I)) != NULL) {
                name = CI->getCalledFunction()->getName().str();
                if (name == "__kmpc_reduce" || name == "__kmpc_reduce_nowait") {
                    is_after_reduction_begin = 1;
                    //
                //B1= BB_it->getBasicBlock();
                }
            }
            if (!is_after_reduction_begin) continue;

            Instruction* IN = &(*IN_it);
            //      errs() << " >>>>> " << IN->getCalledValue()->getName() << "\n"; 
            //      IN->print(errs());
            //      errs() << "\n";
            if ((CI = dyn_cast<CallInst>(IN)) != NULL) {
                name = CI->getCalledFunction()->getName().str();
                if (name == "__kmpc_end_reduce" || name == "__kmpc_end_reduce_nowait") {
                    BasicBlock& bb = *BB_it;
                    /* Instrument to __kmpc_end_reduce or __kmpc_end_reduce_nowait */
                    //insert_func(CI, &bb, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(1));
                    insert_func(CI, &bb, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                    modified_counter++;
                    insert_func(CI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                    //insert_func(CI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(1));
                    modified_counter++;
                }
            }
            else if ((ARMWI = dyn_cast<AtomicRMWInst>(IN)) != NULL) {
                //errs() << "ReOMP::handle_instruction_on_reduction---> bb Instruction IN-->" << "\n"; 
        //errs() << *IN<< "\n"; 
                //errs() << " ReOMP::handle_instruction_on_reduction-->Instruction I-->" << "\n";
            //errs() << I << "\n";  
            //lable2
                BasicBlock& bb = *BB_it;
                insert_func(ARMWI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                //insert_func(ARMWI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(2));
            //insert_func(ARMWI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(2));
                modified_counter++;
            }
            else if ((ACXI = dyn_cast<AtomicCmpXchgInst>(IN)) != NULL) {
                BasicBlock& bb = *BB_it;
                insert_func(ACXI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                modified_counter++;
            }
            if (modified_counter == 5) break;
        }
        if (modified_counter == 5) break;
    }
    if (modified_counter != 5) {
        MUTIL_ERR("Modified counter is not 5 on reduction");
    }
    return modified_counter;
}

int ReOMP::handle_instruction_on_critical(Function& F, BasicBlock& BB, Instruction& I)
{
  //  MUTIL_DBG("Here is handle_instruction_on_critical");
    int modified_counter = 0;
    string name;
    if (CallInst* CI = dyn_cast<CallInst>(&I)) {
        //errs() << "current ins:  " << I << "\n";	
        if (CI == nullptr)
        {
            //errs() << "CI --> current ins:  " << I << "\n";
            //MUTIL_ERR("CI is NULL");
        }
        else if (CI->getCalledFunction() == nullptr)
        {
            //errs() << "CI->getCalledFunction() is null --> current ins:  " << I << "\n";
            Value* func_ptr = CI->getCalledOperand();
            if (Function* func = dyn_cast<Function>(func_ptr->stripPointerCasts())) {
                StringRef func_name = func->getName();
                errs() << "Function pointer calls function: " << func_name << "\n";
            }
            else {
                errs() << "Failed to extract function name\n";
            }

        }
        else {
            name = CI->getCalledFunction()->getName().str();
        }

        if (name == "__kmpc_critical" && REOMP_RR_CRITICAL) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CRITICAL), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if (name == "__kmpc_end_critical" && REOMP_RR_CRITICAL) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CRITICAL), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if ((name == "omp_set_lock" || name == "omp_set_nest_lock") && REOMP_RR_OMP_LOCK) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OMP_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if ((name == "omp_unset_lock" || name == "omp_unset_nest_lock") && REOMP_RR_OMP_LOCK) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OMP_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if (name == "semop" && REOMP_RR_OTHER_LOCK) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OTHER_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OTHER_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
        else if ((name == "__kmpc_reduce" || name == "__kmpc_reduce_nowait") && REOMP_RR_REDUCTION) {
            modified_counter = this->handle_instruction_on_reduction(F, BB, I);
        }
        else if (name == "__kmpc_single" && REOMP_RR_SINGLE) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_SINGLE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_SINGLE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if (name == "__kmpc_master" && REOMP_RR_MASTER) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MASTER), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MASTER), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if ((name == "__kmpc_end_single" && REOMP_RR_SINGLE) ||
            (name == "__kmpc_end_master" && REOMP_RR_MASTER)) {
            /*
           __kmpc_end_single/master is executed by an only thread executing __kmpc_single/master
           Therefore, DO NOT insert reomp_control after __lmpc_end_single/master
            */

        }
        else if (name == "exit") {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_AFT_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(REOMP_RR_LOCK_NULL));
            modified_counter = 1;
        }
        else if (name == "__kmpc_fork_call" && REOMP_RR_FORK_CALL) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_OTHER, REOMP_CONST_INT64TY(0), REOMP_CONST_INT32TY(REOMP_RR_LOCK_NULL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_OTHER, REOMP_CONST_INT64TY(1), REOMP_CONST_INT32TY(REOMP_RR_LOCK_NULL));
            modified_counter = 1;
        }
    }
    else if (AtomicRMWInst* ARMWI = dyn_cast<AtomicRMWInst>(&I)) {
        if (REOMP_RR_ATOMICOP) {
            //errs() << "ReOMP::handle_instruction_on_critical-->first " << "\n"; 
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            //errs() << "ReOMP::handle_instruction_on_critical-->second " << "\n"; 
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (AtomicCmpXchgInst* ACXI = dyn_cast<AtomicCmpXchgInst>(&I)) {
        if (REOMP_RR_ATOMICOP) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (StoreInst* SI = dyn_cast<StoreInst>(&I)) {
        if (SI->isAtomic() && REOMP_RR_ATOMICSTORE) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICSTORE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICSTORE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (LoadInst* LI = dyn_cast<LoadInst>(&I)) {
        if (LI->isAtomic() && REOMP_RR_ATOMICLOAD) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICLOAD), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICLOAD), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (I.isAtomic()) {
        MUTIL_ERR("Missing a atomic");
    }

    //  if (modified_counter > 0) MUTIL_DBG("INS: %s", name.c_str());    
    return modified_counter;
}

int ReOMP::is_data_racy_access(Function* F, Instruction* I, uint64_t* access)
{

    int lock_id;
    unsigned line, column;
    const char* filename, * dirname;
    if (const DebugLoc& dbloc = I->getDebugLoc()) {
        line = dbloc.getLine();
        column = dbloc.getCol();
        if (DIScope* discope = dyn_cast<DIScope>(dbloc.getScope())) {
            filename = discope->getFilename().data();
            dirname = discope->getDirectory().data();
        }
        else {
            MUTIL_ERR("Third operand of DebugLoc is not DIScope");
        }
        //errs() << "current ins: \n";
        //I->print(errs());
        //errs() << "\n";
        //errs() << "the debuginfo : line: " << line << " colum: " << column << " dirname:  " << dirname << " filename:  " << filename << "\n";

        if ((lock_id = reomp_drace_is_data_race(F->getName().data(), dirname, filename, line, column, access)) > 0) {
            return lock_id;
        }
    }
    else if (NULL == I->getDebugLoc()) {
        //errs() << "the return is NULL.>>>>>\n ";
    }
    return 0;
}

int ReOMP::handle_instruction_on_load_store(Function& F, BasicBlock& BB, Instruction& I)
{

    size_t lock_id = 0;
    int modified_counter = 0;
    uint64_t access;
    if (StoreInst* SI = dyn_cast<StoreInst>(&I)) {
        if (!REOMP_RR_RACY_STORE) return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
	    //MUTIL_DBG("~~StoreInst~DT~lock_id ->: %d",lock_id);
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_STORE), REOMP_CONST_INT32TY(lock_id));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_STORE), REOMP_CONST_INT32TY(lock_id));
            modified_counter += 2;
        }
    }
    else if (LoadInst* LI = dyn_cast<LoadInst>(&I)) {
        if (!REOMP_RR_RACY_LOAD) return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
            //MUTIL_DBG("~LoadInst~~DT~lock_id ->: %d",lock_id);
	    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_LOAD), REOMP_CONST_INT32TY(lock_id));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_LOAD), REOMP_CONST_INT32TY(lock_id));

            modified_counter += 2;
        }
    }
    else if (CallInst* CI = dyn_cast<CallInst>(&I)) {
        if (!REOMP_RR_RACY_CALLINST) return modified_counter;

        string name;// = CI->getCalledFunction()->getName().str();

        if (CI == nullptr)
        {
            //errs() << "CI --> current ins:  " << I << "\n";
            //MUTIL_ERR("CI is NULL");
        }
        else if (CI->getCalledFunction() == nullptr)
        {
            //errs() << "CI->getCalledFunction() is null --> current ins:  " << I << "\n";
            Value* func_ptr = CI->getCalledOperand();
            if (Function* func = dyn_cast<Function>(func_ptr->stripPointerCasts())) {
                StringRef func_name = func->getName();
                errs() << "Function pointer calls function: " << func_name << "\n";
            }
            else {
                errs() << "Failed to extract function name\n";
            }

        }
        else {
            name = CI->getCalledFunction()->getName().str();
        }

        if (name == "reomp_control") return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
	    //MUTIL_DBG("~~CallInst~DT~lock_id ->: %d",lock_id);
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));
            modified_counter += 2;
        }
    }
    else if (InvokeInst* II = dyn_cast<InvokeInst>(&I)) {
        if (!REOMP_RR_RACY_INVOKEINST) return modified_counter;
        BasicBlock* nextBB;
        Instruction* frontIN;
        string name; //= II->getCalledFunction()->getName().str();

        if (CI == nullptr)
        {
            //errs() << "CI --> current ins:  " << I << "\n";
            //errs() << ("CI is NULL\n");
        }
        else if (CI->getCalledFunction() == nullptr)
        {
            //errs() << "CI->getCalledFunction() is null --> current ins:  " << I << "\n";
            Value* func_ptr = CI->getCalledOperand();
            if (Function* func = dyn_cast<Function>(func_ptr->stripPointerCasts())) {
                StringRef func_name = func->getName();
                errs() << "Function pointer calls function: " << func_name << "\n";
            }
            else {
                errs() << "Failed to extract function name\n";
            }

        }
        else {
            name = CI->getCalledFunction()->getName().str();
        }

        if (name == "reomp_control") return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
	    //MUTIL_DBG("~~InvokeInst~DT~lock_id ->: %d",lock_id);
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));

            nextBB = II->getNormalDest();
            frontIN = &(nextBB->front());
            insert_func(frontIN, nextBB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));

            nextBB = II->getUnwindDest();
            frontIN = &(nextBB->front());
            //NOTE: clang hangs when instrumenting reomp_control in unwind basicblock.
            insert_func(frontIN, nextBB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), NULL);

            modified_counter += 3;
        }
    }
    //  if (modified_counter > 0)  MUTIL_DBG("INS: race");
    return modified_counter;
}


void ReOMP::init_inserted_functions(Module& M)
{

    LLVMContext& ctx = M.getContext();

    reomp_func_umap[REOMP_CONTROL_STR] = M.getOrInsertFunction(REOMP_CONTROL_STR,
        Type::getVoidTy(ctx),
        Type::getInt32Ty(ctx),
        Type::getInt64Ty(ctx),
        Type::getInt32Ty(ctx));

    return;
}

//-----------------------------------------------------------------------------
// New PM Registration //for clang++
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getReOMPIRPluginInfo() {
    return { LLVM_PLUGIN_API_VERSION, "reompir", LLVM_VERSION_STRING,
            [](PassBuilder& PB) {
              PB.registerPipelineStartEPCallback(
                [](ModulePassManager& MPM, OptimizationLevel Level) {
                   FunctionPassManager FPM;
                   std::unique_ptr<ReOMP> hwobj = std::make_unique<ReOMP>();
                   FPM.addPass(std::move(*hwobj));
                   MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
              });

           }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getReOMPIRPluginInfo();
}

//-----------------------------------------------------------------------------
// Legacy PM implementation
//-----------------------------------------------------------------------------
void LegacyReOMP::insert_func(Instruction* I, BasicBlock* BB, int offset, int control, Value* ptr, Value* size)
{
    errs() << "======15.0-->Enter ReOMP::insert_func=======" << "\n";
    I->print(errs());
    errs() << "\n";
    vector<Value*> arg_vec;
    IRBuilder<> builder(I);
    builder.SetInsertPoint(BB, (offset) ? ++builder.GetInsertPoint() : builder.GetInsertPoint());
    FunctionCallee func = reomp_func_umap.at(REOMP_CONTROL_STR);
    arg_vec.push_back(REOMP_CONST_INT32TY(control));
    if (!ptr) ptr = REOMP_CONST_INT64PTRTY_NULL;// ConstantPointerNull::get(Type::getInt64PtrTy(*REOMP_CTX));
    arg_vec.push_back(ptr);
    if (!size) size = REOMP_CONST_INT32TY(0); //ConstantInt::get(Type::getInt64Ty(*REOMP_CTX), 0);
    arg_vec.push_back(size);
    builder.CreateCall(func, arg_vec);
    return;
}

/* Outer Handlers */
int LegacyReOMP::handle_function(Function& F)
{
    int modified_counter = 0;
    modified_counter += handle_function_on_main(F);
    return modified_counter;
}

int LegacyReOMP::handle_basicblock(Function& F, BasicBlock& BB)
{
    return 0;
}

int LegacyReOMP::handle_function_on_main(Function& F)
{
    int modified_counter = 0;
    int is_hook_enabled = 0;
    size_t num_locks;
    if (F.getName() == "main") {
        for (BasicBlock& BB : F) {
            for (Instruction& I : BB) {
                if (!is_hook_enabled) {
                    num_locks = reomp_drace_get_num_locks();
                    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(num_locks));
                    modified_counter++;
                    is_hook_enabled = 1;
                }

                if (dyn_cast<ReturnInst>(&I)) {
                    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_AFT_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(num_locks));
                    modified_counter++;
                }

            }
        }
    }
    return modified_counter;
}

int LegacyReOMP::handle_instruction(Function& F, BasicBlock& BB, Instruction& I)
{

    int modified_counter = 0;
    modified_counter += handle_instruction_on_critical(F, BB, I); /* insert lock/unlock for omp_ciritcal/reduction*/
    modified_counter += handle_instruction_on_load_store(F, BB, I); /* */
    return modified_counter;
}

int LegacyReOMP::handle_instruction_on_reduction(Function& F, BasicBlock& BB, Instruction& I)
{

    int modified_counter = 0;
    int is_after_reduction_begin = 0;
    string name;
    CallInst* CI = NULL;
    AtomicRMWInst* ARMWI;
    AtomicCmpXchgInst* ACXI;

    //  BasicBlock tmp_BB = BB;
    //  Instruction tmp_I = I;

    /* Instrument to before __kmpc_reduce and __kmpc_reduce_nowait */
    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_REDUCE_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), NULL);
    modified_counter++;
    insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
    modified_counter++;

    /* Instrument to __kmpc_end_reduce and __kmpc_end_reduce_nowait
                   or atomic operations
    */
    Function::iterator BB_it = F.begin();
    Function::iterator BB_it_end = F.end();

    //
    //BasicBlock* B1 = BB_it->getBasicBlock();

    for (; BB_it != BB_it_end; BB_it++) {
        BasicBlock::iterator IN_it = BB_it->begin();
        BasicBlock::iterator IN_it_end = BB_it->end();
        for (; IN_it != IN_it_end; IN_it++) {
            if ((CI = dyn_cast<CallInst>(&I)) != NULL) {
                name = CI->getCalledFunction()->getName().str();
                if (name == "__kmpc_reduce" || name == "__kmpc_reduce_nowait") {
                    is_after_reduction_begin = 1;
                    //
                //B1= BB_it->getBasicBlock();
                }
            }
            if (!is_after_reduction_begin) continue;

            Instruction* IN = &(*IN_it);
            //      errs() << " >>>>> " << IN->getCalledValue()->getName() << "\n"; 
            //      IN->print(errs());
            //      errs() << "\n";
            if ((CI = dyn_cast<CallInst>(IN)) != NULL) {
                name = CI->getCalledFunction()->getName().str();
                if (name == "__kmpc_end_reduce" || name == "__kmpc_end_reduce_nowait") {
                    BasicBlock& bb = *BB_it;
                    /* Instrument to __kmpc_end_reduce or __kmpc_end_reduce_nowait */
                    //insert_func(CI, &bb, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(1));
                    insert_func(CI, &bb, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                    modified_counter++;
                    insert_func(CI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                    //insert_func(CI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(1));
                    modified_counter++;
                }
            }
            else if ((ARMWI = dyn_cast<AtomicRMWInst>(IN)) != NULL) {
                //errs() << "ReOMP::handle_instruction_on_reduction---> bb Instruction IN-->" << "\n"; 
        //errs() << *IN<< "\n"; 
                //errs() << " ReOMP::handle_instruction_on_reduction-->Instruction I-->" << "\n";
            //errs() << I << "\n";  
            //lable2
                BasicBlock& bb = *BB_it;
                insert_func(ARMWI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                //insert_func(ARMWI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(2));
            //insert_func(ARMWI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), REOMP_CONST_INT32TY(2));
                modified_counter++;
            }
            else if ((ACXI = dyn_cast<AtomicCmpXchgInst>(IN)) != NULL) {
                BasicBlock& bb = *BB_it;
                insert_func(ACXI, &bb, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_REDUCE_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_REDUCTION), &I);
                modified_counter++;
            }
            if (modified_counter == 5) break;
        }
        if (modified_counter == 5) break;
    }
    if (modified_counter != 5) {
        MUTIL_ERR("Modified counter is not 5 on reduction");
    }
    return modified_counter;
}

int LegacyReOMP::handle_instruction_on_critical(Function& F, BasicBlock& BB, Instruction& I)
{

    int modified_counter = 0;
    string name;
    if (CallInst* CI = dyn_cast<CallInst>(&I)) {
        //errs() << "current ins:  " << I << "\n";	
        if (CI == nullptr)
        {
            //errs() << "CI --> current ins:  " << I << "\n";
            //MUTIL_ERR("CI is NULL");
        }
        else if (CI->getCalledFunction() == nullptr)
        {
            //errs() << "CI->getCalledFunction() is null --> current ins:  " << I << "\n";
            Value* func_ptr = CI->getCalledOperand();
            if (Function* func = dyn_cast<Function>(func_ptr->stripPointerCasts())) {
                StringRef func_name = func->getName();
                errs() << "Function pointer calls function: " << func_name << "\n";
            }
            else {
                errs() << "Failed to extract function name\n";
            }

        }
        else {
            name = CI->getCalledFunction()->getName().str();
        }

        if (name == "__kmpc_critical" && REOMP_RR_CRITICAL) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CRITICAL), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if (name == "__kmpc_end_critical" && REOMP_RR_CRITICAL) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CRITICAL), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if ((name == "omp_set_lock" || name == "omp_set_nest_lock") && REOMP_RR_OMP_LOCK) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OMP_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if ((name == "omp_unset_lock" || name == "omp_unset_nest_lock") && REOMP_RR_OMP_LOCK) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OMP_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if (name == "semop" && REOMP_RR_OTHER_LOCK) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OTHER_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_OTHER_LOCK), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
        else if ((name == "__kmpc_reduce" || name == "__kmpc_reduce_nowait") && REOMP_RR_REDUCTION) {
            modified_counter = this->handle_instruction_on_reduction(F, BB, I);
        }
        else if (name == "__kmpc_single" && REOMP_RR_SINGLE) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_SINGLE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_SINGLE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if (name == "__kmpc_master" && REOMP_RR_MASTER) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_BEF_CRITICAL_BEGIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MASTER), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_AFT_CRITICAL_END, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MASTER), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 1;
        }
        else if ((name == "__kmpc_end_single" && REOMP_RR_SINGLE) ||
            (name == "__kmpc_end_master" && REOMP_RR_MASTER)) {
            /*
           __kmpc_end_single/master is executed by an only thread executing __kmpc_single/master
           Therefore, DO NOT insert reomp_control after __lmpc_end_single/master
            */

        }
        else if (name == "exit") {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_AFT_MAIN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_MAIN), REOMP_CONST_INT32TY(REOMP_RR_LOCK_NULL));
            modified_counter = 1;
        }
        else if (name == "__kmpc_fork_call" && REOMP_RR_FORK_CALL) {
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_OTHER, REOMP_CONST_INT64TY(0), REOMP_CONST_INT32TY(REOMP_RR_LOCK_NULL));
            insert_func(CI, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_OTHER, REOMP_CONST_INT64TY(1), REOMP_CONST_INT32TY(REOMP_RR_LOCK_NULL));
            modified_counter = 1;
        }
    }
    else if (AtomicRMWInst* ARMWI = dyn_cast<AtomicRMWInst>(&I)) {
        if (REOMP_RR_ATOMICOP) {
            //errs() << "ReOMP::handle_instruction_on_critical-->first " << "\n"; 
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            //errs() << "ReOMP::handle_instruction_on_critical-->second " << "\n"; 
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (AtomicCmpXchgInst* ACXI = dyn_cast<AtomicCmpXchgInst>(&I)) {
        if (REOMP_RR_ATOMICOP) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICOP), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (StoreInst* SI = dyn_cast<StoreInst>(&I)) {
        if (SI->isAtomic() && REOMP_RR_ATOMICSTORE) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICSTORE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICSTORE), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (LoadInst* LI = dyn_cast<LoadInst>(&I)) {
        if (LI->isAtomic() && REOMP_RR_ATOMICLOAD) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICLOAD), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_ATOMICLOAD), REOMP_CONST_INT32TY(REOMP_RR_LOCK_GLOBAL));
            modified_counter = 2;
        }
    }
    else if (I.isAtomic()) {
        MUTIL_ERR("Missing a atomic");
    }

    //  if (modified_counter > 0) MUTIL_DBG("INS: %s", name.c_str());    
    return modified_counter;
}

int LegacyReOMP::is_data_racy_access(Function* F, Instruction* I, uint64_t* access)
{

    int lock_id;
    unsigned line, column;
    const char* filename, * dirname;
    if (const DebugLoc& dbloc = I->getDebugLoc()) {
        line = dbloc.getLine();
        column = dbloc.getCol();
        if (DIScope* discope = dyn_cast<DIScope>(dbloc.getScope())) {
            filename = discope->getFilename().data();
            dirname = discope->getDirectory().data();
        }
        else {
            MUTIL_ERR("Third operand of DebugLoc is not DIScope");
        }
        //errs() << "current ins: \n";
        //I->print(errs());
        //errs() << "\n";
        //errs() << "the debuginfo : line: " << line << " colum: " << column << " dirname:  " << dirname << " filename:  " << filename << "\n";

        if ((lock_id = reomp_drace_is_data_race(F->getName().data(), dirname, filename, line, column, access)) > 0) {
            return lock_id;
        }
    }
    else if (NULL == I->getDebugLoc()) {
        //errs() << "the return is NULL.>>>>>\n ";
    }
    return 0;
}

int LegacyReOMP::handle_instruction_on_load_store(Function& F, BasicBlock& BB, Instruction& I)
{

    size_t lock_id = 0;
    int modified_counter = 0;
    uint64_t access;
    if (StoreInst* SI = dyn_cast<StoreInst>(&I)) {
        if (!REOMP_RR_RACY_STORE) return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_STORE), REOMP_CONST_INT32TY(lock_id));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_STORE), REOMP_CONST_INT32TY(lock_id));
            modified_counter += 2;
        }
    }
    else if (LoadInst* LI = dyn_cast<LoadInst>(&I)) {
        if (!REOMP_RR_RACY_LOAD) return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_LOAD), REOMP_CONST_INT32TY(lock_id));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_LOAD), REOMP_CONST_INT32TY(lock_id));

            modified_counter += 2;
        }
    }
    else if (CallInst* CI = dyn_cast<CallInst>(&I)) {
        if (!REOMP_RR_RACY_CALLINST) return modified_counter;

        string name;// = CI->getCalledFunction()->getName().str();

        if (CI == nullptr)
        {
            //errs() << "CI --> current ins:  " << I << "\n";
            //MUTIL_ERR("CI is NULL");
        }
        else if (CI->getCalledFunction() == nullptr)
        {
            //errs() << "CI->getCalledFunction() is null --> current ins:  " << I << "\n";
            Value* func_ptr = CI->getCalledOperand();
            if (Function* func = dyn_cast<Function>(func_ptr->stripPointerCasts())) {
                StringRef func_name = func->getName();
                errs() << "Function pointer calls function: " << func_name << "\n";
            }
            else {
                errs() << "Failed to extract function name\n";
            }

        }
        else {
            name = CI->getCalledFunction()->getName().str();
        }

        if (name == "reomp_control") return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_AFTER, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));
            modified_counter += 2;
        }
    }
    else if (InvokeInst* II = dyn_cast<InvokeInst>(&I)) {
        if (!REOMP_RR_RACY_INVOKEINST) return modified_counter;
        BasicBlock* nextBB;
        Instruction* frontIN;
        string name; //= II->getCalledFunction()->getName().str();

        if (CI == nullptr)
        {
            //errs() << "CI --> current ins:  " << I << "\n";
            //errs() << ("CI is NULL\n");
        }
        else if (CI->getCalledFunction() == nullptr)
        {
            //errs() << "CI->getCalledFunction() is null --> current ins:  " << I << "\n";
            Value* func_ptr = CI->getCalledOperand();
            if (Function* func = dyn_cast<Function>(func_ptr->stripPointerCasts())) {
                StringRef func_name = func->getName();
                errs() << "Function pointer calls function: " << func_name << "\n";
            }
            else {
                errs() << "Failed to extract function name\n";
            }

        }
        else {
            name = CI->getCalledFunction()->getName().str();
        }

        if (name == "reomp_control") return modified_counter;
        if ((lock_id = this->is_data_racy_access(&F, &I, &access)) != 0) {
            insert_func(&I, &BB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_IN, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));

            nextBB = II->getNormalDest();
            frontIN = &(nextBB->front());
            insert_func(frontIN, nextBB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), REOMP_CONST_INT32TY(lock_id));

            nextBB = II->getUnwindDest();
            frontIN = &(nextBB->front());
            //NOTE: clang hangs when instrumenting reomp_control in unwind basicblock.
            insert_func(frontIN, nextBB, REOMP_IR_PASS_INSERT_BEFORE, REOMP_GATE_OUT, REOMP_CONST_INT64TY(REOMP_RR_TYPE_CPP_STL), NULL);

            modified_counter += 3;
        }
    }
    //  if (modified_counter > 0)  MUTIL_DBG("INS: race");
    return modified_counter;
}

void LegacyReOMP::init_inserted_functions(Module& M)
{

    LLVMContext& ctx = M.getContext();

    reomp_func_umap[REOMP_CONTROL_STR] = M.getOrInsertFunction(REOMP_CONTROL_STR,
        Type::getVoidTy(ctx),
        Type::getInt32Ty(ctx),
        Type::getInt64Ty(ctx),
        Type::getInt32Ty(ctx));

    return;
}

bool LegacyReOMP::doInitialization(Module& M)
{

    REOMP_M = &M;
    REOMP_CTX = &(M.getContext());
    REOMP_DL = new DataLayout(REOMP_M);
    init_inserted_functions(M);
    reomp_drace_parse(REOMP_DRACE_LOG_ARCHER);
    return true;
}

bool LegacyReOMP::runOnFunction(Function& F)
{

    int modified_counter = 0;
    modified_counter += handle_function(F);
    for (BasicBlock& BB : F) {
        modified_counter += handle_basicblock(F, BB);
        for (Instruction& I : BB) {
            modified_counter += handle_instruction(F, BB, I);
        }
    }
    return modified_counter > 0;
}

void LegacyReOMP::getAnalysisUsage(AnalysisUsage& AU) const
{
    //  AU.setPreservesCFG();
    // addReauired cause errors
    //  AU.addRequired<DependenceAnalysisWrapperPass>();
}
//-----------------------------------------------------------------------------
// Legacy PM Registration
//-----------------------------------------------------------------------------
char LegacyReOMP::ID = 0;
//for opt
static RegisterPass<LegacyReOMP> X("legacyreomp", "LegacyReOMP");


