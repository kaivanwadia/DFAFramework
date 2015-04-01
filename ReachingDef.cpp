#include "ReachingDef.h"
#include "Hasher.h"
#include "Equal.h"

#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
#include "DataFlowAnnotator.h"
#include <unordered_set>

using namespace cs380c;
using namespace llvm;
using namespace std;

bool ReachingDefAnalysis::runOnFunction(Function& f)
{
	std::unordered_set<StringRef, StringRefHash, StringRefEqual> initialSet;
	for (auto itr = f.getArgumentList().begin(); itr != f.getArgumentList().end(); ++itr)
	{
		initialSet.insert((*itr).getName());
	}
	StringRefHash temp = StringRefHash();
	StringRefEqual temp2 = StringRefEqual();
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
