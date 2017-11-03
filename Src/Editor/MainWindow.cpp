#include "MainWindow.h"

#include "../EditorBase/Log.h"
#include "../EditorBase/Filter.h"
#include "../EditorBase/AssetBrowser.h"

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qpainter.h>

namespace UEditor
{



	MainWinfow::MainWinfow(QWidget* parent) : QMainWindow(parent)
	{
		setMinimumSize(400, 400);

		setCentralWidget(new QWidget);
		centralWidget()->setMinimumSize(300, 300);
		centralWidget()->setLayout(new QVBoxLayout);
		for (int i = 0; i < 64; i++);
		{
			centralWidget()->layout()->addWidget(new QPushButton(centralWidget()));
		}
		//log
		{
			mLog = new LogDockWidget(this);
			this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, mLog);
		}
		//asset browser
		{
			mAssetBrowser = new AssetBrowserDocWidget(this);
			mAssetBrowser->setGeometry(0, 0, 300, 300);
			this->addDockWidget(Qt::RightDockWidgetArea, mAssetBrowser);
		}

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
	}

	void MainWinfow::Tick()
	{
		mLog->Tick();

	}

	QPaintEngine * MainWinfow::paintEngine() const
	{
		QPaintEngine* pe = QMainWindow::paintEngine();
		return pe;
	}

	void MainWinfow::paintEvent(QPaintEvent *event)
	{
		QPainter& p = QPainter(this);
		
		QMainWindow::paintEvent(event);
	}

};