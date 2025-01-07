#include "RuntimePolyEditTool.h"
#include "RuntimeToolsFramework/RuntimeToolsFrameworkSubsystem.h"

#include "ToolBuilderUtil.h"

#define LOCTEXT_NAMESPACE "URuntimeEditMeshPolygonsTool"


USingleTargetWithSelectionTool* URuntimePolyEditToolBuilder::CreateNewTool(const FToolBuilderState& SceneState) const
{
	URuntimePolyEditTool* PolyEditTool = NewObject<URuntimePolyEditTool>(SceneState.ToolManager);
	PolyEditTool->LocalTargetWorld = SceneState.World;
	return PolyEditTool;
}




void URuntimePolyEditTool::Setup()
{
	UEditMeshPolygonsTool::Setup();

	// mirror properties we want to expose at runtime 
	RuntimeProperties = NewObject<URuntimePolyEditToolProperties>(this);

	AddToolPropertySource(RuntimeProperties);

	check(GEngine->WireframeMaterial != nullptr);
}


void URuntimePolyEditTool::BeginExtrudeAction()
{
	RequestAction(EEditMeshPolygonsToolActions::Extrude);
}

void URuntimePolyEditTool::BeginInsetAction()
{
	RequestAction(EEditMeshPolygonsToolActions::Inset);
}


void URuntimePolyEditTool::BeginOutsetAction()
{
	RequestAction(EEditMeshPolygonsToolActions::Outset);
}

void URuntimePolyEditTool::BeginCutFacesAction()
{
	RequestAction(EEditMeshPolygonsToolActions::CutFaces);
}

UWorld* URuntimePolyEditTool::GetWorld() const
{ 
	return this->LocalTargetWorld;
}


#undef LOCTEXT_NAMESPACE