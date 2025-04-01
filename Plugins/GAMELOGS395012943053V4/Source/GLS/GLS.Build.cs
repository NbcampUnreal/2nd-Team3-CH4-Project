// Copyright (c) 2024 Fergius Engineering.

using UnrealBuildTool;
using System.IO;
using System.Collections.Generic;
using EpicGames.Core;

public class GLS : ModuleRules
{
    //TODO: link to docs "how to disable game logs for distribution build"
    [CommandLine("-gls_disable")]
    public bool GameLogsDisabled = false;

    public GLS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        IWYUSupport = IWYUSupport.Full;

        PublicIncludePaths.AddRange(GetRecursiveIncludePaths(ModuleDirectory + "/Public"));
        PrivateIncludePaths.AddRange(GetRecursiveIncludePaths(ModuleDirectory + "/Private"));

        if (Target.Type == TargetType.Server || GameLogsDisabled)
        {
            PublicDefinitions.Add("GLS_ENABLED=0");
            //System.Console.WriteLine("Game Logs System (GLS) disabled: log collection and overlay will be disabled.");
        }
        else
        {
            PublicDefinitions.Add("GLS_ENABLED=1");
            //System.Console.WriteLine("Game Logs System (GLS) enabled but the behavior can be override based on the settings in UGLSSettings: Log collection and overlay will be available to enable.");
        }

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "ApplicationCore",
                "UMG",
                "Slate",
                "SlateCore",
                "Json",
                "JsonUtilities",
                "Engine",
                "InputCore",
                "HTTP",
                "Projects"
            }
        );

        if (Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd"
            });
        }
    }

    public List<string> GetRecursiveIncludePaths(string Root)
    {
        List<string> Paths = new List<string>();

        string[] Folders = Directory.GetDirectories(Root);
        foreach (string Folder in Folders)
        {
            Paths.Add(Folder);
            Paths.AddRange(GetRecursiveIncludePaths(Folder));
        }

        return Paths;
    }
}
