#include "Log.h"

#include "Filter.h"

#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

namespace UEditor
{
	const char* gLogHTMLBegin =
		"<!DOCTYPE html><html><head><style>"
		"h1, h2{ color: white;	margin: 0; }"
		// 		"p{ color: red; }"
		// 		"p.s0{ color: white; }"
		// 		"p.s1{ color: #66ff33; }"
		// 		"p.s2{ color: yellow; }"
		// 		"p.s3{ color: red; }"
		// 		"p.s4{ color: red; }"
		// 		"p.s5{ color: red; }"
		"body{ background-color : black; }"
		"</style></head><body>"
		"<h1>UPOEngine log file</h1>"
		"<h2>[File Name] [Function Name] [LineNumber] [ThreadID] log</h2>"
		"<p></p>";

	const char* gLogHTMLEnd =
		"</body></html>";




	//////////////////////////////////////////////////////////////////////////
	LogDockWidget::LogDockWidget(QWidget* parent) : QDockWidget(parent)
	{
		this->setWidget(new LogWidget(nullptr));

		this->setAllowedAreas(Qt::DockWidgetArea::TopDockWidgetArea | Qt::DockWidgetArea::BottomDockWidgetArea);
		this->setWindowTitle("Log");
		this->setContentsMargins(0, 0, 0, 0);

		//this->setTitleBarWidget(new QTitleBar)
	}
	//////////////////////////////////////////////////////////////////////////
	LogDockWidget::~LogDockWidget()
	{
	}

	void LogDockWidget::Clean()
	{
		((LogWidget*)this->widget())->Clean();
	}

	void LogDockWidget::Tick()
	{
		((LogWidget*)this->widget())->Tick();
	}



	//////////////////////////////////////////////////////////////////////////
	LogWidget::LogWidget(QWidget* parent) : QWidget(parent)
	{
		// 			mListFileName = new QListWidget();
		// 			mListFunction = new QListWidget();
		// 			mListLineNumber = new QListWidget();
		// 			mListThreadID = new QListWidget();
		// 			mListLog = new QListWidget();
		// 
		// 			QSplitter* splt = new QSplitter(this);
		// 			splt->addWidget(mListFileName);
		// 			splt->addWidget(mListFunction);
		// 			splt->addWidget(mListLineNumber);
		// 			splt->addWidget(mListThreadID);
		// 			splt->addWidget(mListLog);
		// 			splt->setOrientation(Qt::Orientation::Horizontal);

		setLayout(new QVBoxLayout);
		//layout()->setContentsMargins(8, 0, 8, 0);
		mFiltter = new FilterWidget(this);
		layout()->addWidget(mFiltter);
		//connect(mFiltter, &QLineEdit::textChanged, this, &LogWidget::FillterTextChanged);
		
		mMessages = new QTextEdit(this);
		layout()->addWidget(mMessages);
		mMessages->setReadOnly(true);


		{
			mMessages->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

			////////////clear action
			mActionClear = new QAction("Clear", this);
			connect(mActionClear, &QAction::triggered, this, [&](bool) {
				Clean();
			});

			//adding clear to context menu
			{
				connect(mMessages, &QTextEdit::customContextMenuRequested, this, [&](const QPoint p)
				{
					QMenu* menu = mMessages->createStandardContextMenu(p);
					menu->addAction(mActionClear);
					menu->exec(mMessages->mapToGlobal(p));
					delete menu;
				});

				mMessages->addAction(mActionClear);
			}
		}


		FillHTML();
	}

	void LogWidget::Tick()
	{
		if (true)
		{
			FillHTML();

			//set scroll to bottom
			mMessages->verticalScrollBar()->setValue(mMessages->verticalScrollBar()->maximum());
		}

	}
	const char* ULogGetLegibleThreadName(const LogEntry* log)
	{
		//#TODO
		return "";
	}
	const char* ULogGetLegibleFileName(const LogEntry* log)
	{
		//#TODO
		return "";
	}

	void LogWidget::FillHTML()
	{
		QString htmlResult;

		htmlResult.reserve(1024 * 10);

		htmlResult += gLogHTMLBegin;

		{
			GLog()->BeginReadLogs();

			for (unsigned i = 0; i < GLog()->GetNumLog(); i++)
			{
				const LogEntry* log = GLog()->GetLog(i);

				const char* strSyle = "color: red;";
				if (log->mType == ELogType::ELT_Message) strSyle = "color: white;";
				else if (log->mType == ELogType::ELT_Success) strSyle = "color: #66ff33;";
				else if (log->mType == ELogType::ELT_Warn) strSyle = "color: yellow";


				char lineBuffer[512];
				//[file] [function] [line] [thread]  log
				sprintf(lineBuffer, "<p  style=\"%s\"> [%s] [%s] [%d] [%s] \t\t    %s</p>",
					strSyle, ULogGetLegibleFileName(log), log->mFunctionName, log->mLineNumber, ULogGetLegibleThreadName(log), log->mMessage);

				if (mFiltter->Check(lineBuffer))
					htmlResult.append(lineBuffer);
			}
			GLog()->EndReadLogs();
		}

		htmlResult += gLogHTMLEnd;

		mMessages->setHtml(htmlResult);
	}

	void LogWidget::Clean()
	{
		FillHTML();
	}

	//////////////////////////////////////////////////////////////////////////
	QColor LogWidget::GetQColorOfLog(ELogType type)
	{
		switch (type)
		{
		case ELogType::ELT_Message: return QColor(255, 255, 255);
		case ELogType::ELT_Success: return QColor(0, 255, 0);
		case ELogType::ELT_Warn: return QColor(255, 255, 0);
		case ELogType::ELT_Error:
		case ELogType::ELT_Fatal:
		case ELogType::ELT_Assert:
			return QColor(255, 0, 0);
		}
		return QColor(255, 255, 255);
	}


};