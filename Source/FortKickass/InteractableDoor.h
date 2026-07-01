// Fort Kickass — M0 replicated interaction.
// A server-authoritative door: the server owns the open/closed state and
// replicates it; every machine swings the mesh when that state changes.
// This is the smallest possible demonstration of the replication toolkit
// (bReplicates, a replicated variable, RepNotify, HasAuthority) — the core
// M0 networking lesson. See ARCHITECTURE.md §1 in the vault.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableDoor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class FORTKICKASS_API AInteractableDoor : public AActor
{
	GENERATED_BODY()

public:
	AInteractableDoor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	// Visual root + the door leaf that rotates.
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<USceneComponent> DoorRoot;

	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	// A pawn entering this volume toggles the door (server decides — see OnTriggerBeginOverlap).
	UPROPERTY(VisibleAnywhere, Category = "Door")
	TObjectPtr<UBoxComponent> InteractionTrigger;

	// The one piece of replicated truth. RepNotify fires OnRep_IsOpen on clients when it changes.
	UPROPERTY(ReplicatedUsing = OnRep_IsOpen, VisibleAnywhere, Category = "Door")
	bool bIsOpen = false;

	// Yaw (degrees) applied to the door leaf when open.
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenYaw = 90.f;

	UFUNCTION()
	void OnRep_IsOpen();

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Poses the mesh for the current bIsOpen value. Runs on the server and on
	// every client (via RepNotify), so all machines agree on what the door looks like.
	void ApplyDoorState();
};
