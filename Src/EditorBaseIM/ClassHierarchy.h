#pragma once
#include "EditorBaseIM.h"
#include "../Core/Meta.h"

namespace UEditor
{
	struct DClassTree
	{
		const ClassInfo*	mBaseClass = nullptr;	//base class if any
		const ClassInfo*	mSelectedClass = nullptr;
		DFilterBox			mFilter;
		
		void Draw()
		{
			ImGui::PushID(this);
			if (mFilter.Draw())
			{
				mSelectedClass = nullptr;
			}

			if(mBaseClass)
				RecDraw2(mBaseClass->GetSubClasses());
			else
			{
				TArray<const ClassInfo*> roots;
				for (const FieldInfo* typeIter : GMetaSys()->GetAllTypes())
				{
					if (const ClassInfo* pClass = Cast<ClassInfo>(typeIter))
					{
						if (!pClass->HasParent())
							roots.Add(pClass);
					}
				}
				RecDraw2(roots);
			}
			ImGui::PopID();
		}
		bool CheckClassFilter(const ClassInfo* pClass) const
		{
			if (mFilter.CheckFilter(pClass->GetName()))
			{
				return true;
			}
			for (const ClassInfo* iter : pClass->GetSubClasses())
			{
				if (CheckClassFilter(iter))
					return true;
			}
			return false;
		}
		void RecDraw2(const TArray<const ClassInfo*>& classes)
		{
			ImGui::Indent();
			for (const ClassInfo* pClass : classes)
			{
				if (pClass && CheckClassFilter(pClass))
				{
					ImGui::PushID(pClass);
					if (ImGui::Selectable(pClass->GetName(), pClass == mSelectedClass, ImGuiSelectableFlags_DrawFillAvailWidth))
					{
						mSelectedClass = pClass;
					}
					ImGui::PopID();
					RecDraw2(pClass->GetSubClasses());
				}
			}
			ImGui::Unindent();
		}
	};
	class WClassTree : public WBase
	{
	public:
		DClassTree mClassTree;
		DClassTree ct2;

		WClassTree() : WBase(true, "ClassTree")
		{
		}
		virtual void DrawContents() override
		{
			ImGui::Separator();
			mClassTree.Draw();
			if (ImGui::Button("Select Class")) 
			{
				ImGui::OpenPopup("ClassSelectorPopup");
			}
			if (ImGui::BeginPopup("ClassSelectorPopup"))
			{
				ct2.Draw();

				ImGui::EndPopup();
			}
		}
		void DrawClassMenu()
		{
			
		}
	};
};