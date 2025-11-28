// YourProjectName/Private/DungeonGenerator.cpp

#include "DungeonGenerator/DungeonGenerator.h"
#include "Net/UnrealNetwork.h" 
#include "Data/RoomData.h"
#include "Rooms/MasterRoom.h"

ADungeonGenerator::ADungeonGenerator()
{
	bReplicates = true; 
}

void ADungeonGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADungeonGenerator, DungeonSeed);
}

// --- Server-Side Initiation ---
void ADungeonGenerator::StartGeneration(int32 NewSeed)
{
	if (HasAuthority()) 
	{
		DungeonSeed = NewSeed; 
		OnRep_DungeonSeed();   // Call on server immediately
	}
}

// --- Client-Side Synchronization ---
void ADungeonGenerator::OnRep_DungeonSeed()
{
	// Runs on both Server and Clients after the seed is synced.
	GenerateDungeon();
}

// --- The Shared Logic ---
void ADungeonGenerator::GenerateDungeon()
{
	// 1. Initialize a random stream using the synchronized DungeonSeed
	FRandomStream Stream(DungeonSeed);

	// --- SIMPLIFIED LAYOUT LOGIC (Developer-Defined) ---
	
	// CRITICAL: We need a pointer to a URoomDataAsset. For testing,
	// you MUST REPLACE THIS with a call to load a specific Data Asset.
	// For example:
	
	// FSoftObjectPath AssetPath("/Game/DungeonContent/DA_TestRoom.DA_TestRoom");
	// URoomDataAsset* ChosenAsset = Cast<URoomDataAsset>(AssetPath.ResolveObject());
    
	// For now, let's assume we have a way to get the asset:
	URoomData* ChosenAsset = nullptr; // <--- Developer needs to load this!

	// 2. Get a unique local seed for this room instance
	// We use MAX_int32 (not MAX_INT) for the max range.
	int32 RoomLocalSeed = Stream.RandRange(0, MAX_int32);
	
	// 3. Define the spawn point (e.g., world origin)
	FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);
	FTransform SpawnTransform(SpawnLocation); 
	
	// 4. Spawn the room actor (use your Blueprint subclass)
	// **IMPORTANT:** Use the Blueprint subclass, NOT the C++ class, for spawning.
	// We assume you have a Blueprint called 'BP_MasterRoom'.
	
	TSubclassOf<AMasterRoom> RoomClassToSpawn = AMasterRoom::StaticClass(); 
	
	// Find the Blueprint class reference for robust spawning (replace BP_MasterRoom_C)
	// Example: ConstructorHelpers::FClassFinder<AMasterRoom> BP_RoomClass(TEXT("/Game/YourPath/BP_MasterRoom.BP_MasterRoom_C"));
	// if (BP_RoomClass.Succeeded()) { RoomClassToSpawn = BP_RoomClass.Class; }


	AMasterRoom* Room = GetWorld()->SpawnActor<AMasterRoom>(
		RoomClassToSpawn, 
		SpawnTransform
	);

	if (Room && ChosenAsset) // Ensure both asset and room are valid
	{
		// 5. Use the PUBLIC C++ SETTERS to pass synchronized data
		Room->SetRoomConfig(ChosenAsset); 
		Room->SetRoomLocalSeed(RoomLocalSeed);
		
		// 6. Trigger the deterministic build on all machines
		Room->ExecutePopulation();
	}
}