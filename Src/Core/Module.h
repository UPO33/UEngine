#pragma once

#include "Base.h"
#include "Name.h"

namespace UCore
{
	class IModule
	{
	public:
		IModule(){}
		virtual ~IModule(){}

		virtual void OnStartup(){}
		virtual void OnShutDown(){}
	};


	class UCORE_API ModuleSys
	{
	public:
		static ModuleSys& Get();

		IModule*	GetModule(Name moduleNme);
		IModule*	LoadModule(Name moduleName);
		IModule*	ReloadModule(Name modlueName);
	};


	inline ModuleSys& GModuleSys() { return ModuleSys::Get(); }
};