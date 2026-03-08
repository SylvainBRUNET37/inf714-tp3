#pragma once

#include "CoreMinimal.h"
#include "UserData.h"
#include "GameFramework/GameModeBase.h"
#include "UE5Coro.h"

#include "ClientGameMode.generated.h"

class UUserSession;

UCLASS()
class UNREALCLIENT_API AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite)
	UUserData* UserData;

	UPROPERTY(BlueprintReadWrite)
	UUserSession* UserSession;

	virtual void BeginPlay() override;
	
private:
	UE5Coro::TCoroutine<> BeginPlayAsync();
	
	[[nodiscard]] UE5Coro::TCoroutine<bool> LoginAndSaveUser(const FString& UserDataSlotName, const int32 UserIndex);
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveSession(const FString& SessionSlotName, const int32 UserIndex);
	[[nodiscard]] UE5Coro::TCoroutine<bool>	RefreshAndSaveSession(const FString& SessionSlotName, const int32 UserIndex);
};
