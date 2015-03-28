#include "Liveness.h"

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include "DataFlowAnnotator.h"
#include <unordered_set>


using namespace cs380c;
using namespace llvm;
using namespace std;

bool LivenessAnalysis::runOnFunction(Function& f)
{
	vector<StringRef> initialSet;
	dfa->setInitialValues(initialSet);
	dfa->doDFA(f);
	DataFlowAnnotator<LivenessAnalysis> annotator(*this, errs());
	annotator.print(f);
	return false;
}

void LivenessAnalysis::getAnalysisUsage(llvm::AnalysisUsage& AU) const
{
	AU.setPreservesAll();
}

char LivenessAnalysis::ID = 0;
static RegisterPass<LivenessAnalysis> X("live", "Liveness Analysis for LLVM top-level vars");
