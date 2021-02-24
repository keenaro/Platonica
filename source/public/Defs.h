#pragma once
#include <memory>
#include <map>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/ext.hpp"

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
using SharedIVec3Map = std::unordered_map<glm::ivec3, SharedPtr<T>>;



#include <cmath>
#include <string>
#include <chrono>
#include <iostream>

class AutoProfiler {
public:
	AutoProfiler(std::string name)
		: m_name(std::move(name)),
		m_beg(std::chrono::high_resolution_clock::now()) { }
	~AutoProfiler() {
		auto end = std::chrono::high_resolution_clock::now();
		auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - m_beg);
		std::cout << m_name << " : " << dur.count() << " musec\n";
	}
private:
	std::string m_name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_beg;
};
