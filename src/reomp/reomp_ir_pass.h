#include <llvm/Pass.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Function.h>

#include <list>
#include <vector>
#include <unordered_map>
#include <unordered_set>

//new
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;

#define REOMP_IR_PASS_INSERT_AFTER  (1)
#define REOMP_IR_PASS_INSERT_BEFORE (0)

#define REOMP_CONST_INT64TY(val)     ConstantInt::get(Type::getInt64Ty(*REOMP_CTX), val)
#define REOMP_CONST_INT32TY(val)     ConstantInt::get(Type::getInt32Ty(*REOMP_CTX), val)
#define REOMP_CONST_INT64PTRTY_NULL  ConstantPointerNull::get(Type::getInt64PtrTy(*REOMP_CTX))

typedef enum {
	HOWTO_TYPE_OMP_FUNC,
	HOWTO_TYPE_ENABLE_HOOK,
	HOWTO_TYPE_DISABLE_HOOK,
	HOWTO_TYPE_DYN_ALLOC,
	HOWTO_TYPE_OTHERS
} howto_type;

class reomp_omp_rr_data
{
	/*Using unordered_set to check if extracted global var was already inserted or not */
public:
	unordered_set<GlobalVariable*>* global_var_uset;
	list<Value*>* arg_list;
};


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
class ReOMP : public PassInfoMixin<ReOMP>
{
public:
	PreservedAnalyses run(Function& F, FunctionAnalysisManager&);
	static bool isRequired();
private:
	int stop;
	unordered_map<string, FunctionCallee> reomp_func_umap;
	Module* REOMP_M;
	LLVMContext* REOMP_CTX;
	DataLayout* REOMP_DL;
	reomp_omp_rr_data* create_omp_rr_data();

	void init_inserted_functions(Module& M);
	int is_data_racy_access(Function* F, Instruction* I, uint64_t* access);

	void insert_func(Instruction* I, BasicBlock* BB, int offset, int control, Value* ptr, Value* size);
	void insert_func_reduction(Instruction* I, BasicBlock* BB, int offset, int control, Value* ptr, Value* size);


	/* Function handlers */
	int handle_function(Function& F);
	int handle_function_on_main(Function& F);
	/* BasicBlock handlers */
	int handle_basicblock(Function& F, BasicBlock& BB);
	/* Instruction handlers */
	int handle_instruction(Function& F, BasicBlock& BB, Instruction& I);
	int handle_instruction_on_critical(Function& F, BasicBlock& BB, Instruction& I);
	int handle_instruction_on_reduction(Function& F, BasicBlock& BB, Instruction& I);
	int handle_instruction_on_load_store(Function& F, BasicBlock& BB, Instruction& I);
};

//-----------------------------------------------------------------------------
// Legacy PM implementation
//-----------------------------------------------------------------------------
class LegacyReOMP : public FunctionPass
{
public:
	static char ID;

	LegacyReOMP()
		: FunctionPass(ID) {}
	~LegacyReOMP() {}

	virtual bool doInitialization(Module& M);
	virtual bool runOnFunction(Function& F);
	virtual void getAnalysisUsage(AnalysisUsage& AU) const;

private:
	unordered_map<string, FunctionCallee> reomp_func_umap;
	Module* REOMP_M;
	LLVMContext* REOMP_CTX;
	DataLayout* REOMP_DL;
	reomp_omp_rr_data* create_omp_rr_data();

	void init_inserted_functions(Module& M);
	int is_data_racy_access(Function* F, Instruction* I, uint64_t* access);

	void insert_func(Instruction* I, BasicBlock* BB, int offset, int control, Value* ptr, Value* size);
	void insert_func_reduction(Instruction* I, BasicBlock* BB, int offset, int control, Value* ptr, Value* size);


	/* Function handlers */
	int handle_function(Function& F);
	int handle_function_on_main(Function& F);
	/* BasicBlock handlers */
	int handle_basicblock(Function& F, BasicBlock& BB);
	/* Instruction handlers */
	int handle_instruction(Function& F, BasicBlock& BB, Instruction& I);
	int handle_instruction_on_critical(Function& F, BasicBlock& BB, Instruction& I);
	int handle_instruction_on_reduction(Function& F, BasicBlock& BB, Instruction& I);
	int handle_instruction_on_load_store(Function& F, BasicBlock& BB, Instruction& I);

};


