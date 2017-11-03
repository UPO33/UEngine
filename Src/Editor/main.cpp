#include "Base.h"



#include "MainWindow.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qsplashscreen.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/QtWidgets>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPaintDevice>

namespace UEditor
{
	MainWinfow*	gMainWindow;
};

class EditableLableWidget : public QWidget
{
public:
	QLabel* mLable;
	QLineEdit* mLineEdit;

	void UResizeLineEditToContents(QLineEdit* le)
	{
		QString text = le->text();
		QFontMetrics fm = le->fontMetrics();
		int w = fm.boundingRect(text).width();
		le->resize(w, le->height());
	}

	struct Lbl : QLabel
	{
		EditableLableWidget* mEW;
		Lbl(QWidget* parent, EditableLableWidget* ew) : QLabel(parent), mEW(ew)
		{

		}
		void mouseDoubleClickEvent(QMouseEvent * ev) override
		{
			if (ev->button() == Qt::MouseButton::LeftButton)
			{
				ev->accept();
				this->setVisible(false);
				mEW->mLineEdit->setVisible(true);
			}
		}
	};
	EditableLableWidget(QWidget* parent) : QWidget(parent)
	{
		this->setLayout(new QVBoxLayout);
		
		mLable = new Lbl(this, this);
		layout()->addWidget(mLable);
		mLable->setText("asdasdasdad");

		mLineEdit = new QLineEdit(this);
		layout()->addWidget(mLineEdit);
		//mLineEdit->setFixedWidth(64);

		mLineEdit->setText(mLable->text());
		mLineEdit->setVisible(false);

		connect(mLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
			UResizeLineEditToContents(mLineEdit);
		});
		connect(mLineEdit, &QLineEdit::returnPressed, this, [this]() {
			mLineEdit->setVisible(false);
			mLable->setText(mLineEdit->text());
			mLable->setVisible(true);
		});

/*
		this->setLayout(new QVBoxLayout);
		mLineEdit = new QLineEdit(this);
		layout()->addWidget(mLineEdit);
		mLineEdit->setText("fghfgh");

		mLineEdit->setReadOnly(false);
		QPalette mEditable = mLineEdit->palette();  // Default colors
		mLineEdit->setReadOnly(true);
		QPalette  mNonEditable = mLineEdit->palette();
		QColor col = mNonEditable.color(QPalette::Button);
		mNonEditable.setColor(QPalette::Base, col);
		mNonEditable.setColor(QPalette::Text, Qt::black);

		mLineEdit->setReadOnly(true);
		mLineEdit->setPalette(mNonEditable);
		*/

	}
};


#if 1
int main(int argc, char** argv)
{
	using namespace UEditor;

	
	QApplication app(argc, argv);

	 QCoreApplication::setOrganizationName("UPOSoft");
	 QCoreApplication::setOrganizationDomain("UPO33.com");
	 QCoreApplication::setApplicationName("UEditor");
	 QCoreApplication::setApplicationVersion("0.0");

	 QLocale::setDefault(QLocale::c());

	 //QApplication::setStyle("Fusion");

	 struct CPW : QOpenGLWidget
	 {
		 typedef QOpenGLWidget ParentT;

	 protected:
		 virtual void paintEvent(QPaintEvent *event) override
		 {
			 ParentT::paintEvent(event);
		 }

	 public:
		 virtual QPaintEngine * paintEngine() const override
		 {
			 QPaintEngine* pe = ParentT::paintEngine();
			 return nullptr;
		 }

	 };
	 auto* glWnd = new CPW;
	 new QPushButton(glWnd);
	 glWnd->show();
	 return app.exec();
	 
	 {
		 gMainWindow = new MainWinfow(glWnd);
		 gMainWindow->showMaximized();

		 while (gMainWindow->isVisible())
		 {
			 QApplication::processEvents();

			 if (rand() % 7 == 0)
			 {
				 ULOG_SUCCESS("sscessss %", rand());
				 ULOG_ERROR("errorrr %", rand());
			 }
			 gMainWindow->Tick();

			 QThread::usleep(10);
		 }
		 delete gMainWindow;
	 }
};
#endif // 0