// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpectrelightPluginsEditorTarget : TargetRules
{
	public SpectrelightPluginsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SpectrelightPlugins");
	}
}