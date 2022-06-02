// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RuntimeToolsSystem : ModuleRules
{
	public RuntimeToolsSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"RenderCore",
			"InteractiveToolsFramework",
			"MeshDescription",
			"StaticMeshDescription",
			"GeometryCore",
			"DynamicMesh",
			"GeometryFramework",
			"MeshConversion",
			"ModelingComponents",
			"MeshModelingTools",
			"MeshModelingToolsExp",
			"RuntimeGeometryUtils"
		});

		 PrivateDependencyModuleNames.AddRange(new string[] { 
			 "Slate", 
			 "SlateCore" 
		 });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
