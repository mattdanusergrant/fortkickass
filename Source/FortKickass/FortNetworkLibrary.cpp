// Fort Kickass — M0 session join helpers. See FortNetworkLibrary.h for the why.

#include "FortNetworkLibrary.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

bool UFortNetworkLibrary::HostListenServer(const UObject* WorldContextObject, FName MapName)
{
	if (!WorldContextObject || MapName.IsNone())
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	// "?listen" promotes this session to a listen server so clients can connect.
	const FString TravelURL = MapName.ToString() + TEXT("?listen");
	World->ServerTravel(TravelURL);
	return true;
}

bool UFortNetworkLibrary::JoinServer(APlayerController* JoiningPlayer, const FString& Address)
{
	if (!JoiningPlayer || Address.IsEmpty())
	{
		return false;
	}

	// Absolute travel: leave the current session and connect to the host at Address.
	JoiningPlayer->ClientTravel(Address, TRAVEL_Absolute);
	return true;
}
