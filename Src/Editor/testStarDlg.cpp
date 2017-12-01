#if 0
#include "EditorBase.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qsplashscreen.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/qitemeditorfactory.h>

#include "../EditorCommon/FlowLayout.h"


#if 0
static const unsigned PaintingScaleFactor = 2;

//////////////////////////////////////////////////////////////////////////
class StarRating
{
public:
	enum EditMode { Editable, ReadOnly };

	//////////////////////////////////////////////////////////////////////////
	explicit StarRating(int starCount = 1, int maxStarCount = 5)
	{
		myStarCount = starCount;
		myMaxStarCount = maxStarCount;

		starPolygon << QPointF(1.0, 0.5);
		for (int i = 1; i < 5; ++i)
			starPolygon << QPointF(0.5 + 0.5 * std::cos(0.8 * i * 3.14),
				0.5 + 0.5 * std::sin(0.8 * i * 3.14));

		diamondPolygon << QPointF(0.4, 0.5) << QPointF(0.5, 0.4)
			<< QPointF(0.6, 0.5) << QPointF(0.5, 0.6)
			<< QPointF(0.4, 0.5);
	}
	//////////////////////////////////////////////////////////////////////////
	void paint(QPainter *painter, const QRect &rect, const QPalette &palette, EditMode mode) const
	{
		painter->save();

		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setPen(Qt::NoPen);

		if (mode == Editable) {
			painter->setBrush(palette.highlight());
		}
		else {
			painter->setBrush(palette.foreground());
		}

		int yOffset = (rect.height() - PaintingScaleFactor) / 2;
		painter->translate(rect.x(), rect.y() + yOffset);
		painter->scale(PaintingScaleFactor, PaintingScaleFactor);

		for (int i = 0; i < myMaxStarCount; ++i) {
			if (i < myStarCount) {
				painter->drawPolygon(starPolygon, Qt::WindingFill);
			}
			else if (mode == Editable) {
				painter->drawPolygon(diamondPolygon, Qt::WindingFill);
			}
			painter->translate(1.0, 0.0);
		}

		painter->restore();
	}
	//////////////////////////////////////////////////////////////////////////
	QSize sizeHint() const
	{
		return PaintingScaleFactor * QSize(myMaxStarCount, 1);
	}
	int starCount() const { return myStarCount; }
	int maxStarCount() const { return myMaxStarCount; }
	void setStarCount(int starCount) { myStarCount = starCount; }
	void setMaxStarCount(int maxStarCount) { myMaxStarCount = maxStarCount; }

private:
	QPolygonF starPolygon;
	QPolygonF diamondPolygon;
	int myStarCount;
	int myMaxStarCount;
};

Q_DECLARE_METATYPE(StarRating)

//////////////////////////////////////////////////////////////////////////
class StarEditor : public QWidget
{
	//Q_OBJECT
	W_OBJECT(StarEditor)

public:
	StarEditor(QWidget *parent = 0)
	{
		setMouseTracking(true);
		setAutoFillBackground(true);
	}

	QSize sizeHint() const override
	{
		return myStarRating.sizeHint();
	}
	void setStarRating(const StarRating &starRating) {
		myStarRating = starRating;
	}
	StarRating starRating() { return myStarRating; }

//signals:
	void editingFinished();
// 	W_SIGNAL(editingFinished)

protected:
	void paintEvent(QPaintEvent *event) override
	{
		QPainter painter(this);
		myStarRating.paint(&painter, rect(), this->palette(), StarRating::Editable);
	}
	void mouseMoveEvent(QMouseEvent *event) override
	{
		int star = starAtPosition(event->x());

		if (star != myStarRating.starCount() && star != -1) {
			myStarRating.setStarCount(star);
			update();
		}
	}
	void mouseReleaseEvent(QMouseEvent *event) override
	{
		emit editingFinished();
	}

private:
	int starAtPosition(int x)
	{
		int star = (x / (myStarRating.sizeHint().width()
			/ myStarRating.maxStarCount())) + 1;
		if (star <= 0 || star > myStarRating.maxStarCount())
			return -1;

		return star;
	}

	StarRating myStarRating;
};

#include <verdigris/wobjectimpl.h>


//////////////////////////////////////////////////////////////////////////
class StarDelegate : public QStyledItemDelegate
{
public:
	StarDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override
	{
		if (index.data().canConvert<StarRating>()) {
			StarRating starRating = qvariant_cast<StarRating>(index.data());

			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());

			starRating.paint(painter, option.rect, option.palette,
				StarRating::ReadOnly);
		}
		else 
		{
			QStyledItemDelegate::paint(painter, option, index);
		}
	}
	//The sizeHint() function returns an item's preferred size:
	QSize sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const override
	{
		if (index.data().canConvert<StarRating>()) {
			StarRating starRating = qvariant_cast<StarRating>(index.data());
			return starRating.sizeHint();
		}
		else {
			return QStyledItemDelegate::sizeHint(option, index);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//The createEditor() function is called when the user starts editing an item:
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		if (index.data().canConvert<StarRating>()) {
			StarEditor *editor = new StarEditor(parent);
			connect(editor, &StarEditor::editingFinished, this, &StarDelegate::commitAndCloseEditor);
			return editor;
		}
		else {
			return QStyledItemDelegate::createEditor(parent, option, index);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//The setEditorData() function is called when an editor is created to initialize it with data from the model:
	void setEditorData(QWidget *editor, const QModelIndex &index) const override
	{
		if (index.data().canConvert<StarRating>()) {
			StarRating starRating = qvariant_cast<StarRating>(index.data());
			StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
			starEditor->setStarRating(starRating);
		}
		else {
			QStyledItemDelegate::setEditorData(editor, index);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//The setModelData() function is called when editing is finished, to commit data from the editor to the model:
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
	{
		if (index.data().canConvert<StarRating>()) {
			StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
			model->setData(index, QVariant::fromValue(starEditor->starRating()));
		}
		else {
			QStyledItemDelegate::setModelData(editor, model, index);
		}
	}

	private slots:
	void commitAndCloseEditor()
	{
		StarEditor *editor = qobject_cast<StarEditor *>(sender());
		emit commitData(editor);
		emit closeEditor(editor);
	}
};



void populateTableWidget(QTableWidget *tableWidget)
{
	static const struct {
		const char *title;
		const char *genre;
		const char *artist;
		int rating;

	} staticData[] = {
		{ "Mass in B-Minor", "Baroque", "J.S. Bach", 5 },
		
		{ 0, 0, 0, 0 }
	};

	for (int row = 0; staticData[row].title != 0; ++row) {
		QTableWidgetItem *item0 = new QTableWidgetItem(staticData[row].title);
		QTableWidgetItem *item1 = new QTableWidgetItem(staticData[row].genre);
		QTableWidgetItem *item2 = new QTableWidgetItem(staticData[row].artist);
		QTableWidgetItem *item3 = new QTableWidgetItem;
		item3->setData(0,
			QVariant::fromValue(StarRating(staticData[row].rating)));

		tableWidget->setItem(row, 0, item0);
		tableWidget->setItem(row, 1, item1);
		tableWidget->setItem(row, 2, item2);
		tableWidget->setItem(row, 3, item3);
	}
}
#endif

class QExpandingLineEdit : public QLineEdit
{

public:
	QExpandingLineEdit(QWidget *parent);

	void setWidgetOwnsGeometry(bool value)
	{
		widgetOwnsGeometry = value;
	}

protected:
	void changeEvent(QEvent *e) Q_DECL_OVERRIDE;

	public Q_SLOTS:
	void resizeToContents();

private:
	void updateMinimumWidth();

	int originalWidth;
	bool widgetOwnsGeometry;
};

QExpandingLineEdit::QExpandingLineEdit(QWidget *parent)
	: QLineEdit(parent), originalWidth(-1), widgetOwnsGeometry(false)
{
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(resizeToContents()));
	updateMinimumWidth();
}

void QExpandingLineEdit::changeEvent(QEvent *e)
{
	switch (e->type())
	{
	case QEvent::FontChange:
	case QEvent::StyleChange:
	case QEvent::ContentsRectChange:
		updateMinimumWidth();
		break;
	default:
		break;
	}

	QLineEdit::changeEvent(e);
}

void QExpandingLineEdit::updateMinimumWidth()
{
	int left, right;
	getTextMargins(&left, 0, &right, 0);
	int width = left + right + 4 /*horizontalMargin in qlineedit.cpp*/;
	getContentsMargins(&left, 0, &right, 0);
	width += left + right;

	QStyleOptionFrame opt;
	initStyleOption(&opt);

	int minWidth = style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(width, 0).
		expandedTo(QApplication::globalStrut()), this).width();
	setMinimumWidth(minWidth);
}

void QExpandingLineEdit::resizeToContents()
{
	int oldWidth = width();
	if (originalWidth == -1)
		originalWidth = oldWidth;
	if (QWidget *parent = parentWidget()) {
		QPoint position = pos();
		int hintWidth = minimumWidth() + fontMetrics().width(displayText());
		int parentWidth = parent->width();
		int maxWidth = isRightToLeft() ? position.x() + oldWidth : parentWidth - position.x();
		int newWidth = qBound(originalWidth, hintWidth, maxWidth);
		if (widgetOwnsGeometry)
			setMaximumWidth(newWidth);
		if (isRightToLeft())
			move(position.x() - newWidth + oldWidth, position.y());
		resize(newWidth, height());
	}
}

//////////////////////////////////////////////////////////////////////////
class MyDelegate : public QStyledItemDelegate
{
public:
	MyDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

	void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
#if 0
		if (index.data().canConvert<StarRating>()) {
			StarRating starRating = qvariant_cast<StarRating>(index.data());

			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());

			starRating.paint(painter, option.rect, option.palette,
				StarRating::ReadOnly);
		}
		else
		{
			QStyledItemDelegate::paint(painter, option, index);
		}
		return;
#endif // 
		if (index.row() == 1)
		{
			QStyleOptionViewItem opt = option;
			initStyleOption(&opt, index);
			
			const QWidget *widget = option.widget;
			QStyle *style = widget ? widget->style() : QApplication::style();
			//style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
			auto palette = option.palette;
			auto rect = option.rect;

			painter->save();

			painter->setRenderHint(QPainter::Antialiasing, true);
			painter->setPen(Qt::NoPen);

			painter->setBrush(palette.highlight());


			if (option.state & QStyle::State_Selected)
				painter->fillRect(option.rect, option.palette.highlight());



			painter->drawEllipse(option.rect);
		}
		else
		{
			QStyledItemDelegate::paint(painter, option, index);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//The createEditor() function is called when the user starts editing an item:
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{

		// 		if (index.data().canConvert<StarRating>()) {
		// 			StarEditor *editor = new StarEditor(parent);
		// 			connect(editor, &StarEditor::editingFinished, this, &StarDelegate::commitAndCloseEditor);
		// 			return editor;
		// 		}
		// 		else 
		{
			//return new QSpinBox(parent);
		}
		{
			QWidget* w = QStyledItemDelegate::createEditor(parent, option, index);
			ULOG_SUCCESS(w->metaObject()->className());
			return w;
		}
	}

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		return QStyledItemDelegate::sizeHint(option, index);
		//return QSize(200, 200);
	}

};
class ABAssetsList : public QAbstractListModel
{
public:
	ABAssetsList(QObject* p) : QAbstractListModel(p)
	{

	}
};

int main2(int argc, char** argv)
{
	using namespace UEditor;

	QApplication app(argc, argv);


	QListWidget* m_listeWidget = new QListWidget;
	const char* cn = m_listeWidget->itemDelegate()->metaObject()->className();
	m_listeWidget->setItemDelegate(new MyDelegate);
	m_listeWidget->setViewMode(QListWidget::IconMode);
	m_listeWidget->setIconSize(QSize(200, 200));
	m_listeWidget->setMovement(QListView::Static);
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

};
#endif // _DEBUG
