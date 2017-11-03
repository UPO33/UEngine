#pragma once

#include "../Core/Base.h"


#ifdef UENGINE2D_SHARED
#ifdef UENGINE2D_BUILD
#define UENGINE2D_API UMODULE_EXPORT
#else
#define UENGINE2D_API UMODULE_IMPORT
#endif
#else
#define UENGINE2D_API
#endif

namespace UEngine
{
	using namespace UCore;
};



#include "../Core/Object.h"
#include "../Core/Vector.h"

namespace UEngine
{
	class GameViewport
	{

	};

	class PrimitiveBatch
	{
	public:
		void DrawLine(Vec2 a, Vec2 b, Color32 color);
		void DrawCircle(Vec2 cente, float radius, Color32 color);
		void DrawSolidCircle(Vec2 cente, float radius, Color32 color);
		void DrawAABB(Vec2 min, Vec2 max, Color32 color);
	};

	class UENGINE2D_API Scene2D : public Object
	{
	public:
		GameViewport*	GetViewport();
	};

	class UENGINE2D_API Entity2D : public Object
	{
		
	};
	class UENGINE2D_API Entity2DNode : public Entity2D
	{

	};

	class UENGINE2D_API Entity2DSprite : public Entity2DNode
	{

	};
	class AMesh
	{
		GFXResource*	mVBuffer;
		GFXResource*	mIndexBuffer;
	};
	class AMaterialGLES
	{
		GFXProgram* mProgram;

		
		virtual void Create()
		{
			TArray<ShaderMacro> macros;

			GetGFXContext()->GetShaderMgr()->GetShader("TestVS.glsl", "Main", EShader::Vertex, macros);
			GetGFXContext()->GetShaderMgr()->GetShader("TestPS.glsl", "Main", EShader::Pixel, macros);
		}
		virtual void Bind()
		{

			mGFX->BindProgram(mProgram);
		}
		void SetUniform()
		{

		}
	};

	class AMyMaterial : AMaterialGLES
	{
		Vec2	mUVOffset;
		Vec2	mUVScale;

		void Bind()
		{

		}
	};

}