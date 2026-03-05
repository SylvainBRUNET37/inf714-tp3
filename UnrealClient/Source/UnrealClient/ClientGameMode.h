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
	void LoadFromSave(const FString& SlotName, const int32& UserIndex);
	
	void OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	
	UE5Coro::TCoroutine<> BeginPlayAsync();
	
	virtual void BeginPlay() override;
};
