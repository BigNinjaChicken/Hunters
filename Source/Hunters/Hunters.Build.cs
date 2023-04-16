// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Hunters : ModuleRules
{
	public Hunters(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystemEOS" , "OnlineSubsystem", "OnlineSubsystemUtils"});
	}
}
