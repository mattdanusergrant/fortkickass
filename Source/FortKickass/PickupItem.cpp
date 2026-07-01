// Fort Kickass — server-authoritative pickup. See PickupItem.h for the why.

#include "PickupItem.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FortPlayerState.h"
#include "GameFramework/Pawn.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	// Replicated so the server's Destroy() removes the item on every client too.
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	CollectTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("CollectTrigger"));
	CollectTrigger->SetupAttachment(Mesh);
	CollectTrigger->SetSphereRadius(96.f);
	CollectTrigger->SetCollisionProfileName(TEXT("Trigger"));
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();

	CollectTrigger->OnComponentBeginOverlap.AddDynamic(this, &APickupItem::OnCollectOverlap);
}

void APickupItem::OnCollectOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	// Overlap fires on every machine, but only the server owns the world state.
	if (!HasAuthority())
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	// Credit the collecting player. AddResource is server-guarded and replicates the new count.
	if (AFortPlayerState* PS = Pawn->GetPlayerState<AFortPlayerState>())
	{
		PS->AddResource(1);
	}

	// Destroying a replicated actor on the server removes it for all connected clients.
	Destroy();
}
