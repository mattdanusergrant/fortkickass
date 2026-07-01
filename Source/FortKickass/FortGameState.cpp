// Fort Kickass — replicated shared day/night clock. See FortGameState.h.

#include "FortGameState.h"

#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

AFortGameState::AFortGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFortGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFortGameState, TimeOfDay);
}

bool AFortGameState::IsNight() const
{
	return TimeOfDay < 6.f || TimeOfDay >= 20.f;
}

void AFortGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Only the server advances the clock; the new value replicates to every client.
	if (HasAuthority() && DayLengthSeconds > 0.f)
	{
		TimeOfDay += (DeltaSeconds / DayLengthSeconds) * 24.f;
		while (TimeOfDay >= 24.f)
		{
			TimeOfDay -= 24.f;
		}
	}

	// Every machine displays the current (replicated) time.
	ShowClock();
}

void AFortGameState::ShowClock() const
{
	if (!GEngine)
	{
		return;
	}

	const int32 Hours = FMath::FloorToInt(TimeOfDay);
	const int32 Minutes = FMath::FloorToInt((TimeOfDay - Hours) * 60.f);
	const FString Msg = FString::Printf(TEXT("Time %02d:%02d %s"), Hours, Minutes, IsNight() ? TEXT("(night)") : TEXT("(day)"));

	// Constant key -> overwrites the same on-screen line every frame instead of stacking.
	GEngine->AddOnScreenDebugMessage(1, 1.f, IsNight() ? FColor::Cyan : FColor::Yellow, Msg);
}
