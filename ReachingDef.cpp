#include "ReachingDef.h"

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include "DataFlowAnnotator.h"
#include <unordered_set>

using namespace cs380c;
using namespace llvm;
using namespace std;

bool ReachingDefAnalysis::runOnFunction(Function& f)
{
	vector<StringRef> initialSet;
	for (auto itr = f.getArgumentList().begin(); itr != f.getArgumentList().end(); ++itr)
	{
		initialSet.push_back((*itr).getName());
	}
	dfa->setInitialValues(initialSet);
	dfa->doDFA(f);
	DataFlowAnnotator<ReachingDefAnalysis> annotator(*this, errs());
	annotator.print(f);
	return false;
}

void ReachingDefAnalysis::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.setPreservesAll();
}

char ReachingDefAnalysis::ID = 0;
RegisterPass<ReachingDefAnalysis> X("reach", "Reaching Definition Analysis for LLVM top-level vars");
