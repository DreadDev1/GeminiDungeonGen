// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/RoomData.h"
#include "MasterRoom.generated.h"

UCLASS()
class GEMINIDUNGEONGEN_API AMasterRoom : public AActor
{
	GENERATED_BODY()

public:
	AMasterRoom();
    
	// Developer Hook: Called by the Generator after the seed/config is set.
	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void ExecutePopulation();
	
	// Designer/Blueprint friendly read access
	UFUNCTION(BlueprintPure, Category = "Dungeon|Config")
	int32 GetRoomLocalSeed() const; 
    
	UFUNCTION(BlueprintPure, Category = "Dungeon|Config")
	URoomData* GetRoomConfig() const;

	// New functions to write data (Callable by C++ ONLY, not BlueprintCallable)
	void SetRoomLocalSeed(int32 NewSeed); 
	void SetRoomConfig(URoomData* NewConfig);
	
protected:
	// C++ set as RootComponent for Developer Control over structure
	UPROPERTY(VisibleAnywhere, Category = "Dungeon Structure")
	USceneComponent* GeometryRoot;
    
private:
	
private: // <--- BEST PRACTICE: Lock down the sync data
	
	// These variables are set ONLY by the ADungeonGenerator C++ code.
	UPROPERTY() // Removed BlueprintReadOnly to enforce C++ Getters
	URoomData* RoomConfig;
    
	UPROPERTY() // Removed BlueprintReadOnly to enforce C++ Getters
	int32 RoomLocalSeed = 0;
	
	// Core C++ logic functions
	void PopulateRoomVisuals_Internal(); 
	void ClearPreviousMeshes();
};
