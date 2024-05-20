// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpectrelightPluginsTarget : TargetRules
{
	public SpectrelightPluginsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SpectrelightPlugins");
	}
}