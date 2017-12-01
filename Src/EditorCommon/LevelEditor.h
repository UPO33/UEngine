#pragma once


#include "D3DWidget.h"

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API LevelEditorViewportWidget : public ViewportWidget
	{
	public:
		LevelEditorViewportWidget(QWidget* parent = nullptr) : ViewportWidget(parent)
		{

		}
		void Tick();
		void Render();
	};
}