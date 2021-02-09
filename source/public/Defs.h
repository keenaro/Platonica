#pragma once
#include <memory>
#include <map>

#if _DEBUG
#define DPrint(x) (puts(x))
#define DPrintf(x, ...) (printf(x,  __VA_ARGS__))
#else
#define DPrint(X)
#define DPrintf(x, ...)
#endif

template<class T>
using SharedPtr = std::shared_ptr<T>;

#define MakeShared std::make_shared

template<class T>
using SharedMap3 = std::map<int, std::map<int, std::map<int, SharedPtr<T>>>>;

#define IterateMap3(M3) for (auto const& M3X : M3) for (auto const& M3Y : M3X.second) for (auto mapIt : M3Y.second)
