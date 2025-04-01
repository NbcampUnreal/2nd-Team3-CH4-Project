// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WeaponMaster : ModuleRules
{
	public WeaponMaster(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
