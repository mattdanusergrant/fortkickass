// Fort Kickass — M0 session join helpers.
// Thin Blueprint-callable wrappers over Unreal's built-in travel API so a menu
// (or a key bind, or the level Blueprint) can host and join without touching C++.
// This is the "one hosts, one joins" half of the M0 Definition of Done.
// Direct-IP / LAN only for now; Steam friend invites come later (Phase 5).

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FortNetworkLibrary.generated.h"

class APlayerController;

UCLASS()
class FORTKICKASS_API UFortNetworkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// HOST: open MapName as a listen server. Call on the machine that will host;
	// it becomes the authority and starts accepting client connections.
	UFUNCTION(BlueprintCallable, Category = "FortKickass|Network", meta = (WorldContext = "WorldContextObject"))
	static bool HostListenServer(const UObject* WorldContextObject, FName MapName);

	// JOIN: connect this client to a host. Address is "127.0.0.1" for the same PC,
	// or the host's LAN / public IP. Equivalent to the `open <address>` console command.
	UFUNCTION(BlueprintCallable, Category = "FortKickass|Network")
	static bool JoinServer(APlayerController* JoiningPlayer, const FString& Address);
};
