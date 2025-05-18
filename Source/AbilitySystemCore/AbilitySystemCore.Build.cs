using UnrealBuildTool;

public class AbilitySystemCore : ModuleRules
{
	public AbilitySystemCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EnhancedInput"
			}
		);
	}
}