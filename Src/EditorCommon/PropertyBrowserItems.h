#pragma once

#include "PropertyBrowser.h"
#include "TreeComboBox.h"
#include "ClassComboBox.h"
#include "../Core/MetaArray.h"

namespace UEditor
{

	//////////////////////////////////////////////////////////////////////////
	class PBItemBool : public PBItemProperty
	{
	public:
		using WidgetT = QCheckBox;

		PBItemBool(PBItemBase* parent) : PBItemProperty(parent)
		{

		}
		virtual QWidget* CreatePropertyWidget() override
		{
			QCheckBox* w = new QCheckBox;
			w->setTristate(false);
			w->setText(QString());

			QObject::connect(w, &QCheckBox::stateChanged, [this]() {
				WidgetValueChanged();
			});
			return w;
		}
		

		virtual void UpdateValue() override
		{
			SetValueMapped<bool>(GetPropertyWidget<QCheckBox>()->isChecked());
		}

	};
	//////////////////////////////////////////////////////////////////////////
	class PBItemNumberic : public PBItemProperty
	{
	public:

		PBItemNumberic(PBItemBase* parent) : PBItemProperty(parent)
		{

		}
		virtual QWidget* CreatePropertyWidget() override
		{
			QDoubleSpinBox* w = new QDoubleSpinBox;
			return w;
		}
		virtual void UpdateValue() override
		{
			EMetaType type = GetNumbericType();
			SetValueMapped(float());
		}
		EMetaType GetNumbericType() const
		{
			return GetPropertyType().GetType();
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//Name, String, std::string
	class PBItemString : public PBItemProperty
	{
	public:
		PBItemString(PBItemBase* parent) : PBItemProperty(parent)
		{
		}
		virtual QWidget* CreatePropertyWidget() override
		{
			QLineEdit* w = new QLineEdit;
			return w;
		}
		virtual void UpdateValue() override
		{
			auto currentText = GetPropertyWidget<QLineEdit>()->text();
			if(IsName())
				SetValueMapped(UQString2Name(currentText));
			//#TODO String and std::string
		}
		bool IsName() const
		{
			auto pClass = dynamic_cast<const ClassInfo*>(GetPropertyType().GetPtr());
			return pClass == Name::GetClassStatic();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//Vec2, Vec3 , Vec4
	class PBItemVecN : public PBItemProperty
	{
	public:
		QDoubleSpinBox* mSpinboxes[4] = {};

		PBItemVecN(PBItemBase* parent) : PBItemProperty(parent)
		{

		}
		virtual QWidget* CreatePropertyWidget() override
		{
			QWidget* vecW = new QWidget;
			vecW->setLayout(new QHBoxLayout);

			for (unsigned i = 0; i < NumComponent(); i++)
			{
				mSpinboxes[i] = new QDoubleSpinBox;
				vecW->layout()->addWidget(mSpinboxes[i]);
			}

			return vecW;
		}
		unsigned NumComponent() const
		{
			if (auto pClass = dynamic_cast<const ClassInfo*>(GetPropertyType().GetPtr()))
			{
				if (pClass == Vec2::GetClassStatic()) return 2;
				if (pClass == Vec3::GetClassStatic()) return 3;
				if (pClass == Vec4::GetClassStatic()) return 4;
			}

			return 0;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//Color and Color32
	class PBItemColor : public PBItemProperty
	{
	public:
		PBItemColor(PBItemBase* parent) : PBItemProperty(parent)
		{

		}
		virtual QWidget* CreatePropertyWidget() override
		{
			return new QWidget();
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//Object* and TObjectPtr
	class PBItemObjectPtr : public PBItemProperty
	{
	public:
		class Widget : public QCommandLinkButton
		{
		public:
			Widget(QWidget* p = nullptr) : QCommandLinkButton(p)
			{

			}
		};

		PBItemObjectPtr(PBItemBase* parent) : PBItemProperty(parent)
		{

		}
		virtual QWidget* CreatePropertyWidget() override
		{
			auto w = new Widget;
			w->setIcon(UGetClassIcon(GetObjectClass()));
			w->setText("ttttttext");
			return w;
		}
		bool IsTObjectPtr() const
		{
			return GetPropertyType().GetType() == EMetaType::EPT_TObjectPtr;
		}

		TSubClass<Object> GetObjectClass() const
		{
			return dynamic_cast<const ClassInfo*>(GetObjectClassType().GetPtr());
		}
		const TriTypeData& GetObjectClassType() const
		{
			if (IsArrayElement())
			{
				return IsTObjectPtr() ? mPropertyInfo->GetArg0Arg0() : mPropertyInfo->GetArg0();
			}
			else
			{
				return IsTObjectPtr() ? mPropertyInfo->GetArg0() : mPropertyInfo->GetSelf();
			}
		}

	};

	//////////////////////////////////////////////////////////////////////////
	//TSubClass
	class PBItemSubClass : public PBItemProperty
	{
	public:
		PBItemSubClass(PBItemBase* parent) : PBItemProperty(parent) 
		{

		}
		const ClassInfo* GetPropertyBaseClass() const
		{
			if (mArrayIndex == -1)
				return GMetaSys()->FindClass(mPropertyInfo->GetArg0().GetTypeName());
			else
				return GMetaSys()->FindClass(mPropertyInfo->GetArg0Arg0().GetTypeName());
		}
		virtual QWidget* CreatePropertyWidget() override
		{
			auto w = new ClassComboBoxWidget();
			w->SetBaseClass(GetPropertyBaseClass());
// 			QObject::connect(w, &ClassComboBoxWidget::currentTextChanged, nullptr, [this, w](const QString&){
// 
// 			});
			return w;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//TArray
	class PBItemArray : public PBItemProperty
	{
	public:
		struct Widget : public QWidget
		{
			QPushButton* mRemoveAll = nullptr;
			QPushButton* mAddToEnd = nullptr;
			PBItemArray* mArrayItem = nullptr;

			Widget(PBItemArray* pArrayItem, QWidget* p = nullptr) : QWidget(p)
			{
				mArrayItem = pArrayItem;

				this->setLayout(new QHBoxLayout);
				
				mRemoveAll = new QPushButton(this);
				mRemoveAll->setText("C");
				mRemoveAll->setWhatsThis("Remove all elements");
				this->layout()->addWidget(mRemoveAll);

				mAddToEnd = new QPushButton(this);
				mAddToEnd->setText("+");
				mAddToEnd->setWhatsThis("Add a new default element to the end of array");
				this->layout()->addWidget(mAddToEnd);

				QObject::connect(mRemoveAll, &QPushButton::clicked, this, [this](bool) {
					this->mArrayItem->RemoveAllElements();
				});
				
				QObject::connect(mAddToEnd, &QPushButton::clicked, this, [this](bool) {
					this->mArrayItem->AddToEnd();
				});

			}


		};

		PBItemArray(PBItemBase* parent) : PBItemProperty(parent)
		{
		}
		virtual QWidget* CreatePropertyWidget() override
		{
			return new PBItemArray::Widget(this);
		}
		void CreateChildren() override;
		EMetaType GetElementType() const
		{
			return mPropertyInfo->GetArg0().GetType();
		}
		//user clicked to remove all the elements
		void RemoveAllElements();
		void AddToEnd();
		MetaArrayView GetArrayView() const;
	};
	//////////////////////////////////////////////////////////////////////////
	//enum
	class PBItemEnum : public PBItemProperty
	{
	public:
		PBItemEnum(PBItemBase* parent) : PBItemProperty(parent)
		{}

	};
};