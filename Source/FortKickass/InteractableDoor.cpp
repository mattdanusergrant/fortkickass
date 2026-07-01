// Fort Kickass — M0 replicated interaction. See InteractableDoor.h for the why.

#include "InteractableDoor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

AInteractableDoor::AInteractableDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	// This actor's state is owned by the server and pushed to clients.
	bReplicates = true;

	DoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DoorRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorRoot);

	InteractionTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionTrigger"));
	InteractionTrigger->SetupAttachment(DoorRoot);
	InteractionTrigger->SetBoxExtent(FVector(96.f, 96.f, 96.f));
	InteractionTrigger->SetCollisionProfileName(TEXT("Trigger"));
}

void AInteractableDoor::BeginPlay()
{
	Super::BeginPlay();

	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractableDoor::OnTriggerBeginOverlap);

	// Make sure a client that joined late (or an actor placed already-open) renders correctly.
	ApplyDoorState();
}

void AInteractableDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractableDoor, bIsOpen);
}

void AInteractableDoor::OnTriggerBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	// Golden rule: the client requests, the server decides, the result replicates back.
	// Overlap fires on every machine — but only the server is allowed to change the truth.
	if (!HasAuthority())
	{
		return;
	}

	if (!Cast<APawn>(OtherActor))
	{
		return;
	}

	bIsOpen = !bIsOpen;

	// RepNotify does NOT fire on the machine that set the value, so pose the server's own copy here.
	ApplyDoorState();
}

void AInteractableDoor::OnRep_IsOpen()
{
	// Runs on clients when the server's bIsOpen replicates down.
	ApplyDoorState();
}

void AInteractableDoor::ApplyDoorState()
{
	const float Yaw = bIsOpen ? OpenYaw : 0.f;
	DoorMesh->SetRelativeRotation(FRotator(0.f, Yaw, 0.f));
}
