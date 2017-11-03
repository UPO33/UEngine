#pragma once

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopengl.h>
#include <QtGui/QMatrix4x4>
#include <QtGui/QQuaternion>
#include <QtGui/QVector2D>
#include <QtCore/QBasicTimer>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLFunctions>

#include "../Core/Array.h"
#include "../Core/Vector.h"

#include "GLClasses.h"

using namespace UCore;
using namespace UGFX;

class PrimitiveBatch2D
{
public:
	struct LineItem
	{
		Vec2 a;
		Vec2 b;
		Color32 color;
	};
	struct Quad
	{
		Vec2 mPos;
	};

	TArray<LineItem>	mLines;
	
	void DrawLine(Vec2 a, Vec2 b, Color32 color)
	{
		mLines.Add(LineItem{ a, b, color });
	}
	void DrawCircle(Vec2 center, Color32 color)
	{
		
	}
	void DrawPoint(Vec2 center, Vec2 size, Color32 color)
	{

	}
};

//////////////////////////////////////////////////////////////////////////
class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
	ShaderMgrGL* mShaderMgr = nullptr;
	ProgramMgrGL* mProgramMgr = nullptr;
	GFXProgramGL* mProgramTest = nullptr;

	explicit MainWidget(QWidget *parent = 0) : QOpenGLWidget(parent)
	{
		mShaderMgr = new ShaderMgrGL(this);
		mProgramMgr = new ProgramMgrGL(this);
	}

	~MainWidget()
	{
		// Make sure the context is current when deleting the texture
		// and the buffers.
		makeCurrent();
		//delete texture;
		doneCurrent();

		
	}

protected:
	void mousePressEvent(QMouseEvent *e) override
	{

	}
	void mouseReleaseEvent(QMouseEvent *e) override
	{

	}
	void timerEvent(QTimerEvent *e) override
	{
		this->update();
	}

	void initializeGL() override
	{
		initializeOpenGLFunctions();

		

		initShaders();
		initTextures();

		// Enable depth buffer
		glEnable(GL_DEPTH_TEST);

		// Enable back face culling
		glDisable(GL_CULL_FACE);

		// Use QBasicTimer because its faster than QTimer
		timer.start(12, this);
	}
	void resizeGL(int w, int h) override
	{
		// Calculate aspect ratio
		qreal aspect = qreal(w) / qreal(h ? h : 1);

		// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
		const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

		// Reset projection
		projection.setToIdentity();

		// Set perspective projection
		projection.perspective(fov, aspect, zNear, zFar);
	}
	void paintGL() override
	{
		// Clear color and depth buffer
		float f = rand() / ((float)RAND_MAX);
		glClearColor(f, f, f, f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{
			float tris[] = { 0,0,  0.5, 0,   0, 0.5 };
			
			texture->bind();
			
			glUseProgram(mProgramTest->mHandle);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float[2]), tris);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		}
		return;

		

		//! [6]
		// Calculate model view transformation
		QMatrix4x4 matrix;
		matrix.translate(0.0, 0.0, -5.0);
		matrix.rotate(rotation);

		// Set modelview-projection matrix
		program.setUniformValue("mvp_matrix", projection * matrix);
		//! [6]

		// Use texture unit 0 which contains cube.png
		program.setUniformValue("texture", 0);
	}
	//////////////////////////////////////////////////////////////////////////
	void initShaders()
	{
		GFXShader* vs = mShaderMgr->GetShader("Test.glsl", nullptr, EShader::Vertex, TArray<ShaderMacro>());
		GFXShader* ps = mShaderMgr->GetShader("Test.glsl", nullptr, EShader::Pixel, TArray<ShaderMacro>());

		mProgramTest = mProgramMgr->CreateProgram((GFXShaderGL*)vs, (GFXShaderGL*)ps);
		UASSERT(mProgramTest);

		glUseProgram(mProgramTest->mHandle);
		glUniform4f(mProgramTest->GetUniformLocation("uColor"), 1, 0, 0, 1);
		glUseProgram(0);
	}
	//////////////////////////////////////////////////////////////////////////
	void initTextures()
	{
		// Load cube.png image
		texture = new QOpenGLTexture(QImage("../../a.jpg").mirrored());

		// Set nearest filtering mode for texture minification
		texture->setMinificationFilter(QOpenGLTexture::Nearest);

		// Set bilinear filtering mode for texture magnification
		texture->setMagnificationFilter(QOpenGLTexture::Linear);

		// Wrap texture coordinates by repeating
		// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
		texture->setWrapMode(QOpenGLTexture::Repeat);
	}

private:
	QBasicTimer timer;
	QOpenGLShaderProgram program;

	QOpenGLTexture *texture;

	QMatrix4x4 projection;

	QVector2D mousePressPosition;
	QVector3D rotationAxis;
	qreal angularSpeed;
	QQuaternion rotation;
};
