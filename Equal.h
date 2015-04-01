#ifndef CS380C_ASSIGNMENT3_EQUAL_H
#define CS380C_ASSIGNMENT3_EQUAL_H

using namespace llvm;

namespace cs380c
{
template<typename T>
class Equal
{
public:
	virtual size_t operator() (const T &lhsElem, const T &rhsElem) const = 0;
};

class StringRefEqual : Equal<llvm::StringRef>
{
public:
	size_t operator() (const llvm::StringRef &lhsElem, const llvm::StringRef &rhsElem) const
	{
		return lhsElem.str().compare(rhsElem.str()) == 0;
	}
};
}
#endif