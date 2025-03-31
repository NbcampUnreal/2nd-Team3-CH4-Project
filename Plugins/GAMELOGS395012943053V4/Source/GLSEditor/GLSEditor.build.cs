// Copyright (c) 2024 Fergius Engineering.

using System.IO;
using UnrealBuildTool;

public class GLSEditor : ModuleRules
{
    public GLSEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        IWYUSupport = IWYUSupport.Full;

        PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "GLS",
                "Slate",
                "SlateCore",
                "Blutility",
                "UMGEditor",
                "InputCore",
                "EditorStyle",
                "UMG",
                "Projects"
            });
    }
}