#include "RuntimeModelingObjectsCreationAPI.h"
#include "InteractiveToolsContext.h"
#include "ContextObjectStore.h"
#include "MeshScene/RuntimeMeshSceneSubsystem.h"

using namespace UE::Geometry;


URuntimeModelingObjectsCreationAPI* URuntimeModelingObjectsCreationAPI::Register(UInteractiveToolsContext* ToolsContext)
{
	check(ToolsContext);

	if ( URuntimeModelingObjectsCreationAPI* CreationAPI = ToolsContext->ContextObjectStore->FindContext<URuntimeModelingObjectsCreationAPI>() )
	{
		return CreationAPI;
	}
	auto NewCreationAPI = NewObject<URuntimeModelingObjectsCreationAPI>(ToolsContext);
	ToolsContext->ContextObjectStore->AddContextObject(NewCreationAPI);
	return NewCreationAPI;
}


bool URuntimeModelingObjectsCreationAPI::Deregister(UInteractiveToolsContext* ToolsContext)
{
	check(ToolsContext);

	if (URuntimeModelingObjectsCreationAPI* FoundAPI = ToolsContext->ContextObjectStore->FindContext<URuntimeModelingObjectsCreationAPI>() )
	{
		ToolsContext->ContextObjectStore->RemoveContextObject(FoundAPI);
		return true;
	}
	return false;
}


FCreateMeshObjectResult URuntimeModelingObjectsCreationAPI::CreateMeshObject(const FCreateMeshObjectParams& CreateMeshParams)
{
	// create new SceneObject
	URuntimeMeshSceneObject* SceneObject = URuntimeMeshSceneSubsystem::Get()->CreateNewSceneObject();

	// initialize the mesh, depending on whether we were passed a FMeshDescription or a FDynamicMesh3
	if (CreateMeshParams.MeshType == ECreateMeshObjectSourceMeshType::MeshDescription)
	{
		SceneObject->Initialize(CreateMeshParams.TargetWorld, & CreateMeshParams.MeshDescription.GetValue() );
	}
	else
	{
		SceneObject->Initialize(CreateMeshParams.TargetWorld, & CreateMeshParams.DynamicMesh.GetValue() );
	}

	SceneObject->SetTransform(CreateMeshParams.Transform);

	// return the created Actor/Component
	FCreateMeshObjectResult Result;
	Result.ResultCode = ECreateModelingObjectResult::Ok;
	Result.NewActor = SceneObject->GetActor();
	Result.NewComponent = SceneObject->GetMeshComponent();
	return Result;
}


FCreateTextureObjectResult URuntimeModelingObjectsCreationAPI::CreateTextureObject(const FCreateTextureObjectParams& CreateTexParams)
{
	check(false);		// not supported!
	return FCreateTextureObjectResult{ ECreateModelingObjectResult::Failed_InvalidTexture };
}