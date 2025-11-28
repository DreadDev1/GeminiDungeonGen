// Fill out your copyright notice in the Description page of Project Settings.


#include "Rooms/MasterRoom.h"
#include "Components/SceneComponent.h"

AMasterRoom::AMasterRoom()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Developer Control: Set GeometryRoot as the physical anchor.
	GeometryRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryRoot"));
	RootComponent = GeometryRoot;
    
	// CRITICAL: Room visuals are built client-side via the seed, not replicated.
	bReplicates = false;
}

void AMasterRoom::ExecutePopulation()
{
    if (RoomConfig)
    {
        ClearPreviousMeshes(); 
        PopulateRoomVisuals_Internal();
    }
}

void AMasterRoom::SetRoomLocalSeed(int32 NewSeed)
{
    // The implementation: assign the input parameter to the private member variable.
    RoomLocalSeed = NewSeed;
}

void AMasterRoom::SetRoomConfig(URoomData* NewConfig)
{
    // The implementation: assign the input parameter to the private member variable.
    // Note: I am assuming URoomData is URoomDataAsset based on earlier context.
    RoomConfig = NewConfig;
}

void AMasterRoom::ClearPreviousMeshes()
{
    // Destroys all dynamically added mesh components
    TArray<USceneComponent*> ChildMeshes;
    GeometryRoot->GetChildrenComponents(true, ChildMeshes); 

    for (USceneComponent* Child : ChildMeshes)
    {
        if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Child))
        {
            MeshComp->DestroyComponent();
        }
    }
}

void AMasterRoom::PopulateRoomVisuals_Internal()
{
    if (!RoomConfig) return;

    // 1. Initialize the Deterministic Stream using the synchronized seed
    FRandomStream Stream(RoomLocalSeed);

    for (const FMeshSlotData& Slot : RoomConfig->GeometrySlots)
    {
        if (Slot.PossibleMeshes.Num() == 0) continue;

        // 2. Deterministic Selection
        int32 MaxIndex = Slot.PossibleMeshes.Num() - 1;
        
        // Generates the same index on the server and all clients
        int32 SelectedIndex = Stream.RandRange(0, MaxIndex); 
        
        UStaticMesh* SelectedMesh = Slot.PossibleMeshes[SelectedIndex];

        if (SelectedMesh)
        {
            // 3. Spawn the Static Mesh Component (The Visual)
            UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this);
            NewMesh->SetStaticMesh(SelectedMesh);
            
            // Register and Attach
            NewMesh->RegisterComponent();
            NewMesh->AttachToComponent(GeometryRoot, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
            
            // Apply the transform dictated by the developer's FMeshSlotData
            NewMesh->SetRelativeTransform(Slot.RelativeTransform);
        }
    }
}