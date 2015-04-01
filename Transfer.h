#ifndef CS380C_ASSIGNMENT3_TRANSFER_H
#define CS380C_ASSIGNMENT3_TRANSFER_H

#include <llvm/IR/BasicBlock.h>
#include "DFAFramework.h"
#include <unordered_set>
#include <unordered_map>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Instructions.h>
#include "Hasher.h"
#include "Equal.h"

using namespace llvm;
namespace cs380c
{

template <typename T, typename HasherType, typename EqualType>
class Transfer
{
private:

public:
	Transfer() {}
	// Does the Transfer operation on a BasicBlock. Returns a bool of the transfer result.
	virtual bool doTransfer(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<T, HasherType, EqualType>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<T, HasherType, EqualType>>& outMap) = 0;
};

class LivenessTransfer: public Transfer<llvm::StringRef, StringRefHash, StringRefEqual>
{
private:
public:
	LivenessTransfer() : Transfer<llvm::StringRef, StringRefHash, StringRefEqual>() {}
	bool doTransfer(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& outMap)
	{
		bool updated = false;
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> generated;
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> killed;
		// printf("In doTransfer of LivenessTransfer\n");
		for (auto& inst : (*bb))
		{
			if (llvm::isa<llvm::PHINode>(inst)) // Phi Instruction
			{
				killed.insert(inst.getName());
				for (auto opItr = inst.op_begin(); opItr != inst.op_end(); ++opItr)
				{
					int count = killed.count((*opItr)->getName());
					if (!(isa<Constant>(*opItr)) && count == 0)
					{
						generated.insert((*opItr)->getName());
						// this->checkAndInsert(generated, (*opItr)->getName());
					}
				}
			}
			else if (!llvm::isa<TerminatorInst>(inst)) // Normal Instruction
			{
				killed.insert(inst.getName());
				for (auto opItr = inst.op_begin(); opItr != inst.op_end(); ++opItr)
				{
					int count = killed.count((*opItr)->getName());
					if (!(isa<Constant>(*opItr)) && count == 0)
					{
						generated.insert((*opItr)->getName());
						// this->checkAndInsert(generated, (*opItr)->getName());
					}
				}
			}
			else // Terminator Instruction
			{
				if (inst.getName().str() != "")
				{
					killed.insert(inst.getName());
				}
				for (auto opItr = inst.op_begin(); opItr != inst.op_end(); ++opItr)
				{
					// int index = findInVector(killed, (*opItr)->getName());
					int count = killed.count((*opItr)->getName());
					if (!(isa<Constant>(*opItr)) && !(isa<BasicBlock>(*opItr)) && count == 0)
					{
						generated.insert((*opItr)->getName());
						// this->checkAndInsert(generated, (*opItr)->getName());
					}
				}
			}
		}
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> outVars = outMap[bb];
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> inVars;
		for (auto outVar : outVars)
		{
			if (killed.count(outVar) == 0)
			{
				updated |= inVars.insert(outVar).second;
				// updated |= this->checkAndInsert(inVars, outVar);
			}
		}
		for (auto genVar : generated)
		{
			updated |= inVars.insert(genVar).second;
			// updated |= this->checkAndInsert(inVars, genVar);
		}
		inMap[bb] = inVars;
		return updated;
	}
};

class RDefTransfer: public Transfer<llvm::StringRef, StringRefHash, StringRefEqual>
{
private:
public:
	RDefTransfer() : Transfer<llvm::StringRef, StringRefHash, StringRefEqual>() {}
	bool doTransfer(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& outMap)
	{
		bool updated = false;
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> generated;
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> killed;
		// printf("In doTransfer of RDefTransfer\n");
		for (auto& inst : (*bb))
		{
			if (llvm::isa<llvm::PHINode>(inst)) // Phi Instruction
			{
				generated.insert(inst.getName());
			}
			else if (!llvm::isa<TerminatorInst>(inst)) // Normal Instruction
			{
				killed.insert(inst.getName());
				generated.insert(inst.getName());
			}
		}
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> outVars;
		std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> inVars = inMap[bb];
		for (auto inVar : inVars)
		{
			if (killed.count(inVar) == 0)
			{
				updated |= outVars.insert(inVar).second;
			}
		}
		for (auto genVar : generated)
		{
			updated |= outVars.insert(genVar).second;
		}
		outMap[bb] = outVars;
		return updated;
	}
};
}

#endif