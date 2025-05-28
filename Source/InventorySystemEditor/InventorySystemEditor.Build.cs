using UnrealBuildTool;

public class InventorySystemEditor : ModuleRules
{
	public InventorySystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"FunctionalTesting",
				"InventorySystemCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AutomationController",
				"AutomationTest",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"Slate",
				"SlateCore",
				"UnrealEd"
			}
		);
	}
}