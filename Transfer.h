#ifndef CS380C_ASSIGNMENT3_TRANSFER_H
#define CS380C_ASSIGNMENT3_TRANSFER_H

#include <llvm/IR/BasicBlock.h>
#include "DFAFramework.h"
#include <unordered_set>
#include <unordered_map>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Instructions.h>

using namespace llvm;
namespace cs380c
{

template <typename T>
class Transfer
{
private:

public:
	Transfer() {}
	// Does the Transfer operation on a BasicBlock. Returns a bool of the transfer result.
	virtual bool doTransfer(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::vector<T>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::vector<T>>& outMap) = 0;
	virtual bool checkAndInsert(std::vector<T>& vec, T elem) = 0;
	virtual int findInVector(std::vector<T> vec, T elem) = 0;
};

class LivenessTransfer: public Transfer<llvm::StringRef>
{
private:
public:
	LivenessTransfer() : Transfer<llvm::StringRef>() {}
	bool doTransfer(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& outMap)
	{
		bool updated = false;
		std::vector<llvm::StringRef> generated;
		std::vector<llvm::StringRef> killed;
		// printf("In doTransfer of LivenessTransfer\n");
		for (auto& inst : (*bb))
		{
			if (llvm::isa<llvm::PHINode>(inst)) // Phi Instruction
			{
				killed.push_back(inst.getName());
				for (auto opItr = inst.op_begin(); opItr != inst.op_end(); ++opItr)
				{
					int index = findInVector(killed, (*opItr)->getName());
					if (!(isa<Constant>(*opItr)) && index == -1)
					{
						this->checkAndInsert(generated, (*opItr)->getName());
					}
				}
			}
			else if (!llvm::isa<TerminatorInst>(inst)) // Normal Instruction
			{
				killed.push_back(inst.getName());
				for (auto opItr = inst.op_begin(); opItr != inst.op_end(); ++opItr)
				{
					int index = findInVector(killed, (*opItr)->getName());
					if (!(isa<Constant>(*opItr)) && index == -1)
					{
						this->checkAndInsert(generated, (*opItr)->getName());
					}
				}
			}
			else // Terminator Instruction
			{
				if (inst.getName().str() != "")
				{
					killed.push_back(inst.getName());
				}
				for (auto opItr = inst.op_begin(); opItr != inst.op_end(); ++opItr)
				{
					int index = findInVector(killed, (*opItr)->getName());
					if (!(isa<Constant>(*opItr)) && !(isa<BasicBlock>(*opItr)) && index == -1)
					{
						this->checkAndInsert(generated, (*opItr)->getName());
					}
				}
			}
		}
		std::vector<llvm::StringRef> outVars = outMap[bb];
		std::vector<llvm::StringRef> inVars;
		for (auto outVar : outVars)
		{
			if (this->findInVector(killed, outVar) == -1)
			{
				updated |= this->checkAndInsert(inVars, outVar);
			}
		}
		for (auto genVar : generated)
		{
			updated |= this->checkAndInsert(inVars, genVar);
		}
		inMap[bb] = inVars;
		return updated;
	}

	int findInVector(std::vector<llvm::StringRef> vec, llvm::StringRef elem)
	{
		int index = -1;
		for (auto itr = vec.begin(); itr != vec.end(); ++itr)
		{
			if ((*itr).str().compare(elem.str()) == 0)
			{
				index = std::distance(vec.begin(), itr);
				break;
			}
		}
		return index;
	}

	bool checkAndInsert(std::vector<llvm::StringRef>& vec, llvm::StringRef elem)
	{
		bool present = false;
		for (auto itr = vec.begin(); itr != vec.end(); ++itr)
		{
			if ((*itr).str().compare(elem.str()) == 0)
			{
				present = true;
				break;
			}
		}
		if (present == false)
		{
			vec.push_back(elem);
		}
		return !present;
	}
};

class RDefTransfer: public Transfer<llvm::StringRef>
{
private:
public:
	RDefTransfer() : Transfer<llvm::StringRef>() {}
	bool doTransfer(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& outMap)
	{
		bool updated = false;
		std::vector<llvm::StringRef> generated;
		std::vector<llvm::StringRef> killed;
		// printf("In doTransfer of RDefTransfer\n");
		for (auto& inst : (*bb))
		{
			if (llvm::isa<llvm::PHINode>(inst)) // Phi Instruction
			{
				generated.push_back(inst.getName());
			}
			else if (!llvm::isa<TerminatorInst>(inst)) // Normal Instruction
			{
				killed.push_back(inst.getName());
				generated.push_back(inst.getName());
			}
		}
		std::vector<llvm::StringRef> outVars;
		std::vector<llvm::StringRef> inVars = inMap[bb];
		for (auto inVar : inVars)
		{
			if (this->findInVector(killed, inVar) == -1)
			{
				updated |= this->checkAndInsert(outVars, inVar);
			}
		}
		for (auto genVar : generated)
		{
			updated |= this->checkAndInsert(outVars, genVar);
		}
		outMap[bb] = outVars;
		return updated;
	}

	int findInVector(std::vector<llvm::StringRef> vec, llvm::StringRef elem)
	{
		int index = -1;
		for (auto itr = vec.begin(); itr != vec.end(); ++itr)
		{
			if ((*itr).str().compare(elem.str()) == 0)
			{
				index = std::distance(vec.begin(), itr);
				break;
			}
		}
		return index;
	}

	bool checkAndInsert(std::vector<llvm::StringRef>& vec, llvm::StringRef elem)
	{
		bool present = false;
		for (auto itr = vec.begin(); itr != vec.end(); ++itr)
		{
			if ((*itr).str().compare(elem.str()) == 0)
			{
				present = true;
				break;
			}
		}
		if (present == false)
		{
			vec.push_back(elem);
		}
		return !present;
	}
};

}

#endif