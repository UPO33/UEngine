#include "WorkSpace.h"
#include "../IMGUI/imgui.h"

WorkSpaceContext* gWorkSpace;

void WorkSpaceContext::AddWorkSpace(WorkSpacePtr ws, bool makeCurrent)
{
	auto index = mAllWorkSpaces.Add(ws);

	makeCurrent = index == 0 ? true : makeCurrent; // is the first work space ? so make it current

	if (makeCurrent)
		mCurrentWorkSpaceIndex = index;
}

void WorkSpaceContext::Update()
{
	if (mAllWorkSpaces.Length())
	{
		mAllWorkSpaces[mCurrentWorkSpaceIndex]->Update();
	}

	ImGui::Begin("WorkSpaces");
	for (size_t iWorkSpace = 0; iWorkSpace < mAllWorkSpaces.Length(); iWorkSpace++)
	{
		if (ImGui::Button(mAllWorkSpaces[iWorkSpace]->mName.CStr()))
			mCurrentWorkSpaceIndex = iWorkSpace;
	}
	ImGui::End();

	if (ImGui::IsKeyPressed(VK_TAB))
		mCurrentWorkSpaceIndex = (mCurrentWorkSpaceIndex + 1) % mAllWorkSpaces.Length();
}


void WorkSpaceDebug::Update()
{
	// Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
	ImGui::SetNextWindowPos(ImVec2(32, 32), ImGuiSetCond_FirstUseEver);
	ImGui::ShowTestWindow();

	mTest.Draw();
}

void WorkSpace::Update()
{
	for (const TSPtr<WBase>& window : mWindows)
		window->Draw();
}

struct TestStruct0
{
	UCLASS(TestStruct0)

	float mFloat0;
	float mFloat1;
	bool mBool0;
};

//@Note if a property is not root property of an object

class PRPTestObject : public Object
{
	UCLASS(PRPTestObject, Object)

	float mFloat0;
	float mFloat1;
	bool mBool0;
	TestStruct0 mTestStruct0;
	TArray<float> mFloatArray;
};

class PRPTestObjectDerived : public PRPTestObject
{
	UCLASS(PRPTestObjectDerived, PRPTestObject)


	float mSpeed = 1;
	float mAngle = 30;
	TestStruct0 mTestStruct1;
};

UCLASS_BEGIN_IMPL(PRPTestObjectDerived)
UPROPERTY(mSpeed, AttrCategory("Movement"))
UPROPERTY(mAngle, AttrCategory("Movement"))
UPROPERTY(mTestStruct1)
UCLASS_END_IMPL(PRPTestObjectDerived)

UCLASS_BEGIN_IMPL(PRPTestObject)
UPROPERTY(mFloat0)
UPROPERTY(mFloat1)
UPROPERTY(mBool0)
UPROPERTY(mTestStruct0)
UPROPERTY(mFloatArray)
UCLASS_END_IMPL(PRPTestObject)

UCLASS_BEGIN_IMPL(TestStruct0)
UPROPERTY(mFloat0)
UPROPERTY(mFloat1)
UPROPERTY(mBool0)
UCLASS_END_IMPL(TestStruct0)
