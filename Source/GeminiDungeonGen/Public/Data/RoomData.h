// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomData.generated.h"

// Struct: Defines a single mesh element and its transform within the room.
USTRUCT(BlueprintType)
struct FMeshSlotData
{
	GENERATED_BODY()

	// The local transform relative to the room's origin (Developer controlled)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	FTransform RelativeTransform; 

	// The meshes a designer can choose from for this slot (Designer controlled)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	TArray<UStaticMesh*> PossibleMeshes; 
};

UCLASS()
class GEMINIDUNGEONGEN_API URoomData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// This array holds all the possible mesh slots for this specific room variant (e.g., "Stone Hallway")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	TArray<FMeshSlotData> GeometrySlots;
};
