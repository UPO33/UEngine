#pragma once

#include "Base.h"

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtGui/QOpenGLWindow>



namespace UEditor
{
	class LogWidget;
	class LogDockWidget;
	class AssetBrowserDocWidget;

	//////////////////////////////////////////////////////////////////////////
	class MainWinfow : public QMainWindow
	{
	public:
		LogDockWidget*	mLog = nullptr;
		AssetBrowserDocWidget*	mAssetBrowser = nullptr;

		MainWinfow(QWidget* parent = nullptr);

		void Tick();

		virtual QPaintEngine * paintEngine() const override;

	protected:
		virtual void paintEvent(QPaintEvent *event) override;

	};
};