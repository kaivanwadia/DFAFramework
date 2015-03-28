#ifndef CS380C_ASSIGNMENT3_MEET_H
#define CS380C_ASSIGNMENT3_MEET_H

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Instructions.h>
#include "DFAFramework.h"
#include <unordered_set>
#include <unordered_map>

using namespace llvm;

namespace cs380c
{
template<typename T>
class Meet
{
private:

public:
	Meet() {}
	// Does the Meet operation on a BasicBlock. Returns an unordered set of the meet result.
	virtual bool doMeet(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::vector<T>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::vector<T>>& outMap) = 0;
	virtual bool checkAndInsert(std::vector<T>& vec, T elem) = 0;
	virtual int findInVector(std::vector<T> vec, T elem) = 0;
};

class LivenessMeet: public Meet<llvm::StringRef>
{
private:
public:
	LivenessMeet() : Meet<llvm::StringRef>() {}
	bool doMeet(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& outMap)
	{
		// printf("In doMeet of LivenessMeet\n");
		bool updated = false;
		auto itr = outMap.find(bb);
		if (itr == outMap.end())
		{
			updated = true;
			itr = outMap.insert(std::make_pair(bb, std::vector<llvm::StringRef>())).first;
		}
		auto& bbVariables = itr->second;
		for (auto itr = succ_begin(bb); itr != succ_end(bb); ++itr)
		{
			std::vector<llvm::StringRef> otherPHIValues;
			for (auto& inst : (*(*itr)))
			{
				if (llvm::isa<llvm::PHINode>(inst)) // Phi Instruction
				{
					const PHINode* phiInst = &(cast<PHINode>(inst));
					for (int i = 0; i < phiInst->getNumIncomingValues(); i++)
					{
						if (phiInst->getIncomingBlock(i)->getName().str() != (bb)->getName().str())
						{
							otherPHIValues.push_back(phiInst->getIncomingValue(i)->getName());
						}
					}
				}
				else
				{
					break;
				}
			}
			auto succVariables = inMap[*itr];
			for (auto const variable : succVariables)
			{
				if (this->findInVector(otherPHIValues, variable) == -1)
				{
					updated |= this->checkAndInsert(bbVariables, variable);
				}
			}
		}
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
			}
		}
		if (present == false)
		{
			vec.push_back(elem);
		}
		return !present;
	}
};

class RDefMeet: public Meet<llvm::StringRef>
{
private:
public:
	RDefMeet() : Meet<llvm::StringRef>() {}
	bool doMeet(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::vector<llvm::StringRef>>& outMap)
	{
		// printf("In doMeet of RDefMeet\n");
		bool updated = false;
		auto itr = inMap.find(bb);
		if (itr == inMap.end())
		{
			updated = true;
			itr = inMap.insert(std::make_pair(bb, std::vector<llvm::StringRef>())).first;
		}
		auto& bbRDefs = itr->second;
		for (auto itr = pred_begin(bb); itr != pred_end(bb); ++itr)
		{
			auto mapItr = outMap.find(*itr);
			if (mapItr == outMap.end())
			{
				continue;
			}
			auto predVariables = outMap[*itr];
			for (auto const rDef : predVariables)
			{
				updated |= this->checkAndInsert(bbRDefs, rDef);
			}
		}
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