// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpectrelightPluginsTarget : TargetRules
{
	public SpectrelightPluginsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("SpectrelightPlugins");
	}
}