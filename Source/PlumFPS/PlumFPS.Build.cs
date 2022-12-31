// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlumFPS : ModuleRules
{
	public PlumFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
