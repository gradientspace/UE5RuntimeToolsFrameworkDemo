#include "RuntimeToolsFramework/RuntimeDynamicMeshComponentToolTarget.h"

#include "Components/DynamicMeshComponent.h"
#include "DynamicMeshToMeshDescription.h"
#include "MeshDescriptionToDynamicMesh.h"
#include "StaticMeshAttributes.h"
#include "Materials/Material.h"
#include "ModelingToolTargetUtil.h"
#include "RuntimeToolsFrameworkSubsystem.h"  // to emit change transaction


#define LOCTEXT_NAMESPACE "URuntimeDynamicMeshComponentToolTarget"

bool URuntimeDynamicMeshComponentToolTarget::IsValid() const
{
	if (!UPrimitiveComponentToolTarget::IsValid())
	{
		return false;
	}
	if (UDynamicMeshComponent* DynamicMeshComponent = Cast<UDynamicMeshComponent>(Component))
	{
		if (UDynamicMesh* DynamicMesh = DynamicMeshComponent->GetDynamicMesh())
		{
			return true;
		}
	}
	return false;
}


int32 URuntimeDynamicMeshComponentToolTarget::GetNumMaterials() const
{
	check(IsValid());
	return Component->GetNumMaterials();
}

UMaterialInterface* URuntimeDynamicMeshComponentToolTarget::GetMaterial(int32 MaterialIndex) const
{
	check(IsValid());
	return Component->GetMaterial(MaterialIndex);
}

void URuntimeDynamicMeshComponentToolTarget::GetMaterialSet(FComponentMaterialSet& MaterialSetOut, bool bPreferAssetMaterials) const
{
	check(IsValid());
	int32 NumMaterials = Component->GetNumMaterials();
	MaterialSetOut.Materials.SetNum(NumMaterials);
	for (int32 k = 0; k < NumMaterials; ++k)
	{
		MaterialSetOut.Materials[k] = Component->GetMaterial(k);
	}
}

bool URuntimeDynamicMeshComponentToolTarget::CommitMaterialSetUpdate(const FComponentMaterialSet& MaterialSet, bool bApplyToAsset)
{
	check(IsValid());

	UDynamicMeshComponent* DynamicMeshComponent = Cast<UDynamicMeshComponent>(Component);

	// todo: is it necessary to filter the material set like UDynamicMeshComponentToolTarget does?

	int32 NumMaterialsNeeded = Component->GetNumMaterials();
	int32 NumMaterialsGiven = MaterialSet.Materials.Num();
	for (int32 k = 0; k < NumMaterialsGiven; ++k)
	{
		DynamicMeshComponent->SetMaterial(k, MaterialSet.Materials[k]);
	}

	return true;
}


const FMeshDescription* URuntimeDynamicMeshComponentToolTarget::GetMeshDescription(const FGetMeshParameters& GetMeshParams)
{
	check(IsValid());

	if (bHaveCachedMeshDescription)
	{
		return CachedMeshDescription.Get();
	}

	UDynamicMeshComponent* DynamicMeshComponent = Cast<UDynamicMeshComponent>(Component);

	CachedMeshDescription = MakeUnique<FMeshDescription>(GetEmptyMeshDescription());

	FDynamicMeshToMeshDescription Converter;
	Converter.Convert(DynamicMeshComponent->GetMesh(), *CachedMeshDescription, true);

	bHaveCachedMeshDescription = true;
	return CachedMeshDescription.Get();
}

FMeshDescription URuntimeDynamicMeshComponentToolTarget::GetEmptyMeshDescription()
{
	// FStaticMeshAttributes are the attribute set required by a UStaticMesh, and USkeletalMesh supports
	// all the same attributes. Lots of code assumes that these attributes are available, to the point
	// where a FMeshDescription is basically not usable without them
	FMeshDescription MeshDescription;
	FStaticMeshAttributes Attributes(MeshDescription);
	Attributes.Register();
	return MeshDescription;
}

void URuntimeDynamicMeshComponentToolTarget::InvalidateCachedMeshDescription()
{
	if (bHaveCachedMeshDescription)
	{
		CachedMeshDescription = nullptr;
		bHaveCachedMeshDescription = false;
	}
}


void URuntimeDynamicMeshComponentToolTarget::CommitMeshDescription(const FCommitter& Committer, const FCommitMeshParameters& CommitMeshParams)
{
	if (ensure(IsValid()) == false) return;

	// we are going to replace FDynamicMesh3 inside the UDynamicMesh, we will pass to a FMeshChange so we can just steal it here
	UDynamicMesh* DynamicMesh = GetDynamicMeshContainer();
	TSharedPtr<FDynamicMesh3> CurrentMesh( DynamicMesh->ExtractMesh().Release() );

	// run the Committer function to store to a temporary MeshDescription
	FMeshDescription TempMeshDescription(*GetMeshDescription());
	FCommitterParams CommitterParams;
	CommitterParams.MeshDescriptionOut = &TempMeshDescription;
	Committer(CommitterParams);

	// convert to FDynamicMesh3
	FMeshDescriptionToDynamicMesh Converter;
	TSharedPtr<FDynamicMesh3> NewMesh = MakeShared<FDynamicMesh3>();
	NewMesh->EnableAttributes();
	Converter.Convert(CommitterParams.MeshDescriptionOut, *NewMesh, true);

	// update the UDynamicMesh
	DynamicMesh->EditMesh([&](FDynamicMesh3& EditMesh) { EditMesh = *NewMesh; });

	// emit the change
	CommitDynamicMeshChange(MakeUnique<FMeshReplacementChange>(CurrentMesh, NewMesh),
		LOCTEXT("RuntimeDynamicMeshChange", "MeshChange") );
}


UDynamicMesh* URuntimeDynamicMeshComponentToolTarget::GetDynamicMeshContainer()
{
	return Cast<UDynamicMeshComponent>(Component)->GetDynamicMesh();
}

bool URuntimeDynamicMeshComponentToolTarget::HasDynamicMeshComponent() const
{
	return true;
}

UDynamicMeshComponent* URuntimeDynamicMeshComponentToolTarget::GetDynamicMeshComponent()
{
	return Cast<UDynamicMeshComponent>(Component);
}


void URuntimeDynamicMeshComponentToolTarget::CommitDynamicMeshChange(TUniquePtr<FToolCommandChange> Change, const FText& ChangeMessage)
{
	URuntimeToolsFrameworkSubsystem::Get()->GetTransactionsAPI()->AppendChange(Component, MoveTemp(Change),
		LOCTEXT("UpdateMeshChange", "Update Mesh"));

	InvalidateCachedMeshDescription();
}

FDynamicMesh3 URuntimeDynamicMeshComponentToolTarget::GetDynamicMesh()
{
	UDynamicMesh* DynamicMesh = GetDynamicMeshContainer();
	FDynamicMesh3 Mesh;
	DynamicMesh->ProcessMesh([&](const FDynamicMesh3& ReadMesh) { Mesh = ReadMesh; });
	return Mesh;
}

void URuntimeDynamicMeshComponentToolTarget::CommitDynamicMesh(const FDynamicMesh3& UpdatedMesh, const FDynamicMeshCommitInfo& CommitInfo)
{
	UE::ToolTarget::Internal::CommitDynamicMeshViaIPersistentDynamicMeshSource(
		*this, UpdatedMesh, CommitInfo.bTopologyChanged);
}

UBodySetup* URuntimeDynamicMeshComponentToolTarget::GetBodySetup() const
{
	check(IsValid());
	return Cast<UDynamicMeshComponent>(Component)->GetBodySetup();
}


IInterface_CollisionDataProvider* URuntimeDynamicMeshComponentToolTarget::GetComplexCollisionProvider() const
{
	check(IsValid());
	return Cast<UDynamicMeshComponent>(Component);
}


bool URuntimeDynamicMeshComponentToolTargetFactory::CanBuildTarget(UObject* SourceObject, const FToolTargetTypeRequirements& Requirements) const
{
	UDynamicMeshComponent* Component = Cast<UDynamicMeshComponent>(SourceObject);
	return Component
		&& IsValidChecked(Component)
		&& (!Component->IsUnreachable())
		&& Component->IsValidLowLevel()
		&& Component->GetDynamicMesh()
		&& Requirements.AreSatisfiedBy(URuntimeDynamicMeshComponentToolTarget::StaticClass());
}

UToolTarget* URuntimeDynamicMeshComponentToolTargetFactory::BuildTarget(UObject* SourceObject, const FToolTargetTypeRequirements& Requirements)
{
	URuntimeDynamicMeshComponentToolTarget* Target = NewObject<URuntimeDynamicMeshComponentToolTarget>(this);
	Target->Component = Cast<UDynamicMeshComponent>(SourceObject);
	return Target;
}


#undef LOCTEXT_NAMESPACE