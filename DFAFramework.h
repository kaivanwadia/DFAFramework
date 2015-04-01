#ifndef CS380C_ASSIGNMENT3_DFAFRAMEWORK_H
#define CS380C_ASSIGNMENT3_DFAFRAMEWORK_H

#include <unordered_set>
#include <unordered_map>
#include "Meet.h"
#include "Transfer.h"
#include "Worklist.h"
#include "Hasher.h"
#include "Equal.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/CFG.h>
#include <queue>
#include <stack>

namespace cs380c
{

// T is the type on which the analysis is being done. For example it can be done on Instructions, 
// Expressions, Definitions etc
// , typename HasherType, typename EqualType
template <typename T, typename HasherType, typename EqualType>
class DFAFramework {
private:
	bool topDown;
	using TypeSet = std::unordered_set<T, HasherType, EqualType>;
	using DFAMap = std::unordered_map<const llvm::BasicBlock*, TypeSet>;
	using OrderMap = std::unordered_map<llvm::BasicBlock*, int>;
	DFAMap inMap, outMap;
	Meet<T, HasherType, EqualType>* meet;
	Transfer<T, HasherType, EqualType>* transfer;
	TypeSet initialSet;
	OrderMap postOrderMap;

public:
	DFAFramework() {}
	DFAFramework(bool _topDown, Meet<T, HasherType, EqualType>* _meet, Transfer<T, HasherType, EqualType>* _transfer) {
		topDown = _topDown;
		inMap = DFAMap();
		outMap = DFAMap();
		meet = _meet;
		transfer = _transfer;
		initialSet = TypeSet();
		postOrderMap = std::unordered_map<llvm::BasicBlock*, int>();
	}

	void computePostOrder(llvm::Function& f)
	{
		OrderMap statusMap;
		for (auto& bb : f)
		{
			statusMap[&bb] = 0;
		}
		int orderNo = 0;
		std::stack<llvm::BasicBlock*> bbStack;
		bbStack.push(&f.getEntryBlock());
		statusMap[&f.getEntryBlock()] = 1;
		while(!bbStack.empty())
		{
			llvm::BasicBlock* currBB = bbStack.top();
			bool pushed = false;
			for (auto itr = succ_begin(currBB); itr != succ_end(currBB); ++itr)
			{
				if (statusMap[*itr] == 0)
				{
					pushed = true;
					statusMap[*itr] = 1;
					bbStack.push(*itr);
				}
			}
			if (!pushed)
			{
				bbStack.pop();
				statusMap[currBB] = 2;
				postOrderMap[currBB] = orderNo;
				orderNo++;
			}
		}
	}

	void doDFA(llvm::Function& f)
	{
		// printf("In doDFA\n");
		computePostOrder(f);
		WorkList<llvm::BasicBlock*> workList = WorkList<llvm::BasicBlock*>(postOrderMap.size(), topDown);
		llvm::BasicBlock* bb;
		// Setting up initial basic block
		if (topDown)
		{
			bb = &(f.front());
			this->inMap.insert(std::make_pair(bb, this->initialSet));
		}
		else
		{
			bb = &(f.back());
			this->outMap.insert(std::make_pair(bb, this->initialSet));
		}
		workList.enqueue(bb, postOrderMap[bb]);
		bool first = true;
		// Start iterating over the worklist till empty
		while (!workList.empty())
		{
			auto currBB = workList.dequeue();
			bool meetChangedValue = this->meet->doMeet(currBB, this->inMap, this->outMap);
			if (!meetChangedValue && !first)
			{
				continue;
			}
			first = false;
			bool transferChangedValue = this->transfer->doTransfer(currBB, this->inMap, this->outMap);
			this->addToWorklist(currBB, workList);
		}
	}

	template <typename WorkListType>
	void addToWorklist(llvm::BasicBlock* currBB, WorkListType& workList)
	{
		if (this->topDown)
		{
			for (auto itr = succ_begin(currBB); itr != succ_end(currBB); ++itr)
			{
				bool pushed = workList.enqueue(*itr, postOrderMap[*itr]);
			}
		}
		else
		{
			for (auto itr = pred_begin(currBB); itr != pred_end(currBB); ++itr)
			{
				workList.enqueue(*itr, postOrderMap[*itr]);
			}
		}
		return;
	}

	void setInitialValues(TypeSet _initialSet)
	{
		initialSet = _initialSet;
	}

	const TypeSet& getInValues(const llvm::BasicBlock* bb) const {
		return inMap.at(bb);
	}
	const TypeSet& getOutValues(const llvm::BasicBlock* bb) const {
		return outMap.at(bb);
	}
};
}

#endif