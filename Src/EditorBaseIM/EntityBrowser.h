#pragma once

#include "EditorBaseIM.h"
#include "../Engine/Entity.h"
#include "../Engine/EntityPrimitive.h"
#include "../Core/Meta.h"

namespace UEditor
{
// 	void UGetEdInstancableEntities(TArray<const ClassInfo*>& outClasses)
// 	{
// 		Entity::GetClassStatic();
// 
// 		auto l = [](const ClassInfo* pc)
// 		{
// 			pc->GetAttribute<AttrHidden>();
// 		};
// 	}


	class WEntityBrowser : public WBase
	{
	public:
		

	

		//////////////////////////////////////////////////////////////////////////
		struct TreeNode
		{
			TreeNode*			mParent = nullptr;
			TArray<TreeNode*>	mChildren;
			Entity*				mEntity = nullptr;

			void AddChild(TreeNode* child)
			{
				UASSERT(child && child->mParent == nullptr);
				mChildren.Add(child);
				child->mParent = this;
			}
			TreeNode* AddChild(Entity* pEntity)
			{
				TreeNode* node = new TreeNode();
				node->mEntity = pEntity;
				AddChild(node);
				return node;
			}
			~TreeNode()
			{
				for (TreeNode* child : mChildren)
				{
					delete child;
				}
				mChildren.RemoveAll();
			}

		};

		struct CtxTreeNode
		{
			TreeNode* mNode = nullptr;

			char mRenameBuffer[128] = {};
			bool mRename = false;
			bool mDelete = false;

			void CheckCurItem(TreeNode* node);
			void PostGUI();
		};


		TArray<TObjectPtr<Entity>>		mSelectedEntities;
		TObjectPtr<Entity>				mRootEntity = nullptr;
		bool				mConsiderRootEntity = false;

		DFilterBox			mFilter;
		TreeNode*			mRootNode = nullptr;
		
		TreeNode*   mRightClickedNode = nullptr;
		CtxTreeNode mCtx;
		
		Entity* mEntityToDelete = nullptr;

		//the main function to select or unselect an entity
		void SelectEntity(Entity* pEntity, bool select);

		void RecursiveBuildTree(TreeNode* node);
		//clear the current tree rebuild new one, keeps the selected entities
		void ReBuildTreeFromRootEntity();
		void AttachEntity(Entity* root);

		WEntityBrowser()
			: WBase(true, "Entity Browser")
		{
		}

#if 0
		ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Example: Layout", p_open, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Close")) *p_open = false;
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// left
			static int selected = 0;
			ImGui::BeginChild("left pane", ImVec2(150, 0), true);
			for (int i = 0; i < 100; i++)
			{
				char label[128];
				sprintf(label, "MyObject %d", i);
				if (ImGui::Selectable(label, selected == i))
					selected = i;
			}
			ImGui::EndChild();
			ImGui::SameLine();

			// right
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("MyObject: %d", selected);
			ImGui::Separator();
			ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
			ImGui::EndChild();
			ImGui::BeginChild("buttons");
			if (ImGui::Button("Revert")) {}
			ImGui::SameLine();
			if (ImGui::Button("Save")) {}
			ImGui::EndChild();
			ImGui::EndGroup();
		}
		ImGui::End();
#endif
		virtual void DrawContents() override;
		//returns the number of entities in our tree
		size_t GetEntitiesCount() const;
		size_t GetNumSelectedEntities() const { return mSelectedEntities.Length(); }

		//////////////////////////////////////////////////////////////////////////
		bool CheckFilter(Entity* pEntity) const
		{
			if (mFilter.CheckFilter(pEntity->GetName())
				|| mFilter.CheckFilter(pEntity->GetClass()->GetName()))
			{
				return true;
			}
			else
			{
				auto child = pEntity->GetEntityFirstChild();
				while (child)
				{
					if (CheckFilter(child))
						return true;
					child = child->GetDownEntity();
				}
			}
			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		void ToggleEntitySelection(Entity* pEntrity)
		{
			SelectEntity(pEntrity, !pEntrity->IsSelected());
		}
		void ClearEntitySelection();
		void RecursiveDrawEntity(TreeNode* node);
		
#if 0
		void RecDrawEntity(Entity* pEntity)

		{
			if (nullptr == pEntity) return;
			if (!pEntity->IsAlive()) return;
			
			Entity* child = pEntity->GetEntityFirstChild();
			while (child)
			{
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick /*| ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0)*/;

				if (child->GetEntityFirstChild() == nullptr) //has no child?
					node_flags |= ImGuiTreeNodeFlags_Leaf;

				//check name and class filtering
				if(CheckEntityFilter(child))
				{
					bool node_open = ImGui::TreeNodeEx(child, node_flags, child->GetName());
					if (ImGui::IsItemClicked())
					{

					}

					mCtxMenu.Draw();


					ImGui::NextColumn();
					ImGui::Text(child->GetClass()->GetName().CStr());

					ImGui::NextColumn();
					if (node_open)
					{
						RecDrawEntity(child);
						ImGui::TreePop();
					}
				}

				child = child->GetDownEntity();
			}
		}
#endif
	};

	UEDITORBASE_API void TestIMGUIDRaw();
	inline void TestIMGUIDRaw2()
	{
		bool isOpen = true;

		ImGui::Begin("TestIMGUIDRaw2", &isOpen, ImVec2(300, 500), 0.8f, 0);

		ImGui::End();
	}

};