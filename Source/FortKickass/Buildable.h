// Fort Kickass — the "build/place" pillar (world-concept.md): something persistent
// the whole group can see. A player presses Build → their client asks the server
// (Server RPC on the Character) → the server spawns one of these → it replicates to
// everyone. Because it's spawned server-side and replicates, it's shared world state.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buildable.generated.h"

class UStaticMeshComponent;

UCLASS()
class FORTKICKASS_API ABuildable : public AActor
{
	GENERATED_BODY()

public:
	ABuildable();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Buildable")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
