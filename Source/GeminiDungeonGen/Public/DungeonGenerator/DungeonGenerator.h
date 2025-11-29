
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

class URoomData;

UCLASS()
class GEMINIDUNGEONGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGenerator();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	
protected:
	// 1. NEW: The Blueprint class to spawn (set in BP_DungeonGenerator)
	UPROPERTY(EditAnywhere, Category = "Configuration")
	TSubclassOf<class AMasterRoom> RoomClassToSpawn; 

	// 2. NEW: The specific room data asset to load for testing
	// Note: This needs to be a hard reference to load the asset before runtime.
	UPROPERTY(EditAnywhere, Category = "Configuration")
	URoomData* DefaultRoomData;
	
	// SERVER ENTRY POINT: Protected for encapsulation.
	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void StartGeneration(int32 NewSeed); 
    
	// The single seed replicated from the server to all clients
	UPROPERTY(ReplicatedUsing = OnRep_DungeonSeed)
	int32 DungeonSeed = 0;

	// Called on clients when the seed is received (and on server after setting)
	UFUNCTION()
	void OnRep_DungeonSeed();

	// SHARED LOGIC: The core deterministic generation function
	void GenerateDungeon();
};
