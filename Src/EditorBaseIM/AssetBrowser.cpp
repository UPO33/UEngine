#include "AssetBrowser.h"

namespace UEditor
{

	WAssetBrowser::WAssetBrowser() : WBase(true, "AssetBrowser")
	{

	}

	void WAssetBrowser::DrawContents()
	{
		ImGui::Separator();
		ImGui::Columns(2);
		mFolderFilter.Draw();
		ImGui::BeginChild("Left", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		DrawFolders();
		ImGui::EndChild();

		ImGui::NextColumn();
		mAssetsFilter.Draw(); 
		ImGui::SameLine();
		ImGui::SliderInt("Column", &mAssetNumColumn, 1, 6);

		ImGui::BeginChild("Right", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), true, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::PushItemWidth(64);
		DrawAssets();
		ImGui::PopItemWidth();
		ImGui::EndChild();
		ImGui::NextColumn();
		

		ImGui::Columns(1);

		ImGui::Separator();
		//drawing footer
		ImGui::Text("Selected Asset X");


	}

	void WAssetBrowser::DrawFolders()
	{
		for (size_t i = 0; i < 128; i++)
			ImGui::Selectable("Folder");
	}

	void WAssetBrowser::DrawAssets()
	{
#if  ImGui::BeginChild("Sub2", ImVec2(0,300), true);
		ImGui::Text("With border");
		ImGui::Columns(2);
		for (int i = 0; i < 100; i++)
		{
			if (i == 50)
				ImGui::NextColumn();
			char buf[32];
			sprintf(buf, "%08x", i * 5731);
			ImGui::Button(buf, ImVec2(-1.0f, 0.0f));
		}
		ImGui::EndChild();
#endif
		
		ImVec2 v = ImGui::GetContentRegionAvail();
		ImGui::Columns(v.x / 80, "Columns", false);
		for (size_t i = 0; i < 128; i++)
		{
			//ImVec2 v = ImGui::GetContentRegionAvail();
			ImGui::Button("asset", ImVec2(80, 80)); 
			//ImGui::SameLine();
			ImGui::Text("Asset %d", i);
			ImGui::NextColumn();
		}

// 		if (ImGui::TreeNode("a"))
// 		{
// 			if (ImGui::TreeNode("b"))
// 			{
// 				ImGui::TreePop();
// 			}
// 			ImGui::TreePop();
// 		}
	}

};

