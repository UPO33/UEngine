#include "EntityBrowser.h"

namespace UEditor
{
	UEDITORBASE_API void UEditor::TestIMGUIDRaw()
	{
		bool isOpen = true;

		ImGui::Begin("TestIMGUIDRaw", &isOpen, ImVec2(300, 500), 0.8f, 0);

		ImGui::End();

	}

	//////////////////////////////////////////////////////////////////////////
	void WEntityBrowser::SelectEntity(Entity* pEntity, bool select)
	{
		if (pEntity == nullptr) return;

		if (select)
		{
			mSelectedEntities.AddUnique(pEntity);
		}
		else
		{
			size_t indexFound = mSelectedEntities.Find(pEntity);
			if (indexFound != INVALID_INDEX)
				mSelectedEntities.RemoveShift(indexFound);
		}

		pEntity->SetSelected(select);
	}

	//////////////////////////////////////////////////////////////////////////
	void WEntityBrowser::RecursiveBuildTree(TreeNode* node)
	{
		Entity* child = node->mEntity->GetEntityFirstChild();
		while (child)
		{
			if (child->IsAlive())
			{
				//check name and class filtering
				if (CheckFilter(child))
				{
					TreeNode* newNode = node->AddChild(child);
					RecursiveBuildTree(newNode);
				}
			}

			child = child->GetDownEntity();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WEntityBrowser::ReBuildTreeFromRootEntity()
	{
		SafeDelete(mRootNode);

		if (mRootEntity.Get())
		{
			mRootNode = new TreeNode();
			mRootNode->mEntity = mRootEntity.Get();
				
			RecursiveBuildTree(mRootNode);
		}
	}

	void WEntityBrowser::AttachEntity(Entity* root)
	{
		mRootEntity = root;
		ReBuildTreeFromRootEntity();
	}
	void WEntityBrowser::DrawContents()
	{
		if (nullptr == mRootEntity.Get())
			ReBuildTreeFromRootEntity();

		//remove destroyed entities from selection
		mSelectedEntities.ConditionalRemove([](const EntityPtr& element) {
			return element.Get() == nullptr;
		});

		ImGui::Columns(1);
		ImGui::Separator();

		if (mFilter.Draw())
			ReBuildTreeFromRootEntity();

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Columns(2);
		RecursiveDrawEntity(mRootNode);
		ImGui::EndChild();
		ImGui::Columns(1);

		ImGui::Separator();
		//drawing footer
		ImGui::Text("Num Entities %d  Num Selected %d", GetEntitiesCount(), GetNumSelectedEntities());



		if (mEntityToDelete)
		{
			mEntityToDelete->DestroyEntity();
			mEntityToDelete = nullptr;
			this->ReBuildTreeFromRootEntity();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	size_t WEntityBrowser::GetEntitiesCount() const
	{
		size_t num = 0;

		if (mRootEntity)
		{
			mRootEntity->ForEachChild(true, false, [&num](const Object* pObj) {
				if(UCast<Entity>(pObj))
					num++;
			});

			if (mConsiderRootEntity)
				num++;
		}

		return num;
	}
	//////////////////////////////////////////////////////////////////////////
	void WEntityBrowser::ClearEntitySelection()
	{
		for (TObjectPtr<Entity> iterEntity : mSelectedEntities)
		{
			if (iterEntity.Get())
				iterEntity->SetSelected(false);
		}

		mSelectedEntities.RemoveAll();
	}
	//////////////////////////////////////////////////////////////////////////
	void WEntityBrowser::RecursiveDrawEntity(TreeNode* node)
	{
		if (node == nullptr) return;

		for (TreeNode* childNode : node->mChildren)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

			if (childNode->mChildren.Length() == 0) //has no child?
				node_flags |= ImGuiTreeNodeFlags_Leaf;

			if (childNode->mEntity->IsSelected())
				node_flags |= ImGuiTreeNodeFlags_Selected;
			
			ImGui::PushID(childNode);
			if (ImGui::SmallButton("D"))
			{
				mEntityToDelete = childNode->mEntity;
				ULOG_MESSAGE("");
			}
			ImGui::PopID();

			ImGui::SameLine();

			bool node_open = ImGui::TreeNodeEx(childNode->mEntity, node_flags, childNode->mEntity->GetName());
			if (ImGui::IsItemClicked())
			{
				bool wasSelected = childNode->mEntity->IsSelected();

				if (!ImGui::GetIO().KeyCtrl)
					ClearEntitySelection();

				SelectEntity(childNode->mEntity, !wasSelected);
			}
			

// 			ImGui::PushID(childNode);
// 			if (ImGui::BeginPopupContextItem("Ctx"))
// 			{
// 				if (ImGui::MenuItem("Delete"))
// 					mEntityToDelete = childNode->mEntity;
// 
// 				ImGui::MenuItem("Rename");
// 				ImGui::EndPopup();
// 			}
// 			ImGui::PopID();

			//go to class name column
			ImGui::NextColumn();
			ImGui::Text(childNode->mEntity->GetClass()->GetName().CStr());

			ImGui::NextColumn();
			if (node_open)
			{
				childNode->mEntity->mEdIsClosedNode = false;
				RecursiveDrawEntity(childNode);
				ImGui::TreePop();
			}
			else
			{
				childNode->mEntity->mEdIsClosedNode = true;
			}
		}
	}


	void WEntityBrowser::CtxTreeNode::CheckCurItem(TreeNode* node)
	{
		ImGui::PushID(node);
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("Ctx");
			mNode = node;
		}

		if (ImGui::BeginPopup("Ctx"))
		{
			mDelete = ImGui::MenuItem("Delete");
			mRename = ImGui::InputText("Rename", mRenameBuffer, sizeof(mRenameBuffer));
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void WEntityBrowser::CtxTreeNode::PostGUI()
	{
		if (mNode == nullptr) return;

		if (mDelete)
		{
			ULOG_MESSAGE("Destroy Entity");
		}
		else if (mRename)
		{
			mNode->mEntity->SetName(mRenameBuffer);
		}

		mDelete = mRename = false;
		mNode = nullptr;
	}

};