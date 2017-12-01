#include "ClassComboBox.h"

namespace UEditor
{



ClassComboBoxWidget::ClassComboBoxWidget(QWidget* parent /*= nullptr*/) : QComboBox(parent), skipNextHide(false)
{
	QTreeView* v = new QTreeView(this);
	v->setItemsExpandable(false);
	setView(v);
	v->header()->hide();
	v->viewport()->installEventFilter(this);
	v->setMinimumHeight(300);

	SetBaseClass(nullptr);
	//auto Func = static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	//QObject::connect(this, &QComboBox::currentTextChanged, this, &SelfT::EvCurrentIndexChanged);
}

QTreeView* ClassComboBoxWidget::GetView() const
{
	return (QTreeView*)this->view();
}

QStandardItemModel* ClassComboBoxWidget::GetModel() const
{
	return (QStandardItemModel*)this->model();
}

void ClassComboBoxWidget::SetBaseClass(const ClassInfo* pBaseClass)
{
	QStandardItemModel* model = new QStandardItemModel;

	QStandardItem* itemNull = new QStandardItem("Null");
	model->appendRow(itemNull);

	QStandardItem* parentItem = model->invisibleRootItem();

	TArray<const ClassInfo*> classes;

	if (pBaseClass)
	{
		classes.Add(pBaseClass);
	}
	else
	{
		GMetaSys()->GetSubClassesOf(nullptr, classes);
	}


	for (auto pClass : classes)
	{
		QStandardItem* item = new QStandardItem;
		parentItem->appendRow(item);
		item->setText(UToQString(pClass->GetName()));
		item->setIcon(UGetClassIcon(pClass));
		_SetClass(pClass, item);
	}

	this->setModel(model);
	GetView()->expandAll();
}

const ClassInfo* ClassComboBoxWidget::GetCurrentClass() const
{
	return GMetaSys()->FindClass(UQString2Name(this->currentText()));
}

bool ClassComboBoxWidget::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		QModelIndex index = view()->indexAt(mouseEvent->pos());
		if (!view()->visualRect(index).contains(mouseEvent->pos()))
			skipNextHide = true;
	}
	return false;
}

void ClassComboBoxWidget::_SetClass(const ClassInfo* pParentClass, QStandardItem* parentItem)
{
	for (const ClassInfo* iter : pParentClass->GetSubClasses())
	{
		QStandardItem* item = new QStandardItem;
		parentItem->appendRow(item);

		item->setText(UToQString(iter->GetName()));
		item->setIcon(UGetClassIcon(iter));
		_SetClass(iter, item);
	}
}

void ClassComboBoxWidget::showPopup()
{
	setRootModelIndex(QModelIndex());
	QComboBox::showPopup();
}

void ClassComboBoxWidget::hidePopup()
{
	setRootModelIndex(view()->currentIndex().parent());
	setCurrentIndex(view()->currentIndex().row());
	if (skipNextHide)
		skipNextHide = false;
	else
		QComboBox::hidePopup();
}



};