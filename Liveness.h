#ifndef CS380C_ASSIGNMENT3_LIVENESS_H
#define CS380C_ASSIGNMENT3_LIVENESS_H

#include "llvm/Pass.h"
#include "DFAFramework.h"
#include "llvm/IR/Instruction.h"
#include "Meet.h"
#include "Transfer.h"

namespace cs380c
{

class LivenessAnalysis: public llvm::FunctionPass
{
private:
public:
	static char ID;
	DFAFramework<llvm::StringRef>* dfa;
	LivenessAnalysis() : llvm::FunctionPass(ID) {
		dfa = new DFAFramework<llvm::StringRef>(false, new LivenessMeet(), new LivenessTransfer());
	}

	bool runOnFunction(llvm::Function&);

	const std::vector<llvm::StringRef>& getInValues(const llvm::BasicBlock* bb) const {
		return dfa->getInValues(bb);
	}
	const std::vector<llvm::StringRef>& getOutValues(const llvm::BasicBlock* bb) const {
		return dfa->getOutValues(bb);
	}

	void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
};

}

#endif