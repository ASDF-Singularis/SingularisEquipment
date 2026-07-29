#pragma once

#include <Modules/ModuleManager.h>

class FSingularisEquipmentModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
