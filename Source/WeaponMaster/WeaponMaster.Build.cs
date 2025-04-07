// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WeaponMaster : ModuleRules
{
	public WeaponMaster(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"UMG", 
			"Slate", 
			"SlateCore",
			"SST",
			"Niagara",
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"OnlineSubsystemUtils",
		});
		
		PrivateIncludePaths.Add("WeaponMaster");
    }
}
