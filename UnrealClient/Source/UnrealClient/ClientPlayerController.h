#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UE5Coro.h"

#include "ClientPlayerController.generated.h"

class UUserSession;
class UUserData;

UCLASS()
class UNREALCLIENT_API AClientPlayerController : public APlayerController
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
	void SetInputModeToUIOnly();
	
	UE5Coro::TCoroutine<> BeginPlayAsync();

	[[nodiscard]] UE5Coro::TCoroutine<bool> LogAndCreateSession();
	[[nodiscard]] UE5Coro::TCoroutine<bool> RetrieveUserData();

	[[nodiscard]] UE5Coro::TCoroutine<bool> LoginAndSaveUser();
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveSession();
	[[nodiscard]] UE5Coro::TCoroutine<bool> RefreshAndSaveSession();
};
