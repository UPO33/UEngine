#pragma once

#include "Base.h"
#include "../Core/Array.h"
#include "../Core/Object.h"

namespace UCore
{
	class StreamLoaderMemory;
	class StreamSaverMemory;
};

namespace UEngine
{
	UENGINE_API bool USaveArchive(Object* root, ByteSerializer& ser, bool cook, bool compress);
	UENGINE_API Object* ULoadArchive(ByteDeserializer&);
};