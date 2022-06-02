#include "RuntimeDrawPolygonTool.h"
#include "RuntimeToolsFramework/RuntimeToolsFrameworkSubsystem.h"
#include "MeshScene/RuntimeMeshSceneSubsystem.h"

#define LOCTEXT_NAMESPACE "URuntimeDrawPolygonTool"

UInteractiveTool* URuntimeDrawPolygonToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	URuntimeDrawPolygonTool* NewTool = NewObject<URuntimeDrawPolygonTool>(SceneState.ToolManager);
	NewTool->SetWorld(SceneState.World);
	return NewTool;
}



void URuntimeDrawPolygonTool::Setup()
{
	UDrawPolygonTool::Setup();

	// initialize to drawing material
	this->MaterialProperties->Material = URuntimeMeshSceneSubsystem::Get()->StandardMaterial;

	// mirror properties we want to expose at runtime 
	RuntimeProperties = NewObject<URuntimeDrawPolygonToolProperties>(this);

	RuntimeProperties->SelectedPolygonType = (int)PolygonProperties->PolygonDrawMode;
	RuntimeProperties->WatchProperty(RuntimeProperties->SelectedPolygonType,
		[this](int NewType) { PolygonProperties->PolygonDrawMode = (EDrawPolygonDrawMode)NewType; });

	AddToolPropertySource(RuntimeProperties);
}


#undef LOCTEXT_NAMESPACE