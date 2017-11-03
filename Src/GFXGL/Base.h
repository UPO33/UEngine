#pragma once


#include <GLES2/gl2.h>
#include "../GFXCore/Base.h"

#define UGFXGLES2_API

namespace UGFX
{
	UGFXGLES2_API int TestFunc();

	inline void UGLCheckError() 
	{
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
	}
	struct GLHandle
	{
		GLuint mHandle = 0;
	};

	struct GLBuffer : GLHandle
	{
		GLenum mTarget = 0;

		void Init(size_t size, void* initialData, GLenum target /*GL_ARRAY_BUFFER ..*/, GLenum usage /*GL_STATIC_DRAW*/)
		{
			UGLCheckError();
			glGenBuffers(1, &mHandle);
			glBindBuffer(target, mHandle);
			UGLCheckError();
			glBufferData(target, size, initialData, usage);
			UGLCheckError();
			glBindBuffer(target, 0);
			UGLCheckError();
		}

		void InitVB(size_t size, void* initialData)
		{
			Init(size, initialData, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
		}
		void InitIB(size_t size, void* initialData)
		{
			Init(size, initialData, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
		}
		//bind this buffer as vertex buffer
		void BindVB()
		{
			glBindBuffer(GL_ARRAY_BUFFER, mHandle);
		}
		//bind this buffer as index buffer
		void BindIB()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandle);
		}
		void Release()
		{
			if (mHandle)
				::glDeleteBuffers(1, &mHandle);
			mHandle = 0;
		}
	};

	struct GLShader : GLHandle
	{
		bool Init(const char* shaderSrc, int type)
		{
			GLuint shader = glCreateShader(type);
			// Load the shader source
			glShaderSource(shader, 1, &shaderSrc, nullptr);

			// Compile the shader
			glCompileShader(shader);

			int compiled = false;

			// Check the compile status
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

			if (!compiled)
			{
				GLint infoLen = 0;

				char errorBuffer[2048];

				glGetShaderInfoLog(shader, sizeof(errorBuffer), nullptr, errorBuffer);
				ULOG_ERROR("Shader Compilation failed %s", errorBuffer);
				glDeleteShader(shader);
				return false;
			}

			mHandle = shader;
			return true;
		}
		void Release()
		{
			if (mHandle)
				::glDeleteShader(mHandle);
			mHandle = 0;
		}
	};

	struct GLProgram : GLHandle
	{
		GLShader mVShader;
		GLShader mPShader;

		bool Init(const char* vertexShaderSrc, const char* pixelShaderSrc)
		{
			if (vertexShaderSrc)
				mVShader.Init(vertexShaderSrc, GL_VERTEX_SHADER);
			if (pixelShaderSrc)
				mPShader.Init(pixelShaderSrc, GL_FRAGMENT_SHADER);

			return Init(mVShader.mHandle, mPShader.mHandle);
		}
		bool Init(GLuint vertexShader, GLuint pixelShader)
		{
			// Create the program object
			mHandle = glCreateProgram();

			if (mHandle == 0)
				return false;

			if (vertexShader)
				glAttachShader(mHandle, vertexShader);
			if (pixelShader)
				glAttachShader(mHandle, pixelShader);

			//bindings
			//glBindAttribLocation(mHandle, 0, "vPosition");
			//glBindAttribLocation(mHandle, 1, "vColor");

			// Link the program
			glLinkProgram(mHandle);

			int linked = false;
			// Check the link status
			glGetProgramiv(mHandle, GL_LINK_STATUS, &linked);

			if (!linked)
			{
				char logBuffer[2048];
				glGetProgramInfoLog(mHandle, sizeof(logBuffer), nullptr, logBuffer);
				ULOG_ERROR(logBuffer);
				glDeleteProgram(mHandle);
				mHandle = 0;
				return false;
			}

			return true;
		}
		void Bind()
		{
			glUseProgram(mHandle);
			UGLCheckError();
		}
		void Release()
		{
			mPShader.Release();
			mVShader.Release();

			if (mHandle)
				glDeleteProgram(mHandle);
			mHandle = 0;
		}
	};

	
	class GFXContextGLES2
	{
		void BindVertexBuffer(GLBuffer buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer.mHandle);
			UGLCheckError();
		}
		void BindIndexBuffer(GLBuffer buffer)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.mHandle);
			UGLCheckError();
		}
		void UnbindVertexBuffer()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		void UnbindIndexBuffer()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		void BindProgram(GLProgram program)
		{
			glUseProgram(program.mHandle);
		}
		
	};
};
