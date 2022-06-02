#include "RuntimeRemeshMeshTool.h"
#include "RuntimeToolsFramework/RuntimeToolsFrameworkSubsystem.h"

#include "ToolBuilderUtil.h"

#define LOCTEXT_NAMESPACE "URuntimeRemeshMeshTool"

UMultiSelectionMeshEditingTool* URuntimeRemeshMeshToolBuilder::CreateNewTool(const FToolBuilderState& SceneState) const
{
	return NewObject<URuntimeRemeshMeshTool>(SceneState.ToolManager);
}


void URuntimeRemeshMeshTool::Setup()
{
	URemeshMeshTool::Setup();

	// disable wireframe because it crashes at runtime
	//this->BasicProperties->bShowWireframe = false;

	// mirror properties we want to expose at runtime 
	RuntimeProperties = NewObject<URuntimeRemeshMeshToolProperties>(this);

	RuntimeProperties->bDiscardAttributes = BasicProperties->bDiscardAttributes;
	RuntimeProperties->WatchProperty(RuntimeProperties->bDiscardAttributes,
		[this](bool bNewValue) { BasicProperties->bDiscardAttributes = bNewValue; BasicProperties->bPreserveSharpEdges = !bNewValue; Preview->InvalidateResult(); });

	RuntimeProperties->TargetTriangleCount = BasicProperties->TargetTriangleCount;
	RuntimeProperties->WatchProperty(RuntimeProperties->TargetTriangleCount,
		[this](int NewValue) { BasicProperties->TargetTriangleCount = NewValue; Preview->InvalidateResult(); });

	AddToolPropertySource(RuntimeProperties);
}


#undef LOCTEXT_NAMESPACE