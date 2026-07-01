// Fort Kickass — the shared world's clock (Gameplay Framework: GameState).
// GameState is replicated shared state that every player sees the same way — the
// counterpart to PlayerState (per-player). The server advances the time of day
// and replicates it, so the whole group is on one clock. This is the "light
// pressure" pillar of the survival slice (world-concept.md): a day/night rhythm.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FortGameState.generated.h"

UCLASS()
class FORTKICKASS_API AFortGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFortGameState();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Current time of day in hours [0,24). Server owns it; replicated to everyone.
	UFUNCTION(BlueprintPure, Category = "FortKickass|World")
	float GetTimeOfDay() const { return TimeOfDay; }

	UFUNCTION(BlueprintPure, Category = "FortKickass|World")
	bool IsNight() const;

protected:
	// The replicated clock. Server advances it in Tick; clients receive the value.
	UPROPERTY(Replicated, VisibleAnywhere, Category = "FortKickass|World")
	float TimeOfDay = 8.f; // start at 08:00

	// Real seconds for one full 24h cycle.
	UPROPERTY(EditDefaultsOnly, Category = "FortKickass|World")
	float DayLengthSeconds = 120.f;

	// On-screen readout (no HUD yet). Fixed message key overwrites one line — no spam.
	void ShowClock() const;
};
