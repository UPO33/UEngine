#pragma once

#include "Base.h"

#include <QtWidgets/QLineEdit>


namespace UEditor
{
	class UEDITORBASE_API FilterWidget : public QLineEdit
	{
	public:
		explicit FilterWidget(QWidget *parent = 0);

		bool Check(const QString& stringToCheck) const;
		bool Check(const char* stringToCheck) const;

		QStringList mTokens;
	};

};