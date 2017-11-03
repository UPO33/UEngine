#include <QtWidgets/QApplication>

#include "MainWindow.h"

#include "GLClasses.h"

#include "../Core/Vector.h"

GLFuncs* gGL = nullptr;

using namespace UCore;
using namespace UGFX;

struct RD
{
	Vec3 mA, mB;
};
struct WR
{
	float dist;
};
struct Node
{
	RD mR;
	WR mW;
};

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	QSurfaceFormat::setDefaultFormat(format);


	app.setApplicationName("cube");
	app.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL
	MainWidget widget;
	widget.show();
#else
	QLabel note("OpenGL Support required");
	note.show();
#endif
	return app.exec();
}
