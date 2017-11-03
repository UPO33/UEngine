#pragma once

#include "Base.h"

class QTextEdit;
class QLineEdit;

namespace UEditor
{
	class FilterWidget;

	//////////////////////////////////////////////////////////////////////////
	class UEDITORBASE_API LogWidget : public QWidget
	{
		FilterWidget*	mFiltter;
		QAction*		mActionClear;
		QTextEdit*		mMessages;

	public:

		LogWidget(QWidget* parent);

		void FillHTML();
		void Clean();
		void Tick();
	};

	//////////////////////////////////////////////////////////////////////////
	class UEDITORBASE_API LogDockWidget : public QDockWidget
	{
	public:
		LogDockWidget(QWidget* parent);
		~LogDockWidget();

		void Clean();

		void Tick();
	};

	UEDITORBASE_API QColor UGetQColorOfLog(ELogType type);
};