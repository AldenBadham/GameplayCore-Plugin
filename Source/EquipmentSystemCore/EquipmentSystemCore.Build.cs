using UnrealBuildTool;

public class EquipmentSystemCore : ModuleRules
{
	public EquipmentSystemCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"AbilitySystemCore",
				"Core",
				"DeveloperSettings",
				"GameplayAbilities",
				"InventorySystemCore",
				"NetCore"
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"AbilitySystemCore",
				"CoreUObject",
				"Engine",
				"GameplayAbilities",
				"GameplayTags",
				"InventorySystemCore"
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}