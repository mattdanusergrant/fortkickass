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

bool AFortPlayerState::TrySpendResource(int32 Amount)
{
	// Server validates: only spend if the player actually has it.
	if (!HasAuthority() || ResourceCount < Amount)
	{
		return false;
	}

	ResourceCount -= Amount;
	ShowResourceCount(); // RepNotify won't fire on the server that changed it.
	return true;
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
