#pragma once

#include "Base.h"
#include <memory.h>

namespace UCore
{
	//maximum alignment that malloc may give us in all platforms
	static const size_t MALLOC_MAX_ALIGN = 8;

	inline void MemCopy(void* dst, const void* src, size_t size) { ::memcpy(dst, src, size); }
	inline void MemMove(void* dst, const void* src, size_t size) { ::memmove(dst, src, size); }
	inline void MemZero(void* dst, size_t size) { ::memset(dst, 0, size); }
	inline int  MemCmp(const void* a, const void* b, size_t size) { return ::memcmp(a, b, size); }
	inline bool MemEqual(const void* a, const void* b, size_t size) { return ::memcmp(a, b, size) == 0; }

	inline void* MemAlloc(size_t size) { return ::malloc(size); }
	inline void* MemAllocZero(size_t size)
	{
		void* p = MemAlloc(size);
		if(p) MemZero(p, size);
		return p;
	}
	inline void* MemRealloc(void* memory, size_t newSize) { return ::realloc(memory, newSize); }
	inline void  MemFree(void* memory) { ::free(memory); }
	template<typename T> void MemFreeSafe(T*& memory)
	{
		if (memory) MemFree(memory);
		memory = nullptr;
	}

	#ifdef UCOMPILER_MSVC
	inline void* MemAllocAligned(size_t size, size_t align) { return ::_aligned_malloc(size, align); }
	inline void* MemAllocAlignedZero(size_t size, size_t align)
	{ 
		void* p = MemAllocAligned(size, align);
		if(p) MemZero(p, size);
		return p;
	}
	inline void* MemReallocAligned(void* memory, size_t newSize, size_t newAlignment) { return ::_aligned_realloc(memory, newSize, newAlignment); }
	inline void  MemFreeAligned(void* memory) { ::_aligned_free(memory); }
	#else
	//#TODO aligned allocation for other compilers
    inline void* MemAllocAligned(size_t size, size_t align) { return ::malloc(size); }
	inline void* MemAllocAlignedZero(size_t size, size_t align)
	{
		void* p = MemAllocAligned(size, align);
		if(p) MemZero(p, size);
		return p;
	}
	inline void* MemReallocAligned(void* memory, size_t newSize, size_t newAlignment) { return ::realloc(memory, newSize); }
	inline void  MemFreeAligned(void* memory) { ::free(memory); }
	#endif

	template<typename T> void  MemFreeAlignedSafe(T*& memory)
	{
		if (memory) MemFreeAligned(memory);
		memory = nullptr;
	}

	//MemNew and MemDelete should be paired
	//allocate memory sizeof(T) and alignof(T), doesn't call constructor
	template < typename T> T* MemNew(const bool zeromemory = false)
	{
		T* ptr = (T*)MemAllocAligned(sizeof(T), alignof(T));
		if (zeromemory) MemZero(ptr, sizeof(T));
		return ptr;
	}
	//MemNew and MemDelete should be paired
	//free allocated memory of T, doesn't call destructor
	template< typename T> void MemDelete(T* ptr)
	{
		MemFreeAligned(ptr);
	}
	template< typename T> void MemDeleteSafe(T*& ptr)
	{
		MemFreeAlignedSafe(ptr);
	}



	template<typename T> void MemZero(T& object)
	{
		::memset(&object, 0, sizeof(T));
	};

#if 0
	void* MemMem(char *haystack, size_t hlen, char *needle, size_t nlen) 
	{
		if (nlen == 0) return haystack; /* degenerate edge case */
		if (hlen < nlen) return 0; /* another degenerate edge case */

		char *hlimit = haystack + hlen - nlen + 1;
		while (haystack = memchr(haystack, needle[0], hlimit - haystack)) {
			if (!memcmp(haystack, needle, nlen)) return haystack;
			haystack++;
		}
		return 0;
	}
#endif // 


	//allocates the requested memory from pool, thew allocated memory will exist till shutdown. threadsafe
	//this function is usually used for startup objects like meta classes and properties
	UCORE_API void* MemPersistentPoolAlloc(size_t size, size_t align);

	struct PersistentPoolAlloc
	{
		static void* operator new (size_t size) { return MemPersistentPoolAlloc(size, sizeof(void*)); }
		static void operator delete(void*) {}
	};
};
