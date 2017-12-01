#pragma once

#include "EditorCommonBase.h"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/qapplication.h>

namespace UEditor
{
// 	struct TestAssetLM : public QAbstractListModel
// 	{
// 
// 	};
// 
// 	class TestAssetBrowser :public QListWidget
// 	{
// 	public:
// 		TestAssetBrowser(QWidget* p) : QListWidget(p)
// 		{
// 		}
// 	};
// 	
	
	
	inline void DbgMain(int argc, char** argv)
	{
		QApplication qa(argc, argv);
		QListWidget* mw = new QListWidget;
		QListWidgetItem* newItem = new QListWidgetItem;
		newItem->setText("jkljkl");
		
		mw->addItem(newItem);
		//mw->setItemWidget(newItem, new QPushButton);

		mw->show();
		qa.exec();
	}
};