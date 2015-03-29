#ifndef CS380C_ASSIGNMENT3_WORKLIST_H
#define CS380C_ASSIGNMENT3_WORKLIST_H

namespace cs380c
{

template <typename ElemType, typename Hasher = std::hash<ElemType>>
class WorkList
{
private:
	std::unordered_map<int, ElemType> list;
	std::unordered_set<ElemType, Hasher> set;
	int maxNumber;
	bool topDown;
public:
	WorkList(int _maxNumber, bool _topDown) : maxNumber(_maxNumber), topDown(_topDown) {};

	bool enqueue(ElemType elem, int priority)
	{
		if (!set.count(elem))
		{
			list[priority] = elem;
			set.insert(elem);
			return true;
		}
		else
			return false;
	}

	ElemType dequeue()
	{
		assert(!list.empty() && "Trying to dequeue an empty queue!");
		ElemType ret;
		int begin = 0;
		int end = maxNumber;
		if (topDown)
		{
			for (int i = maxNumber-1; i >= 0; i--)
			{
				if (list.find(i) != list.end())
				{
					ret = list[i];
					list.erase(i);
					set.erase(ret);
					break;
				}
			}
		}
		else
		{
			for (int i = 0; i < maxNumber; i++)
			{
				if (list.find(i) != list.end())
				{
					ret = list[i];
					list.erase(i);
					set.erase(ret);
					break;
				}
			}
		}
		assert(ret != NULL && "ElemType ret is null");
		return ret;
	}

	bool empty() const { return list.empty(); }
	int size() const { return list.size(); }
};

}
#endif