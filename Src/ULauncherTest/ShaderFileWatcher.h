#include "Base.h"

#include <FileWatcher/FileWatcher.h>

//////////////////////////////////////////////////////////////////////////
struct ShaderFilesWatcher
{
	//////////////////////////////////////////////////////////////////////////
	struct Listener : FW::FileWatchListener
	{
		ShaderFilesWatcher* mOwner;

		Listener(ShaderFilesWatcher* owner) : mOwner(owner) {}

		virtual void handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename, FW::Action action) override
		{
			if (action == FW::Action::Modified || action == FW::Action::Add)
			{
				std::string filenameLC = filename;
				std::transform(filenameLC.begin(), filenameLC.end(), filenameLC.begin(), ::tolower);
				if (UStrHasSuffix(filenameLC, ".hlsl"))
				{
					mOwner->mModifiedShadersFile.AddUnique(filenameLC.c_str());
				}
			}
		}
	};

	Listener			mListenear;
	FW::FileWatcher		mWatcher;
	TArray<Name>		mModifiedShadersFile;

	FW::WatchID			mEngineShadersFolderID;

	ShaderFilesWatcher()
		: mListenear(this)
	{
		mEngineShadersFolderID = mWatcher.addWatch("../../Shaders/", &mListenear, true);
	}
	void Update()
	{
		mWatcher.update();
	}
	void RecompileModifides()
	{
		for (Name shaderName : mModifiedShadersFile)
			GetGFXContext()->GetShaderMgr()->RecompileShader(shaderName);

		mModifiedShadersFile.RemoveAll();
	}
};
