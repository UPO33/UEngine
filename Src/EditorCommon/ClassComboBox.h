#pragma once 

#include "TreeComboBox.h"
#include "../Core/Meta.h"

namespace UEditor
{
	class ClassComboBoxWidget : public QComboBox
	{
	public:
		using SelfT = ClassComboBoxWidget;

		ClassComboBoxWidget(QWidget* parent = nullptr);
		QTreeView* GetView() const;
		QStandardItemModel* GetModel() const;

		void SetBaseClass(const ClassInfo* pBaseClass);
		const ClassInfo* GetCurrentClass() const;

	protected:
		bool eventFilter(QObject* object, QEvent* event);
		void _SetClass(const ClassInfo* pParentClass, QStandardItem* parentItem);
		virtual void showPopup();

		virtual void hidePopup();

	private:
		bool skipNextHide;
	};

}