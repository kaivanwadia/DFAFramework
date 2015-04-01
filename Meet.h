#ifndef CS380C_ASSIGNMENT3_MEET_H
#define CS380C_ASSIGNMENT3_MEET_H

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Instructions.h>
#include "DFAFramework.h"
#include "Hasher.h"
#include "Equal.h"
#include <unordered_set>
#include <unordered_map>

using namespace llvm;

namespace cs380c
{
template<typename T, typename HasherType, typename EqualType>
class Meet
{
private:
	// using TypeSet = std::unordered_set<T, HasherType, EqualType>;
	// using DFAMap = std::unordered_map<const llvm::BasicBlock*, TypeSet>;
public:
	Meet() {}
	// Does the Meet operation on a BasicBlock. Returns an unordered set of the meet result.
	virtual bool doMeet(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<T, HasherType, EqualType>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<T, HasherType, EqualType>>& outMap) = 0;
};

class LivenessMeet: public Meet<llvm::StringRef, StringRefHash, StringRefEqual>
{
private:
public:
	LivenessMeet() : Meet<llvm::StringRef, StringRefHash, StringRefEqual>() {}
	bool doMeet(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& outMap)
	{
		// printf("In doMeet of LivenessMeet\n");
		bool updated = false;
		auto itr = outMap.find(bb);
		if (itr == outMap.end())
		{
			updated = true;
			itr = outMap.insert(std::make_pair(bb, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>())).first;
		}
		auto& bbVariables = itr->second;
		for (auto itr = succ_begin(bb); itr != succ_end(bb); ++itr)
		{
			std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual> otherPHIValues;
			for (auto& inst : (*(*itr)))
			{
				if (llvm::isa<llvm::PHINode>(inst)) // Phi Instruction
				{
					const PHINode* phiInst = &(cast<PHINode>(inst));
					for (int i = 0; i < phiInst->getNumIncomingValues(); i++)
					{
						if (phiInst->getIncomingBlock(i)->getName().str() != (bb)->getName().str())
						{
							otherPHIValues.insert(phiInst->getIncomingValue(i)->getName());
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
				if (otherPHIValues.count(variable) == 0)
				{
					updated |= bbVariables.insert(variable).second;
					// updated |= this->checkAndInsert(bbVariables, variable);
				}
			}
		}
		return updated;
	}
};

class RDefMeet: public Meet<llvm::StringRef, StringRefHash, StringRefEqual>
{
private:
public:
	RDefMeet() : Meet<llvm::StringRef, StringRefHash, StringRefEqual>() {}
	bool doMeet(const llvm::BasicBlock* bb, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& inMap, std::unordered_map<const llvm::BasicBlock*, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>>& outMap)
	{
		// printf("In doMeet of RDefMeet\n");
		bool updated = false;
		auto itr = inMap.find(bb);
		if (itr == inMap.end())
		{
			updated = true;
			itr = inMap.insert(std::make_pair(bb, std::unordered_set<llvm::StringRef, StringRefHash, StringRefEqual>())).first;
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
				updated |= bbRDefs.insert(rDef).second;
			}
		}
		return updated;
	}
};

}

#endif