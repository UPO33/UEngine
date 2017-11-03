#pragma once

#include "../Core/Base.h"
#include "../GFXCore/Base.h"
#include "../Core/FileSys.h"
#include "../Core/SmartMemory.h"

#define USE_QT_GLES


#ifdef USE_QT_GLES
#include <QtGui/QOpenGLFunctions>
typedef QOpenGLFunctions GLFuncs;
#else
#include <GLES2/gl2.h>
struct GLFuncs
{

};
#endif

#pragma once

namespace UGFX
{
	extern GLFuncs* gGL;

	using namespace UGFX;


	inline void UGLCheckError()
	{
#if 0
		GLenum err(glGetError());

		const char* error = nullptr;

		while (err != GL_NO_ERROR) {
			switch (err) {
			case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
			}

			UASSERT(err == GL_NO_ERROR && error == nullptr);
			err = glGetError();
		}
#endif // 
	}


	//////////////////////////////////////////////////////////////////////////
	struct GFXShaderGL : GFXShader
	{
		GLuint mHandle;

		GFXShaderGL(GLuint handle, EShader type) : GFXShader(type), mHandle(handle)
		{
		}
	};
	struct GFXProgramGL : GFXResource
	{
		GLuint mHandle = 0;
		GLuint mVSHandleForwarded = 0;
		GLuint mPSHandleForwarded = 0;

		GFXShaderGL* mVS = nullptr;
		GFXShaderGL* mPS = nullptr;
		GLFuncs* mGLFuncs = nullptr;

		GFXProgramGL(GLFuncs* funcs, GFXShaderGL* vs, GFXShaderGL* ps, GLuint handle) 
			: mGLFuncs(funcs), mVS(vs), mPS(ps), mHandle(handle)
		{
			if (vs)
				mVSHandleForwarded = vs->mHandle;
			if (ps)
				mPSHandleForwarded = ps->mHandle;
		}

		unsigned GetUniformLocation(const char* name)
		{
			return mGLFuncs->glGetUniformLocation(mHandle, name);
		}
	};

	struct ProgramMgrGL
	{
		GLFuncs* mGLFuncs = nullptr;
		TArray<GFXProgramGL*> mPrograms;

		ProgramMgrGL(GLFuncs* funcs) : mGLFuncs(funcs)
		{

		}

		//////////////////////////////////////////////////////////////////////////
		GFXProgramGL* CreateProgram(GFXShaderGL* vs, GFXShaderGL* ps)
		{
			// Create the program object
			GLuint handle = mGLFuncs->glCreateProgram();

			if (handle == 0)
			{
				ULOG_ERROR("failed to create program");
				return nullptr;
			}

			if (vs)
				mGLFuncs->glAttachShader(handle, vs->mHandle);
			if (ps)
				mGLFuncs->glAttachShader(handle, ps->mHandle);

			// Link the program
			mGLFuncs->glLinkProgram(handle);

			int linked = false;
			// Check the link status
			mGLFuncs->glGetProgramiv(handle, GL_LINK_STATUS, &linked);

			if (!linked)
			{
				char logBuffer[2048];
				mGLFuncs->glGetProgramInfoLog(handle, sizeof(logBuffer), nullptr, logBuffer);
				ULOG_ERROR(logBuffer);
				mGLFuncs->glDeleteProgram(handle);
				return nullptr;
			}

			{
				//mGLFuncs->glGetUniformLocation(0, 0);
			}
			GFXProgramGL* ret = new GFXProgramGL(mGLFuncs, vs, ps, handle);
			mPrograms.Add(ret);
			return ret;
		}
		//////////////////////////////////////////////////////////////////////////
	};

	//////////////////////////////////////////////////////////////////////////
	inline GLenum UToGLES2(EShader in)
	{
		switch (in)
		{
		case EShader::Vertex:
			return GL_VERTEX_SHADER;
		case EShader::Pixel:
			return GL_FRAGMENT_SHADER;
		}
		return 0;
	}

	struct GLBuffer
	{
		GLuint mHandle = 0;

		GLBuffer(GLuint handle) : mHandle(handle) {}
	};
	//////////////////////////////////////////////////////////////////////////
	struct ShaderMgrGL : IShaderMgr
	{
		GLFuncs* mGLFuncs;

		ShaderMgrGL(GLFuncs* pFuncs) : mGLFuncs(pFuncs)
		{

		}

		using StringT = std::string;

		//////////////////////////////////////////////////////////////////////////
		bool GetShaderSource(Name filename, EShader shader, const TArray<ShaderMacro>& macros, StringT& shaderFinalSource)
		{
			//#Node entry point is ignored, and only vertex and pixel shader is supported

			shaderFinalSource.reserve(4096);

			if (shader == EShader::Vertex)
				shaderFinalSource += "#define VERTEX_SHADER 1\n";
			if (shader == EShader::Pixel)
				shaderFinalSource += "#define PIXEL_SHADER 1\n";


			for (const ShaderMacro& macro : macros)
			{
				if (!macro.mName.IsEmpty())
				{
					shaderFinalSource += "#define ";
					shaderFinalSource += macro.mName;
					shaderFinalSource += ' ';
					shaderFinalSource += macro.mValue;
					shaderFinalSource += '\n';
				}
			}

			char shaderFilename[1024];
			sprintf(shaderFilename, "%s%s", "../../Shaders/", filename.CStr());

			TSPtr<SmartMemBlock> shaderFileContent = UFileOpenReadFull(shaderFilename);

			if (shaderFileContent)
			{
				shaderFinalSource.append((const char*)shaderFileContent->Memory(), shaderFileContent->Size());

				//#TODO processing #include
				return true;
			}

			return false;
		}


		virtual void RecompileShader(GFXShader* shader) override
		{
		}


		virtual void RecompileShader(Name filename) override
		{
		}


		virtual void ReleaseShder(GFXShader*) override
		{
		}

		//////////////////////////////////////////////////////////////////////////
		GFXShader* CreateShader(const StringT& shaderSource, EShader type)
		{
			GLuint shader = mGLFuncs->glCreateShader(UToGLES2(type));
			UGLCheckError();

			const char* sources[128];

			const char* shaderSrc = shaderSource.c_str();
			// Load the shader source
			mGLFuncs->glShaderSource(shader, 1, &shaderSrc, nullptr);

			// Compile the shader
			mGLFuncs->glCompileShader(shader);

			int compiled = false;

			// Check the compile status
			mGLFuncs->glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

			if (!compiled)
			{
				GLint infoLen = 0;

				char errorBuffer[2048];

				mGLFuncs->glGetShaderInfoLog(shader, sizeof(errorBuffer), nullptr, errorBuffer);
				ULOG_ERROR("Shader Compilation failed %s", errorBuffer);
				mGLFuncs->glDeleteShader(shader);

				return nullptr;
			}

			return new GFXShaderGL(shader, type);
		}
		//////////////////////////////////////////////////////////////////////////
		virtual GFXShader* GetShader(Name filename, Name entrypoint, EShader shader, const TArray<ShaderMacro>& macros) override
		{
			StringT shaderSource;
			if (GetShaderSource(filename, shader, macros, shaderSource))
			{
				return CreateShader(shaderSource, shader);
			}
			return nullptr;
		}


		virtual GFXShader* GetShader(Name filename, Name entrypoint, EShader shader, const ShaderMacros& macros = ShaderMacros()) override
		{
			return nullptr;
		}

	};

	void UGFXFormatToSizeAndType(EGFXFormat format, GLuint& outCount, GLenum& outType)
	{
		struct Dummy
		{
			GLuint mCount;	//e.g 3
			GLenum mType; //e.g GL_FLOAT
		};

		static Dummy LUT[(unsigned)EGFXFormat::Max];

		{
			LUT[(unsigned)EGFXFormat::R32_FLOAT] = { 1, GL_FLOAT };
			LUT[(unsigned)EGFXFormat::R32G32_FLOAT] = { 2, GL_FLOAT };
			LUT[(unsigned)EGFXFormat::R32G32B32_FLOAT] = { 3, GL_FLOAT };
			LUT[(unsigned)EGFXFormat::R32G32B32A32_FLOAT] = { 4, GL_FLOAT };
		}

		outCount = LUT[(unsigned)format].mCount;
		outType = LUT[(unsigned)format].mType;
	}

	struct GFXInputElement
	{
		Name		mName;
		EGFXFormat	mFormat;
	};
	//////////////////////////////////////////////////////////////////////////
	struct GFXContextGL
	{
		static const unsigned MAX_VERTEX_BUFFER = 8;

		GLuint mVertexBuffers[MAX_VERTEX_BUFFER];
		GLuint mIndexBuffer;
		unsigned mNumInput = 0;
		GLFuncs* mGLFuncs = nullptr;
		
		void BindInputs()
		{
			for (unsigned i = 0; i < mNumInput; i++)
			{

				mGLFuncs->glEnableVertexAttribArray(i);
				mGLFuncs->glVertexAttribPointer(i, 1, GL_FLOAT, false, 0, 0);
			}
		}

		GLBuffer CreateBuffer(size_t size, void* initialData, GLenum target /*GL_ARRAY_BUFFER ..*/, GLenum usage /*GL_STATIC_DRAW*/)
		{
			GLuint handle = 0;
			mGLFuncs->glGenBuffers(1, &handle);
			mGLFuncs->glBindBuffer(target, handle);
			mGLFuncs->glBufferData(target, size, initialData, usage);
			mGLFuncs->glBindBuffer(target, 0);

			return GLBuffer(handle);
		}
		GLBuffer CreateVertexBufferStatic(size_t size, void* initialData)
		{
			return CreateBuffer(size, initialData, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		}
		GLBuffer CreateIndexBufferStatic(size_t size, void* initialData)
		{
			return CreateBuffer(size, initialData, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
		}

		void Draw()
		{
			
		}
		void DrawIndexed()
		{
			
		}
		void BindVertexBuffer(unsigned index, GLBuffer vb)
		{
			mVertexBuffers[index] = vb.mHandle;
		}
		void UnbindIndexBuffer()
		{
			mGLFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		void BinIndexBuffer(GLBuffer ib)
		{
			mGLFuncs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.mHandle);
		}
	};
};