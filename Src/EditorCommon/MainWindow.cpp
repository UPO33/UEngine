#include "MainWindow.h"

#include "Log.h"
#include "Filter.h"
#include "AssetBrowser.h"
#include "LevelEditor.h"
#include "AssetInterface.h"
#include "EntityBrowser.h"
#include "PropertyBrowser.h"

#include "../Engine/Scene.h"
#include "../Engine/EntityPrimitive.h"
#include "../Engine/EntityShape.h"

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtabwidget.h>
#include <QtWidgets/qtoolbutton.h>

namespace UEditor
{
	UEDITORCOMMON_API MainWidget* gMainWidget;


	MainEditorWindow::MainEditorWindow(QWidget* parent) : QMainWindow(parent)
	{
		setMinimumSize(400, 400);

		//central widget, level editor viewport
		{
			mLevelEditor = new LevelEditorViewportWidget(this);
			setCentralWidget(mLevelEditor);
		}


		this->setCorner(Qt::Corner::BottomLeftCorner, Qt::LeftDockWidgetArea);
		this->setCorner(Qt::Corner::BottomRightCorner, Qt::RightDockWidgetArea);

		//asset browser
		{
			mAssetBrowser = new AssetBrowserDockWidget(this);
			this->addDockWidget(Qt::RightDockWidgetArea, mAssetBrowser);
		}
		//entity browser
		{
			mEntityBrowser = new EntityBrowserDockWidget(this);
			this->addDockWidget(Qt::LeftDockWidgetArea, mEntityBrowser);
		}
		//log
		{
			mLog = new LogDockWidget(this);
			this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, mLog);
		}
		//property browser
		{
			mPropertyBrowser = new PropertyBrowserDock(this);
			this->tabifyDockWidget(mAssetBrowser, mPropertyBrowser);
		}
#if 0

		QDockWidget* dock = new QDockWidget(this);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

		auto paragraphsList = new QListWidget();
		paragraphsList->addItems(QStringList()
			<< "Thank you for your payment which we have received today."
			<< "Your order has been dispatched and should be with you "
			"within 28 days."
			<< "We have dispatched those items that were in stock. The "
			"rest of your order will be dispatched once all the "
			"remaining items have arrived at our warehouse. No "
			"additional shipping charges will be made."
			<< "You made a small overpayment (less than $5) which we "
			"will keep on account for you, or return at your request."
			<< "You made a small underpayment (less than $1), but we have "
			"sent your order anyway. We'll add this underpayment to "
			"your next bill."
			<< "Unfortunately you did not send enough money. Please remit "
			"an additional $. Your order will be dispatched as soon as "
			"the complete amount has been received."
			<< "You made an overpayment (more than $5). Do you wish to "
			"buy more items, or should we return the excess to you?");
		dock->setWidget(paragraphsList);

		this->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dock);
#endif // 

		InitMenuBar();
		InitToolBar();

		CreateTestScene();
	}

	void MainEditorWindow::Tick()
	{
		mEntityBrowser->Tick();
		mAssetBrowser->Tick();
		mLevelEditor->Tick();
		mLog->Tick();
	}

	QPaintEngine * MainEditorWindow::paintEngine() const
	{
		QPaintEngine* pe = QMainWindow::paintEngine();
		return pe;
	}

	void MainEditorWindow::paintEvent(QPaintEvent *event)
	{
		QPainter& p = QPainter(this);
		
		QMainWindow::paintEvent(event);
	}

	void MainEditorWindow::InitMenuBar()
	{
		this->menuBar()->addAction("File");
		this->menuBar()->addAction("View");
	}

	void MainEditorWindow::InitToolBar()
	{
		QToolBar* tb = new QToolBar(this);
		
		tb->addAction("Play");
		tb->addAction("Exit");

		this->addToolBar(Qt::ToolBarArea::TopToolBarArea, tb);

		
	}
	//////////////////////////////////////////////////////////////////////////
	void MainEditorWindow::CreateTestScene()
	{
		mEditorScene  = new Scene(SceneCreationParam());

		const ClassInfo* entClasses[] = { EntityBox::GetClassStatic(), EntitySphere::GetClassStatic(), EntityNode::GetClassStatic() };
		const unsigned NumClass = 3;

		Entity* root = NewObject<Entity>();
		root->SetName("root");

		for (unsigned i = 0; i < 16; i++)
		{
			auto childA = NewObject<Entity>(entClasses[RandInt() % 3]);
			childA->SetName(UQString2Name(QString::asprintf("entity %d", i)));
			childA->ObjectAattachTo(root);

			for (unsigned j = 0; j < 4; j++)
			{
				auto childB = NewObject<Entity>(entClasses[RandInt() % NumClass]);
				childB->SetName(UQString2Name(QString::asprintf("entity %d", j)));
				childB->ObjectAattachTo(childA);
			}
		}

		root->RegisterEntityTo(mEditorScene);

		mEntityBrowser->GetWidget()->SetSene(mEditorScene);
	}

	MainWidget::MainWidget(QWidget* parent /*= nullptr*/) : QTabWidget(parent)
	{
		mMainTab = new MainEditorWindow(this);
		
		this->setMovable(true);
		this->setTabsClosable(true);
		this->setIconSize(QSize(32, 32));

		this->addTab(mMainTab, "Level Editor");
		this->tabBar()->setTabButton(0, QTabBar::RightSide,  nullptr);

		connect(this, &QTabWidget::tabCloseRequested, this, [this](int index) {
			QWidget* page = this->widget(index);
			this->removeTab(index);
			delete page;
		});
	}

	void MainWidget::Tick()
	{
		mMainTab->Tick();
	}

	AssetBrowserWidget* MainWidget::GetAssetBrowser() const
	{
		return mMainTab->mAssetBrowser ? mMainTab->mAssetBrowser->GetWidget() : nullptr;
	}

	void MainWidget::OpenAssetWindow(Asset* pAsset)
	{
		if (!pAsset) return;

		int tabIndex = GetAssetWindowIndex(pAsset);
		if (tabIndex == -1)
		{
			AssetInterface* pAssetInterface = AssetInterface::GetInterface(pAsset->GetClass());
			UASSERT(pAssetInterface);

			if (AssetWindowBase* pAssetWindow = pAssetInterface->OnCreateAssetWindow())
			{
				tabIndex = this->addTab(pAssetWindow, UGetClassIcon(pAsset->GetClass()), QString(pAsset->GetName()));
				pAssetWindow->AttachAsset(pAsset);
			}
		}

		if(tabIndex != -1)
			this->setCurrentIndex(tabIndex);
	}

	bool MainWidget::IsAssetWindowOpen(const Asset* pAsset)
	{
		return GetAssetWindowIndex(pAsset) != -1;
	}

	bool MainWidget::IsAssetWindowActive(const Asset* pAsset)
	{
		return this->currentIndex() == GetAssetWindowIndex(pAsset);
	}

	int MainWidget::GetAssetWindowIndex(const Asset* pAsset)
	{
		unsigned tabCount = this->count();
		//#note the tab at zero index is level editor and always open
		for (int iTab = 1; iTab < tabCount; iTab++)
		{
			auto assetWindow = dynamic_cast<AssetWindowBase*>(this->widget(iTab));
			UASSERT(assetWindow);
			if (assetWindow->GetAsset() == pAsset)
				return iTab;
		}
		return -1;
	}

	void MainWidget::ShowAssetInAssetBrowser(const Asset* pAsset)
	{
		if (pAsset)
		{
			this->setCurrentIndex(0);
			if (auto assetBrowser = GetAssetBrowser())
			{
				assetBrowser->FocusOnAsset(pAsset);
			}
		}
	}

	

	void MainWidget::keyPressEvent(QKeyEvent *event)
	{
		if (event->key() == Qt::Key_Terminal)
		{

		}
		ULOG_MESSAGE("Key %", event->key());

		SuperT::keyPressEvent(event);
	}

	void MainWidget::keyReleaseEvent(QKeyEvent *event)
	{
		SuperT::keyReleaseEvent(event);
	}

};