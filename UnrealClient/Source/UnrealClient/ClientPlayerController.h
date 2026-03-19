#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UE5Coro.h"

#include "ClientPlayerController.generated.h"

class UUserSession;
class UUserData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginSuccessSignature);

UCLASS()
class UNREALCLIENT_API AClientPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	FOnLoginSuccessSignature OnLoginSuccess;
	
	UE5Coro::TCoroutine<> ChangeUserName(const FString& NewName) const;
	[[nodiscard]] UE5Coro::TCoroutine<FString> GetUserName() const;
	
	void ConnectWithSteam();
	UE5Coro::TCoroutine<> ConnectWithDeviceID();

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
	FString SteamAuthTicket;
	
	void SetInputModeToUIOnly();
	
	// Refresh the session if there is one, otherwise do nothing
	UE5Coro::TCoroutine<> TryRefreshSession();
	UE5Coro::TCoroutine<> LoginWithSteam();
	
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveSteamUser();
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveSteamSession();
	
	[[nodiscard]] UE5Coro::TCoroutine<bool> LoginAsTempUser();
	[[nodiscard]] UE5Coro::TCoroutine<bool> RetrieveUserData();

	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveTempUser();
	[[nodiscard]] UE5Coro::TCoroutine<bool> CreateAndSaveSession();
	[[nodiscard]] UE5Coro::TCoroutine<bool> RefreshAndSaveSession();
};
