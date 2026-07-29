using UnrealBuildTool;

public class SingularisEquipment : ModuleRules
{
	public SingularisEquipment(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			[
				"Core",
				"CoreUObject",
				"Engine",
				"NetCore",

				"InputCore",
				"EnhancedInput",

				"GameplayTags"
			]
		);
	}
}