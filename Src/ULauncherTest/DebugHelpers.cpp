#include "Base.h"
#include "../Core/Log.h"
#include "../Core/Meta.h"

void UPrintObjectClassHierarchy(Object* root, int indent /*= 0*/)
{
	if (indent == 0)
		ULOG_MESSAGE("==============================================");

	ULOG_MESSAGE("% Object [%] Class [%]", LogIndent(indent), root->GetName(), root->GetClass()->GetName());
	root->DebugPrint();

	root->GetObjectDebugStr();
	Object* child = root->GetObjectFirstChild();
	while (child)
	{
		UPrintObjectClassHierarchy(child, indent + 1);
		child = child->GetObjectDown();
	}

	if (indent == 0)
		ULOG_MESSAGE("==============================================");
}

