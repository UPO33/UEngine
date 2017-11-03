#pragma once

#include "Base.h"
#include "Name.h"
#include "Memory.h"
#include "Buffer.h"
#include "Array.h"

namespace UCore
{
 	static const unsigned UMAX_PATH = FILENAME_MAX;

	
#if 0
//////////////////////////////////////////////////////////////////////////
	enum class EFileOpenMode
	{
		//Open file for input operations. The file must exist.
		Read,
		//Create an empty file for output operations. If a file with the same name already exists, 
		//its contents are discarded and the file is treated as a new empty file. 
		Write,
		//Open file for output at the end of a file. Output operations always write data at the end of the file, expanding it.
		//Repositioning operations are ignored. The file is created if it does not exist.
		Append
	};

	//////////////////////////////////////////////////////////////////////////
	class UCORE File
	{
	public:
		File() : mHandle(nullptr), mOpenMode(EFileOpenMode::Read) {}
		File(const wchar_t* filename, EFileOpenMode openMode)
		{
			mHandle = nullptr;
			mOpenMode = openMode;
			mfileName = filename;
			Open(filename, openMode);
		}
		~File()
		{
			Close();
		}
		bool Flush();

		//The total number of bytes successfully written is returned.
		size_t WriteBytes(const void* bytes, size_t size);
		//The total number of bytes successfully read is returned.
		size_t ReadBytes(void* outBytes, size_t size);
		//return size of the file in bytes, -1 if error
		int GetSize() const;

		bool IsOpen() const { return mHandle != nullptr; }
		EFileOpenMode OpenMode() const { return mOpenMode; }
		const String& GetName() const { return mfileName; }

		bool Open(const char* filename, EFileOpenMode mode);
		bool Close();

		static bool Rename(const char* oldname, const char* newname);
		static bool Delete(const char* filename);
		static bool Exist(const char* filename);
		static bool OpenReadFull(const char* filename, Buffer& out);

	private:
		void* mHandle;
		EFileOpenMode mOpenMode;
		String mfileName;
	};


	UCORE void PathGetFiles(const String& path, TArray<String>& outFileNames, bool includingSubFiles = false);
	UCORE void PathGetFolders(const String& path, TArray<String>& outFolderNames, bool includingSubFolders = false);
	UCORE String PathGetExt(const String& path);
	UCORE String PathGetFileName(const String& path, bool includingExt = false);
#endif
	
	class SmartMemBlock;

	UCORE_API String PathGetExt(const String& path);
	UCORE_API String PathGetFileName(const String& path, bool includingExt = false);
	
	UCORE_API bool UFileExist(const char* filename);
	UCORE_API SmartMemBlock* UFileOpenReadFull(const char* filename);
	UCORE_API bool UFileCreateWriteFull(const char* filename, void* pData, size_t dataSize);
}