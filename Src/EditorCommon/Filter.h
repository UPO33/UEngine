#pragma once

#include "EditorCommonBase.h"

#include <QtWidgets/qlineedit.h>


namespace UEditor
{
	class UEDITORCOMMON_API FilterWidget : public QLineEdit
	{
	public:
		explicit FilterWidget(QWidget *parent = 0);

		bool Check(const QString& stringToCheck) const;
		bool Check(const char* stringToCheck) const;

		QStringList mTokens;
	};

};