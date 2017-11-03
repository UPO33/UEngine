#pragma once

#include "../Core/Base.h"
#include "../Core/SmartPointers.h"
#include "../Engine/Base.h"
#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_internal.h"


#define UEDITORBASE_API 


namespace UEditor
{
	using namespace UCore;
	using namespace UEngine;



	class UEDITORBASE_API WBase : public ISmartBase
	{
	public:
		//whether window is open or closed
		bool				mOpen = true;
		const char*			mName = nullptr;
		ImGuiWindowFlags	mFlags = 0;
		ImVec2				mPosition = ImVec2(0, 0);
		bool				mPaneable = true;

		WBase(bool isOpen, const char* name, ImGuiWindowFlags flags = 0, ImVec2 initialPos = ImVec2(0,0))
		{
			mOpen = isOpen;
			mName = name;
			mFlags = flags;
			mPosition = initialPos;
		}
		void Draw()
		{
			ImGui::SetNextWindowPos(mPosition, ImGuiSetCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);

			if (mPaneable && ImGui::IsMouseDown(2))
			{
				mPosition += ImGui::GetIO().MouseDelta;
				ImGui::SetNextWindowPos(mPosition, ImGuiSetCond_Always);
			}
			if (mOpen)
			{
				ImGui::Begin(mName, &mOpen, mFlags);
				DrawMenu();
				DrawContents();
				mPosition = ImGui::GetWindowPos();
				ImGui::End();
			}
		}

		virtual void DrawContents() {}
		virtual void DrawMenu() {}
	};

	//////////////////////////////////////////////////////////////////////////
	struct DFilterBox
	{
		char	mText[512] = {};
		
		using StrRangeT = TStrRange<char>;

		TArray<StrRangeT, TArrayAllocStack<StrRangeT, 128>>	mFilters;

		void Clear()
		{
			mText[0] = 0;
			mFilters.RemoveAll();
		}
		bool Draw()
		{
			ImGui::PushID(this);
			if (ImGui::Button("C"))
			{
				Clear();
			}
			ImGui::SameLine();
			if(ImGui::IsItemHovered())
				ImGui::SetTooltip("Clear filter");

			bool changed = ImGui::InputText("Filter", mText, sizeof(mText));
			
			if(changed)
			{
				mFilters.RemoveAll();
				USplitString(mText, " \t,", mFilters);
			}
			ImGui::PopID();
		
			return changed;
		}
		bool CheckFilter(const char* text) const
		{
			if (mFilters.Length() == 0)
				return true;
			if (text == nullptr)
				return false;

			for (const StrRangeT& range : mFilters)
			{
				if (UStrStr(text, range, false))
				{
					return true;
				}
			}
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct DFilteredContextMenu
	{
		bool	mIsFirst = false;
		char	mFilterText[256] = {};
		
		using StrRangeT = TStrRange<char>;
		TArray<StrRangeT, TArrayAllocStack<StrRangeT, 128>>	mFilters;

		bool CheckFilter(const char* text)
		{
			if (mFilters.Length() == 0)
				return true;
			if (text == nullptr)
				return false;

			for (const StrRangeT& range : mFilters)
			{
				if (UStrStr(text, range, false))
					return true;
			}
			return false;
		}
		void Draw()
		{
			ImGui::PushID(this);
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
			{
				ImGui::OpenPopup("FilteredContextMenu");
				mIsFirst = true;
			}
			if (ImGui::BeginPopup("FilteredContextMenu"))
			{
				if (mIsFirst)
				{
					ImGui::SetKeyboardFocusHere();
					mIsFirst = false;
				}
				if (ImGui::InputText("", mFilterText, sizeof(mFilterText)))
				{
					USplitString(mFilterText, " \t,", mFilters);
				}


				//ImGui::MenuItem("menu0");
				//ImGui::MenuItem("menu1");

				DrawContents();

				ImGui::EndPopup();
			}
			ImGui::PopID();
		}
		virtual void DrawContents() {}

	};

	
};
