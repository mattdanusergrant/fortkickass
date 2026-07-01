// Fort Kickass — the "gather" pillar of the survival slice (world-concept.md).
// A server-authoritative pickup: when a player walks into it, the SERVER consumes
// it and the item disappears for everyone. Teaches replicated actor destruction
// and the HasAuthority() gate — the next networking lesson after InteractableDoor.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class FORTKICKASS_API APickupItem : public AActor
{
	GENERATED_BODY()

public:
	APickupItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;

	// A pawn entering this sphere collects the item (server decides).
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> CollectTrigger;

	UFUNCTION()
	void OnCollectOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
