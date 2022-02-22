// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TutorialProjectEditorTarget : TargetRules
{
	public TutorialProjectEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "TutorialProject" , "AccelByteUe4Sdk", "OnlineSubsystemSteam", "WebBrowserWidget" });
	}
}
