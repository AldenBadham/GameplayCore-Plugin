using UnrealBuildTool;

public class InventorySystemCore : ModuleRules
{
	public InventorySystemCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"DeveloperSettings",
				"GameplayAbilities",
				"NetCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"GameplayAbilities",
				"GameplayTags",
				"Slate",
				"SlateCore"
			}
		);
	}
}