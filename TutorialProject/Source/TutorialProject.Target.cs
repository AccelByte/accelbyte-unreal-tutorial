// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TutorialProjectTarget : TargetRules
{
	public TutorialProjectTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "TutorialProject", "AccelByteUe4Sdk", "OnlineSubsystemSteam", "WebBrowserWidget" });
	}
}
