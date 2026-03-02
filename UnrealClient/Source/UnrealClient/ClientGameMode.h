#pragma once

#include "CoreMinimal.h"
#include "UserData.h"
#include "GameFramework/GameModeBase.h"
#include "UE5Coro.h"

#include "ClientGameMode.generated.h"

UCLASS()
class UNREALCLIENT_API AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
	UUserData* UserData;
	
	UE5Coro::TCoroutine<> LoginWithDeviceID();
	
	virtual void BeginPlay() override;
};
