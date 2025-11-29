// Fill out your copyright notice in the Description page of Project Settings.

#include "Rooms/MasterRoom.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Containers/UnrealString.h" // Needed for TMap hash combineh"

AMasterRoom::AMasterRoom()
{
	PrimaryActorTick.bCanEverTick = false;
	
    // Developer Control: Create the GeometryRoot and make it the primary transform component.
	GeometryRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryRoot"));
	RootComponent = GeometryRoot;
    
	// CRITICAL: Room visuals are built client-side via the seed, not replicated.
	bReplicates = false;
}

// --- PUBLIC SETTERS (Callable by C++ ADungeonGenerator) ---

void AMasterRoom::SetRoomLocalSeed(int32 NewSeed)
{
    // The implementation: assign the input parameter to the private member variable.
    RoomLocalSeed = NewSeed;
}

void AMasterRoom::SetRoomConfig(URoomData* NewConfig)
{
    // Implementation: assign the input parameter to the private member variable.
    RoomConfig = NewConfig;
}

// --- PUBLIC GETTERS (Callable by BlueprintPure) ---

int32 AMasterRoom::GetRoomLocalSeed() const
{
    return RoomLocalSeed;
}

URoomData* AMasterRoom::GetRoomConfig() const
{
    return RoomConfig;
}

// --- CORE LOGIC IMPLEMENTATIONS ---

void AMasterRoom::ExecutePopulation()
{
    if (RoomConfig)
    {
        ClearPreviousMeshes(); 
        PopulateRoomVisuals_Internal();
    }
}

void AMasterRoom::ClearPreviousMeshes()
{
    // Destroys all dynamically added mesh components
    TArray<USceneComponent*> ChildMeshes;
    // GetChildrenComponents(bool bIncludeAllDescendants)
    GeometryRoot->GetChildrenComponents(true, ChildMeshes); 

    for (USceneComponent* Child : ChildMeshes)
    {
        // Safety check: Only destroy UStaticMeshComponents
        if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Child))
        {
            MeshComp->DestroyComponent();
        }
    }
}

void AMasterRoom::PopulateRoomVisuals_Internal()
{
if (!RoomConfig) return;

    // 1. Initialize the Deterministic Stream
    FRandomStream Stream(GetRoomLocalSeed());
    
    const float TileSize = RoomConfig->TileSize; 

    // --- Developer Control: Iterate the entire defined grid space ---
    for (int32 X = 0; X < RoomConfig->GridSizeX; ++X)
    {
        for (int32 Y = 0; Y < RoomConfig->GridSizeY; ++Y)
        {
            FGridCoords CurrentCoords;
            CurrentCoords.X = X;
            CurrentCoords.Y = Y;
            
            // Define variables for the slot data and mesh array source
            FMeshSlotData SlotData;
            TArray<UStaticMesh*> MeshSourceArray;
            bool bIsWall = false; // Used to track if we need wall logic

            // 2. Override Check: See if the designer explicitly defined this coordinate
            const FMeshSlotData* OverrideSlot = RoomConfig->GridLayout.Find(CurrentCoords);
            
            if (OverrideSlot)
            {
                // **OVERRIDE PATH:** Designer defined custom content/type
                SlotData = *OverrideSlot;
                MeshSourceArray = SlotData.PossibleMeshes;
                bIsWall = (SlotData.ElementType == EGridElementType::WallOrDoor);
            }
            else 
            {
                // **DEFAULT PATH:** Use the default floor logic
                // Ensure the default array isn't empty before proceeding
                if (RoomConfig->DefaultFloorMeshes.Num() == 0) continue; 
                
                // Construct default floor data
                SlotData.ElementType = EGridElementType::FloorOrProp;
                MeshSourceArray = RoomConfig->DefaultFloorMeshes;
            }
            
            // Skip if no meshes were provided (e.g., designer left an override slot empty)
            if (MeshSourceArray.Num() == 0) continue;

            // --- 3. Placement Math (Now uses SlotData.ElementType) ---
            FTransform RelativeTransform = FTransform::Identity;
            
            // Check the element type based on either the override or the default
            if (bIsWall || SlotData.ElementType == EGridElementType::WallOrDoor)
            {
                // TODO: Wall logic here (will need to be simplified to assume a boundary wall)
                // For now, it will use the placeholder logic from the previous step
                // ... (existing WallOrDoor logic) ...
            }
            else // Default or Overridden Floor/Prop (FloorOrProp)
            {
                // Floor logic (Center-Pivot Mesh) - UNCHANGED
                FVector Location(
                    CurrentCoords.X * TileSize + (TileSize / 2.0f), 
                    CurrentCoords.Y * TileSize + (TileSize / 2.0f), 
                    0.0f
                );
                RelativeTransform.SetLocation(Location);
            }

            // --- 4. Deterministic Selection and Spawning ---
            int32 MaxIndex = MeshSourceArray.Num() - 1;
            int32 SelectedIndex = Stream.RandRange(0, MaxIndex); 
            UStaticMesh* SelectedMesh = MeshSourceArray[SelectedIndex];

            if (SelectedMesh)
            {
                UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this);
                NewMesh->SetStaticMesh(SelectedMesh);
                NewMesh->RegisterComponent();
                NewMesh->AttachToComponent(GeometryRoot, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
                NewMesh->SetRelativeTransform(RelativeTransform); 
            }
        }
    }
}