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
		
		
		virtual void UpdateWidgetValue() override
		{
			GetPropertyWidget<QCheckBox>()->setChecked(this->GetValueRef<bool>());
		}
		virtual void UpdateValue() override
		{
			ULOG_MESSAGE("Cur State %", this->GetPropertyWidget<QCheckBox>()->isChecked());
			SetValueMapped<bool>(GetPropertyWidget<QCheckBox>()->isChecked());
			ULOG_MESSAGE("After Asgin State %", *((bool*)this->GetValuePtr()));
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
			w->setDecimals(IsFlotingPoint() ? 5 : 0);
			
			float minval = 0;
			float maxval = 0;

			if (auto pAttr = mPropertyInfo->GetAttribute<AttrMinMax>())
			{
				w->setMinimum(pAttr->mMin);
				w->setMaximum(pAttr->mMax);
			}

			QObject::connect(w, static_cast<void(QDoubleSpinBox::*) (double)>(&QDoubleSpinBox::valueChanged), [this](double v) {
				this->WidgetValueChanged();
			});
			return w;
		}
		virtual void UpdateValue() override
		{
			EMetaType type = GetNumbericType();
			
			switch (type)
			{
			case EMetaType::EPT_int32:
				SetValueMapped(static_cast<int32>(GetPropertyWidget<QDoubleSpinBox>()->value()));
				break;
			case EMetaType::EPT_float:
				SetValueMapped(static_cast<float>(GetPropertyWidget<QDoubleSpinBox>()->value()));
				break;
			}
		}
		EMetaType GetNumbericType() const
		{
			return GetPropertyType().GetType();
		}
		bool IsFlotingPoint() const
		{
			return GetNumbericType() == EMetaType::EPT_float || GetNumbericType() == EMetaType::EPT_double;
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
			if (IsMultiLine())
			{
				QPlainTextEdit* w = new QPlainTextEdit;
				w->setLineWrapMode(QPlainTextEdit::NoWrap);
				w->setMinimumHeight(160);

				QObject::connect(w, &QPlainTextEdit::textChanged, [this]() {
					WidgetValueChanged();
				});
				return w;
			}
			else
			{
				QLineEdit* w = new QLineEdit;
				w->setClearButtonEnabled(true);

				QObject::connect(w, &QLineEdit::editingFinished, [this]() {
					this->WidgetValueChanged();
				});
				return w;
			}
		}
		virtual void UpdateWidgetValue() override
		{
			QString str;
			if (GetPropertyClass() == Name::GetClassStatic()) //is UCore::Name?
				str = UToQString(GetValueRef<Name>());
			else if (GetPropertyClass() == String::GetClassStatic()) // is UCore::String
				str = UToQString(GetValueRef<String>());


			if (IsMultiLine())
				GetPropertyWidget<QPlainTextEdit>()->setPlainText(str);
			else
				GetPropertyWidget<QLineEdit>()->setText(str);
		}
		virtual void UpdateValue() override
		{
			QString currentText;
			if (IsMultiLine())
				currentText = GetPropertyWidget<QPlainTextEdit>()->toPlainText();
			else
				currentText = GetPropertyWidget<QLineEdit>()->text();

			if (GetPropertyClass() == Name::GetClassStatic()) //is UCore::Name
				SetValueMapped<Name>(UQString2Name(currentText));
			else if (GetPropertyClass() == String::GetClassStatic()) //is UCore::String
				SetValueMapped<String>(UQString2String(currentText));
		}
		const ClassInfo* GetPropertyClass() const
		{
			auto ret = dynamic_cast<const ClassInfo*>(GetPropertyType().GetPtr());
			UASSERT(ret);
			return ret;
		}
		bool IsName() const
		{
			return GetPropertyClass() == Name::GetClassStatic();
		}
		bool IsMultiLine() const
		{
			return mPropertyInfo->GetAttribute<AttrMultiLine>() != nullptr;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//Vec2, Vec3 , Vec4
	class PBItemVecN : public PBItemProperty
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		class Widget : public QWidget
		{
		public:
			QDoubleSpinBox* mSpinBoxes[4] = {};
			PBItemVecN* mItem;

			Widget(PBItemVecN* pItem, QWidget* parent = nullptr) : QWidget(parent)
			{
				mItem = pItem;

				this->setLayout(new QHBoxLayout);
				
				for (unsigned i = 0; i < pItem->GetNumComponent(); i++)
				{
					static const char* LUTText[] = { "X", "Y", "Z", "W" };

					mSpinBoxes[i] = new QDoubleSpinBox;
					this->layout()->addWidget(mSpinBoxes[i]);
					mSpinBoxes[i]->setToolTip(LUTText[i]);
				}
			}
		};

		PBItemVecN(PBItemBase* parent) : PBItemProperty(parent)
		{

		}
		virtual QWidget* CreatePropertyWidget() override
		{
			auto w = new Widget(this);
			return w;
		}
		unsigned GetNumComponent() const
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
		//////////////////////////////////////////////////////////////////////////
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