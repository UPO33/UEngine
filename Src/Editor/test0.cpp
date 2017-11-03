#include "Base.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qsplashscreen.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/QtWidgets>

#include "../EditorBase/FlowLayout.h"

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

	//////////////////////////////////////////////////////////////////////////
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
				mEW->mLineEdit->selectAll();
				mEW->mLineEdit->setFocus();
			}
		}
	};
	struct LEd : QLineEdit
	{
		EditableLableWidget* mEW;
		LEd(QWidget* p, EditableLableWidget* ew) : QLineEdit(p), mEW(ew)
		{

		}
	protected:
		virtual void focusOutEvent(QFocusEvent *event) override
		{
			QLineEdit::focusOutEvent(event);
		}
		virtual bool eventFilter(QObject *obj, QEvent *event) override
		{
			Q_UNUSED(obj)
				if (event->type() == QEvent::FocusIn)
				{

				}
				else if (event->type() == QEvent::FocusOut)
				{

				}

			return false;
		}

	};
	EditableLableWidget(QWidget* parent) : QWidget(parent)
	{
		this->setLayout(new QVBoxLayout);

		mLable = new Lbl(this, this);
		layout()->addWidget(mLable);
		mLable->setText("asdasdasdad");

		mLineEdit = new LEd(this, this);
		layout()->addWidget(mLineEdit);
		//mLineEdit->setFixedWidth(64);

		mLineEdit->setText(mLable->text());
		mLineEdit->setVisible(false);

		connect(mLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
			UResizeLineEditToContents(mLineEdit);
		});
		connect(mLineEdit, &QLineEdit::editingFinished, this, [this]() {
		
			

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

class ABItemWidget : public QWidget
{
public:
	QWidget* mThumbnail = nullptr;
	EditableLableWidget* mLable = nullptr;

	ABItemWidget(QWidget* p) : QWidget(p)
	{
		this->setLayout(new QVBoxLayout);

		mThumbnail = new QWidget(this);
		mThumbnail->setMinimumSize(64, 64);
		mThumbnail->setStyleSheet(QStringLiteral("background-color: rgb(255, 0, 4);"));

		layout()->addWidget(mThumbnail);

		mLable = new EditableLableWidget(this);
		this->layout()->addWidget(mLable);

	}
};

#if 1
int ___main(int argc, char** argv)
{
	using namespace UEditor;

	QApplication app(argc, argv);

	QLocale::setDefault(QLocale::c());

	//QApplication::setStyle("Fusion");
	if(1)
	{
		//QListView* lw = new QListView;
		//lw->setViewMode(QListView::IconMode);
		//lw->setIconSize(QSize(32, 32));

		QListWidget* m_listeWidget = new QListWidget;
		const char* cn = m_listeWidget->itemDelegate()->metaObject()->className();

		m_listeWidget->setViewMode(QListWidget::IconMode);
		m_listeWidget->setIconSize(QSize(200, 200));

		m_listeWidget->setResizeMode(QListWidget::Adjust);
		
		auto icon = QIcon("../../a.jpg");
		for (size_t i = 0; i < 64; i++)
		{
			auto ni = new QListWidgetItem(icon, "asdasda");
			ni->setFlags(ni->flags() | Qt::ItemFlag::ItemIsEditable);
			m_listeWidget->addItem(ni);
		}

		m_listeWidget->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
		m_listeWidget->show();
		return app.exec();
	}
	{
		
		QWidget* widget = new QWidget;
		widget->setLayout(new UEditor::FlowLayout);
		for (size_t i = 0; i < 32; i++)
		{
			ABItemWidget* ab = new ABItemWidget(widget);
			widget->layout()->addWidget(ab);
		}
		widget->show();
		return app.exec();
	}
	{

		QTableWidget* table = new QTableWidget;
		table->setColumnCount(3);
		table->setRowCount(3);

		table->setItem(0, 0, new QTableWidgetItem("aaa"));
		table->setCellWidget(0, 1, new QLineEdit());

		table->setShowGrid(true);
		table->setGridStyle(Qt::DotLine);
		table->horizontalHeader()->setVisible(false);
		table->verticalHeader()->setVisible(false);

		table->show();
		return app.exec();
	}
	{
		QWidget* w = new EditableLableWidget(nullptr);
		w->show();
		return app.exec();
	}
};
#endif // 0