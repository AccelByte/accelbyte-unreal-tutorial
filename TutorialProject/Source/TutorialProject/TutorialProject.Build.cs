// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TutorialProject : ModuleRules
{
	public TutorialProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","AccelByteUe4Sdk", "UMG", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Json", "OnlineSubsystem", "OnlineSubsystemSteam", "Steamworks", "http", "WebBrowserWidget" });
	}
}
