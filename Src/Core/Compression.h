#pragma once

#include "Base.h"

namespace UCore
{
	//return null if failed, the returned value must be freed by MemFree
	UCORE_API void* UCompressBuffer(const void* inBuffer, size_t inBufferSize, size_t& outBufferSize);
	UCORE_API void* UDeCompressBuffer(const void* inBuffer, size_t inBufferSize, size_t& outBufferSize);
};