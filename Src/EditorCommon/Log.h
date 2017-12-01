#pragma once

#include "EditorCommonBase.h"

#include <QtWidgets/qdockwidget.h>

class QTextEdit;
class QLineEdit;

namespace UEditor
{
	class FilterWidget;

	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API LogWidget : public QWidget
	{
		FilterWidget*	mFiltter;
		QAction*		mActionClear;
		QTextEdit*		mMessages;

	public:

		static QColor GetQColorOfLog(ELogType);

		LogWidget(QWidget* parent);

		void FillHTML();
		void Clean();
		void Tick();
	};

	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API LogDockWidget : public QDockWidget
	{
	public:
		LogDockWidget(QWidget* parent);
		~LogDockWidget();

		void Clean();

		void Tick();
	};

};