// Fort Kickass — a player's replicated data (Gameplay Framework: PlayerState).
// Holds the resources this player has gathered. The server owns the count and
// replicates it; a RepNotify prints it on screen so you can see gathering work
// without building a HUD yet.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FortPlayerState.generated.h"

UCLASS()
class FORTKICKASS_API AFortPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Server-only: add to this player's gathered resources. No-op on clients.
	void AddResource(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "FortKickass|Resources")
	int32 GetResourceCount() const { return ResourceCount; }

protected:
	// The replicated truth. Server sets it; clients receive it via OnRep_ResourceCount.
	UPROPERTY(ReplicatedUsing = OnRep_ResourceCount, VisibleAnywhere, Category = "FortKickass|Resources")
	int32 ResourceCount = 0;

	UFUNCTION()
	void OnRep_ResourceCount();

	// Temporary readout (no HUD yet) — prints "<name> resources: N" to the screen.
	void ShowResourceCount() const;
};
