#include "EditorBase.h"


#include "../Engine/TaskMgr.h"
#include "../EditorCommon/MainWindow.h"
#include "../EditorCommon/Project.h"
#include "../EditorCommon/AssetMgrEd.h"
#include "../EditorCommon/EditorResources.h"

#include "../EditorCommon/NewAssetBrowser.h"

#include "../Engine/EntityPrimitive.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qsplashscreen.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/QtWidgets>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPaintDevice>

namespace UEditor
{
	QApplication* gQApp;

	void UEngineStep();
	void UEngineShutdown();

	//////////////////////////////////////////////////////////////////////////
	void UEngineRender()
	{
		UASSERT(UIsRenderThread());

		UEnqueue(EET_Game, []() {
			UEngineStep();
		});
	}
	//////////////////////////////////////////////////////////////////////////
	void UEngineShutdown()
	{
		UASSERT(UIsGameThread());

		SafeDelete(gMainWidget);
		SafeDelete(gAssetMgr);
		SafeDelete(gEditorResources);
		SafeDelete(gQApp);


		gGFX->Release();
		SafeDelete(gGFX);
	}
	//////////////////////////////////////////////////////////////////////////
	//returns true if we should continue the game loop
	bool UTick()
	{
		gMainWidget->Tick();
		QApplication::processEvents();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void UEngineStep()
	{
		UASSERT(UIsGameThread());

		
		bool bQuit = false;

		bQuit |= !UTick();
		bQuit |= !gMainWidget->isVisible();

		UEnqueue(EET_Render, []() {
			UEngineRender();
		});

		if (bQuit)
		{
			UEngineShutdown();
			gTaskMgr->ReqestExit();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void UCreateTestScene()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	void UEngineInit()
	{
		UASSERT(UIsGameThread());

		//init gfx
		{
			gGFX = new GFXContextDX12;
			gGFX->Init();
			SetGFXContext(gGFX);
		}

		

		gQApp = new QApplication(__argc, __argv);

		QCoreApplication::setOrganizationName("O_O");
		QCoreApplication::setOrganizationDomain("upo33.blogspot.com");
		QCoreApplication::setApplicationName("UEditor");
		QCoreApplication::setApplicationVersion("0.0");

		QLocale::setDefault(QLocale::c());

		gEditorResources = new EditorResources;

		QApplication::setStyle("Fusion");

#if 0
		/////loading style sheet
		{
			if (auto stylehseet = UFileOpenReadFull("../../Resources/dark.stylesheet"))
			{
				QString strSheet = QString::fromUtf8((const char*)stylehseet->Memory(), stylehseet->Size());
				gQApp->setStyleSheet(strSheet);
			}
		}
#endif // 
		gAssetMgr = new AssetMgrEd;
		
		
		gMainWidget = new MainWidget();
		gMainWidget->showMaximized();
	}

	//the initial task of engine , must not bee loop
	void MainProc()
	{
		UEngineInit();
		UEngineStep();
	};
};



int main(int argc, char** argv)
{
	using namespace UEditor;

#if 0 //debug widget?
	DbgMain(argc, argv);
#else

#if 1
	gProject = new ProjectInfo;
	gProject->mName = "TestProject";
	gProject->mAbsoluteDir = QDir("../../TestProject").absolutePath();
	gProject->mAbsoluteAssetsDir = QDir("../../TestProject/Assets/").absolutePath();
	gProject->mAbsoluteShadersDir = QDir("../../TestProject/Shaders/").absolutePath();
#endif

	gTaskMgr = new TaskMgr();
	gTaskMgr->Run(&UEditor::MainProc);
	delete gTaskMgr;
	return 0;
#endif
};
