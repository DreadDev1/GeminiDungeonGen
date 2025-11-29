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
    
    // Get the developer-defined constant tile size
    const float TileSize = RoomConfig->TileSize; 

    // 2. Iterate through all (X, Y) grid slots defined by the designer
    for (const auto& Pair : RoomConfig->GridLayout)
    {
        const FGridCoords Coords = Pair.Key;         // (X, Y)
        const FMeshSlotData& Slot = Pair.Value;      // Mesh options

        if (Slot.PossibleMeshes.Num() == 0) continue;

        // --- DEVELOPER TASK: Calculate the deterministic RelativeTransform ---
        FTransform RelativeTransform = FTransform::Identity;
        
        // 3. SWITCH STATEMENT: Use the ElementType to select the correct placement math
        switch (Slot.ElementType)
        {
        case EGridElementType::FloorOrProp:
            {
                // This math is for Center-Pivot Meshes (like your floor tiles)
                FVector Location(
                    Coords.X * TileSize + (TileSize / 2.0f), 
                    Coords.Y * TileSize + (TileSize / 2.0f), 
                    0.0f
                );
                RelativeTransform.SetLocation(Location);
                break;
            }

        case EGridElementType::WallOrDoor:
            {
                // TODO: IMPLEMENT COMPLEX WALL PLACEMENT LOGIC HERE
                // This requires determining the tile's edge, calculating the half-size offset,
                // and applying rotation (0, 90, 180, or 270 degrees).
                
                // Placeholder: Use a basic tile-center for now until wall logic is built
                FVector Location(
                    Coords.X * TileSize + (TileSize / 2.0f), 
                    Coords.Y * TileSize + (TileSize / 2.0f), 
                    0.0f // Walls need a height offset, too!
                );
                RelativeTransform.SetLocation(Location);
                
                break;
            }
        }
        
        // 4. Deterministic Selection
        int32 MaxIndex = Slot.PossibleMeshes.Num() - 1;
        int32 SelectedIndex = Stream.RandRange(0, MaxIndex); 
        UStaticMesh* SelectedMesh = Slot.PossibleMeshes[SelectedIndex];

        if (SelectedMesh)
        {
            // 5. Spawn and Attach (Unchanged logic)
            UStaticMeshComponent* NewMesh = NewObject<UStaticMeshComponent>(this);
            NewMesh->SetStaticMesh(SelectedMesh);
            
            NewMesh->RegisterComponent();
            NewMesh->AttachToComponent(GeometryRoot, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
            
            // Apply the calculated transform (including location and rotation)
            NewMesh->SetRelativeTransform(RelativeTransform); // Use the calculated transform
        }
    }
}