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
				"InventorySystemCore", 
				"GameplayCore"
			}
		);
	}
}