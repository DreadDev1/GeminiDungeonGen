#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RoomData.generated.h"

// --- ENUM: Developer Control over Mesh Type/Placement Logic ---
UENUM(BlueprintType)
enum class EGridElementType : uint8
{
	// These elements are placed at the center of the grid tile.
	FloorOrProp UMETA(DisplayName="Floor/Prop (Center Pivot)"),

	// These elements are placed along the edge of the tile and require rotation.
	WallOrDoor UMETA(DisplayName="Wall/Door (Edge Aligned)"),
};

// 1. Key Structure: TMap Key (Coordinates) ---
USTRUCT(BlueprintType)
struct FGridCoords
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 Y = 0;
    
	// Required operator for TMap key comparison/hashing
	bool operator==(const FGridCoords& Other) const
	{
		return X == Other.X && Y == Other.Y;
	}

	friend uint32 GetTypeHash(const FGridCoords& Other)
	{
		return HashCombine(GetTypeHash(Other.X), GetTypeHash(Other.Y));
	}
};

// 2. Value Structure: The TMap Value (Designer Content) ---
USTRUCT(BlueprintType)
struct FMeshSlotData
{
	GENERATED_BODY()

	// Developer Control: Which C++ placement logic to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
	EGridElementType ElementType = EGridElementType::FloorOrProp;

	// Designer Control: The visual choices
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Geometry")
	TArray<UStaticMesh*> PossibleMeshes; 
};

UCLASS()
class GEMINIDUNGEONGEN_API URoomData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Developer Constant: Defines the size of one grid cell (e.g., 400x400 cm)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Configuration")
	float TileSize = 400.0f; 

	// The entire room layout: (X, Y) map to mesh options
	// Designers will fill this TMap with coordinates and meshes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Configuration")
	TMap<FGridCoords, FMeshSlotData> GridLayout;
};
