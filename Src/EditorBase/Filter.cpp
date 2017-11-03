#include "Filter.h"




namespace UEditor
{

	FilterWidget::FilterWidget(QWidget *parent /*= 0*/) : QLineEdit(parent)
	{
		this->setClearButtonEnabled(true);
		this->setMaximumWidth(128);
		this->setPlaceholderText("Search");

		connect(this, &QLineEdit::textChanged, this, [this](const QString& text) {
			this->mTokens = text.split(' ', QString::SkipEmptyParts);
		});
	}

	bool FilterWidget::Check(const QString& stringToCheck) const
	{
		if (mTokens.isEmpty()) return true;
		if (stringToCheck.isNull() || stringToCheck.isEmpty()) return true;

		for (auto str : mTokens)
		{
			if (stringToCheck.contains(str, Qt::CaseInsensitive))
				return true;
		}
		return false;
	}

	bool FilterWidget::Check(const char* stringToCheck) const
	{
		return Check(QString(stringToCheck));
	}

};