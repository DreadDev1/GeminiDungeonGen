
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

UCLASS()
class GEMINIDUNGEONGEN_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	ADungeonGenerator();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
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
