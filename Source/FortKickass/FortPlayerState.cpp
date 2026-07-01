// Fort Kickass — replicated per-player resource count. See FortPlayerState.h.

#include "FortPlayerState.h"

#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

void AFortPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFortPlayerState, ResourceCount);
}

void AFortPlayerState::AddResource(int32 Amount)
{
	// Only the server changes the truth; the new value replicates to clients.
	if (!HasAuthority())
	{
		return;
	}

	ResourceCount += Amount;

	// RepNotify does NOT fire on the machine that set the value, so refresh the host's own readout here.
	ShowResourceCount();
}

void AFortPlayerState::OnRep_ResourceCount()
{
	// Runs on clients when the server's ResourceCount replicates down.
	ShowResourceCount();
}

void AFortPlayerState::ShowResourceCount() const
{
	if (GEngine)
	{
		const FString Msg = FString::Printf(TEXT("%s resources: %d"), *GetPlayerName(), ResourceCount);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Msg);
	}
}
