#include "Memory.h"
#include "Allocators.h"
#include "Threading.h"

namespace UCore
{


//////////////////////////////////////////////////////////////////////////
UCORE_API void* MemPersistentPoolAlloc(size_t size, size_t align)
{
	static SpinLock Lock;	//the probability of going to lock is very low so I used SpinLock instead 
	static LinearAllocator PersistenPool(1024 * 1024);

	USCOPE_LOCK(Lock);
	return PersistenPool.Alloc(size, align);
}

};