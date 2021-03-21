#pragma once
#include "stb_image.h"
#include <memory>
#include <map>
#include <unordered_map>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
template<class T>
using WeakPtr = std::weak_ptr<T>;
template<class T>
using UniquePtr = std::unique_ptr<T>;

using String = std::string;

#define MakeShared std::make_shared
#define MakeUnique std::make_unique



template<class T>
using SharedIVec3Map = std::unordered_map<glm::ivec3, SharedPtr<T>>;

