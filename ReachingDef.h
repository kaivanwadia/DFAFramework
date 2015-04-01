#ifndef CS380C_ASSIGNMENT3_REACHINGDEF_H
#define CS380C_ASSIGNMENT3_REACHINGDEF_H

#include "llvm/Pass.h"
#include "DFAFramework.h"
#include "llvm/IR/Instruction.h"
#include "Meet.h"
#include "Transfer.h"
#include "Hasher.h"
#include "Equal.h"
#include <unordered_set>

namespace cs380c
{

class ReachingDefAnalysis: public llvm::FunctionPass
{
private:
public:
	static char ID;
	DFAFramework<llvm::StringRef, StringRefHash, StringRefEqual>* dfa;
	ReachingDefAnalysis() : llvm::FunctionPass(ID) {
		dfa = new DFAFramework<llvm::StringRef, StringRefHash, StringRefEqual>(true, new RDefMeet(), new RDefTransfer());
	}

	bool runOnFunction(llvm::Function&);

	const std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>& getInValues(const llvm::BasicBlock* bb) const {
		return dfa->getInValues(bb);
	}
	const std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>& getOutValues(const llvm::BasicBlock* bb) const {
		return dfa->getOutValues(bb);
	}

	void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
};

}

#endif