#pragma once

#include "TargetInterfaces/DynamicMeshCommitter.h"
#include "TargetInterfaces/DynamicMeshProvider.h"
#include "TargetInterfaces/MaterialProvider.h"
#include "TargetInterfaces/MeshDescriptionCommitter.h"
#include "TargetInterfaces/MeshDescriptionProvider.h"
#include "TargetInterfaces/DynamicMeshSource.h"
#include "TargetInterfaces/PhysicsDataSource.h"
#include "ToolTargets/PrimitiveComponentToolTarget.h"
#include "RuntimeDynamicMeshComponentToolTarget.generated.h"

class UDynamicMesh;

/**
 * URuntimeDynamicMeshComponentToolTarget is a UToolTarget implementation suitable for
 * UDynamicMeshComponent, which is the Component Type that ultimately backs a URuntimeMeshSceneObject
 * (via an ADynamicSDMCActor). 
 * 
 * The Engine provides UPrimitiveComponentToolTarget, which we subclass to re-use various
 * common UPrimitiveComponent functionality. The Engine actually also provides UDynamicMeshComponentToolTarget,
 * but it is Editor-Only and so cannot be used at Runtime. This class mirrors that
 * implementation heavily, although some things are specific to the Runtime system
 * (ie no transactions, changes are emitted via RuntimeToolsFrameworkSubsystem)
 * 
 */
UCLASS(Transient)
class RUNTIMETOOLSSYSTEM_API URuntimeDynamicMeshComponentToolTarget :
	public UPrimitiveComponentToolTarget,
	public IMeshDescriptionCommitter, public IMeshDescriptionProvider,
	public IDynamicMeshProvider, public IDynamicMeshCommitter,
	public IMaterialProvider,
	public IPersistentDynamicMeshSource,
	public IPhysicsDataSource
{
	GENERATED_BODY()
public:

	// override UPrimitiveComponentToolTarget to also check if underlying UDynamicMesh is valid
	virtual bool IsValid() const override;

	// IMeshDescriptionProvider
	const FMeshDescription* GetMeshDescription(const FGetMeshParameters& GetMeshParams = FGetMeshParameters()) override;
	virtual FMeshDescription GetEmptyMeshDescription() override;

	// IMeshDescriptionCommitter
	virtual void CommitMeshDescription(const FCommitter& Committer, const FCommitMeshParameters& CommitMeshParams = FCommitMeshParameters()) override;
	using IMeshDescriptionCommitter::CommitMeshDescription;

	// IDynamicMeshProvider
	virtual UE::Geometry::FDynamicMesh3 GetDynamicMesh() override;

	// IDynamicMeshCommitter
	virtual void CommitDynamicMesh(const UE::Geometry::FDynamicMesh3& Mesh, const FDynamicMeshCommitInfo& CommitInfo) override;
	using IDynamicMeshCommitter::CommitDynamicMesh;

	// IMaterialProvider
	int32 GetNumMaterials() const override;
	UMaterialInterface* GetMaterial(int32 MaterialIndex) const override;
	void GetMaterialSet(FComponentMaterialSet& MaterialSetOut, bool bPreferAssetMaterials) const override;
	virtual bool CommitMaterialSetUpdate(const FComponentMaterialSet& MaterialSet, bool bApplyToAsset) override;

	// IPersistentDynamicMeshSource
	virtual UDynamicMesh* GetDynamicMeshContainer() override;
	virtual void CommitDynamicMeshChange(TUniquePtr<FToolCommandChange> Change, const FText& ChangeMessage) override;
	virtual bool HasDynamicMeshComponent() const override;
	virtual UDynamicMeshComponent* GetDynamicMeshComponent() override;

	// IPhysicsDataSource
	virtual UBodySetup* GetBodySetup() const override;
	virtual IInterface_CollisionDataProvider* GetComplexCollisionProvider() const override;

protected:
	// In many cases it is necessary to convert the DynamicMeshComponent's UDynamicMesh/FDynamicMesh3 to a FMeshDescription.
	// We cache this conversion in case it can be re-used.
	TUniquePtr<FMeshDescription> CachedMeshDescription;
	bool bHaveCachedMeshDescription = false;
	void InvalidateCachedMeshDescription();

protected:
	friend class URuntimeDynamicMeshComponentToolTargetFactory;
};


UCLASS(Transient)
class RUNTIMETOOLSSYSTEM_API URuntimeDynamicMeshComponentToolTargetFactory : public UToolTargetFactory
{
	GENERATED_BODY()
public:
	virtual bool CanBuildTarget(UObject* SourceObject, const FToolTargetTypeRequirements& TargetTypeInfo) const override;
	virtual UToolTarget* BuildTarget(UObject* SourceObject, const FToolTargetTypeRequirements& TargetTypeInfo) override;
};