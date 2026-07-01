// Fort Kickass — a placed, replicated structure. See Buildable.h for the why.

#include "Buildable.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABuildable::ABuildable()
{
	PrimaryActorTick.bCanEverTick = false;

	// Spawned on the server → the initial transform + existence replicate to every client.
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Default to the engine cube so a placed block is visible with zero editor setup.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
}
