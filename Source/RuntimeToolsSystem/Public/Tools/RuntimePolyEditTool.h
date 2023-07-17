#pragma once

#include "EditMeshPolygonsTool.h"
#include "RuntimePolyEditTool.generated.h"

UCLASS()
class RUNTIMETOOLSSYSTEM_API URuntimePolyEditToolBuilder : public UEditMeshPolygonsToolBuilder
{
	GENERATED_BODY()
public:
	virtual USingleTargetWithSelectionTool* CreateNewTool(const FToolBuilderState& SceneState) const override;
};


UCLASS(BlueprintType)
class RUNTIMETOOLSSYSTEM_API URuntimePolyEditToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
public:

};


UCLASS(BlueprintType)
class RUNTIMETOOLSSYSTEM_API URuntimePolyEditTool : public UEditMeshPolygonsTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;

	UPROPERTY(BlueprintReadOnly)
	URuntimePolyEditToolProperties* RuntimeProperties;



	UFUNCTION(BlueprintCallable)
	void BeginExtrudeAction();

	UFUNCTION(BlueprintCallable)
	void BeginInsetAction();

	UFUNCTION(BlueprintCallable)
	void BeginOutsetAction();

	UFUNCTION(BlueprintCallable)
	void BeginCutFacesAction();


	// Multiple base classes have a UWorld* TargetWorld, causes scoping problems...so we add a third that we can rely on...
	UPROPERTY()
	TObjectPtr<UWorld> LocalTargetWorld = nullptr; 

	// Override this to work around a bug in PolyEditActivityUtil::CreatePolyEditPreviewMesh() that calls GetWorld instead of GetTargetWorld()
	virtual UWorld* GetWorld() const;
};