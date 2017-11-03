#pragma once

#include "Base.h"

#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qsplitter.h>

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qscrollarea.h>

#include "FlowLayout.h"

#include "../EngineBase/Asset.h"

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class UEDITORBASE_API AssetBrowserWidget : public QWidget
	{
	public:


		AssetBrowserWidget(QWidget* parent = nullptr) : QWidget(parent)
		{
			

			if(0)
			{
				auto verticalLayout = new QVBoxLayout(this);
				verticalLayout->addWidget(new QLineEdit(this));

				verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
				auto splitter = new QSplitter(this);
				splitter->setObjectName(QStringLiteral("splitter"));
				splitter->setOrientation(Qt::Horizontal);
				auto treeView_2 = new QTreeView(splitter);
				treeView_2->setObjectName(QStringLiteral("treeView_2"));
				splitter->addWidget(treeView_2);
				auto treeView = new QTreeView(splitter);
				treeView->setObjectName(QStringLiteral("treeView"));
				splitter->addWidget(treeView);

				verticalLayout->addWidget(splitter);
			}
			else
			{




				this->setLayout(new QVBoxLayout);

				{
					QWidget* bottom = new QWidget(this);
					bottom->setLayout(new QHBoxLayout());
					bottom->layout()->setMargin(2);
					bottom->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
					bottom->layout()->addWidget(new QLineEdit(bottom));
					bottom->layout()->addWidget(new QPushButton(bottom));

					this->layout()->addWidget(bottom);
				}
				

				QSplitter* sp = new QSplitter(this);
				sp->setOrientation(Qt::Horizontal);
				sp->setOpaqueResize(false);
				sp->setChildrenCollapsible(true);

				this->layout()->addWidget(sp);

				{
					QWidget* bottom = new QWidget(this);
					bottom->setLayout(new QHBoxLayout());
					bottom->layout()->setMargin(0);
					bottom->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
					{
						QLabel* lable = new QLabel(bottom);
						lable->setText("Engine/Textures/Wall");
						bottom->layout()->addWidget(lable);
					}
					this->layout()->addWidget(bottom);
				}
				sp->setSizes(QList<int>() << 32 << 128);
			}

			this->setGeometry(0, 0, 600, 600);
		}

		void Tick()
		{

		}
	};

	//////////////////////////////////////////////////////////////////////////
	class UEDITORBASE_API AssetBrowserDocWidget : public QDockWidget
	{
	public:
		AssetBrowserDocWidget(QWidget* parent = nullptr) : QDockWidget(parent)
		{
			this->setWidget(new AssetBrowserWidget);
			this->setAllowedAreas(Qt::AllDockWidgetAreas);
		}
		void Tick()
		{
			((AssetBrowserWidget*)this->widget())->Tick();
		}
	};
}