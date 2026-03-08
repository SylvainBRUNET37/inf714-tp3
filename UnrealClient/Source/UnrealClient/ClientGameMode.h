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

	UPROPERTY(BlueprintReadonly, EditAnywhere)
	FString UserDataSlotName = "UserDataSlot";

	UPROPERTY(BlueprintReadonly, EditAnywhere)
	FString SessionSlotName = "SessionSlot";

	UPROPERTY(BlueprintReadonly, EditAnywhere)
	int32 UserIndex = 0;

	virtual void BeginPlay() override;

private:
	UE5Coro::TCoroutine<> BeginPlayAsync();

	[[nodiscard]] UE5Coro::TCoroutine<bool> LogAndCreateSession();
	[[nodiscard]] UE5Coro::TCoroutine<bool> RetrieveUserData();

	[[nodiscard]] UE5Coro::TCoroutine<bool> LoginAndSaveUser();
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveSession();
	[[nodiscard]] UE5Coro::TCoroutine<bool> RefreshAndSaveSession();
};
