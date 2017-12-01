#pragma once

#include "AssetInterface.h"

#include "../Engine/Texture.h"

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class TextureImporter : public AssetInterface
	{
		UCLASS(TextureImporter, AssetInterface)


		const QStringList& GetSupportedExts() override
		{
			static QStringList Exts({"dds", "tga", "png", "jpg", "jpeg", "bmp"});
			return Exts;

		}

	};

};