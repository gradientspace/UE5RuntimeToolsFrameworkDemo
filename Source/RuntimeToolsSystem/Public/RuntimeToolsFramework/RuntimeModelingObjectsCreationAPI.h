#pragma once

#include "CoreMinimal.h"
#include "ModelingObjectsCreationAPI.h"
#include "RuntimeModelingObjectsCreationAPI.generated.h"

class UInteractiveToolsContext;

/**
 * Implementation of UModelingObjectsCreationAPI, which UE Modeling Tools use to 
 * emit new "Mesh Objects" (eg StaticMeshAsset+Actor, DynamicMeshActor, AVolume).
 * In this Runtime system we will only emit URuntimeMeshSceneObject.
 * 
 * The UE Modeling Tools find an available UModelingObjectsCreationAPI by searching
 * in the ContextStore of the their ToolsContext. So, use the 
 * URuntimeModelingObjectsCreationAPI::Register(ToolsContext) function to create
 * a new instance and set it in the ContextStore (and Deregister to remove it)
 * 
 * This is similar to UEditorModelingObjectsCreationAPI, which is what Modeling Mode
 * in the UE Editor makes available to the Tools.
 * 
 * CreateTextureObject currently not supported.
 */
UCLASS()
class RUNTIMETOOLSSYSTEM_API URuntimeModelingObjectsCreationAPI : public UModelingObjectsCreationAPI
{
	GENERATED_BODY()
public:

	virtual FCreateMeshObjectResult CreateMeshObject(const FCreateMeshObjectParams& CreateMeshParams) override;
	virtual FCreateTextureObjectResult CreateTextureObject(const FCreateTextureObjectParams& CreateTexParams) override;

	// Call this to provide an instance of URuntimeModelingObjectsCreationAPI to UE Modeling Tools
	static URuntimeModelingObjectsCreationAPI* Register(UInteractiveToolsContext* ToolsContext);
	// Call this to clean up the Register'd instance
	static bool Deregister(UInteractiveToolsContext* ToolsContext);
};