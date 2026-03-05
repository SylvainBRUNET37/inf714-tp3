#include "ClientGameMode.h"

#include "BackendSubsystem.h"
#include "Kismet/GameplayStatics.h"

UE5Coro::TCoroutine<> AClientGameMode::LoginWithDeviceID()
{
	UserData = co_await UBackendSubsystem::LoginWithDeviceID();
}

void AClientGameMode::LoadFromSave(const FString& SlotName, const int32& UserIndex)
{
	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	UserData = Cast<UUserData>(LoadedGame);
	if (IsValid(UserData))
	{
		UE_LOG(LogTemp, Log, TEXT("Loaded save."));
	}
}

void AClientGameMode::OnSaveFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Save succeeded."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Save failed."));
	}
}



UE5Coro::TCoroutine<> AClientGameMode::BeginPlayAsync()
{
	const FString SlotName = "PlayerSave";
	constexpr int32 LocalPlayerId = 0; 
	
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("Save exists, loading."));
		LoadFromSave(SlotName, LocalPlayerId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Save not found, requesting guest account."));
		UserData = co_await UBackendSubsystem::LoginWithDeviceID();
	}
	
	if (IsValid(UserData))
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerName: %s"), *UserData->PlayerName);
		UE_LOG(LogTemp, Log, TEXT("UserId: %s"), *UserData->UserId);
		UE_LOG(LogTemp, Log, TEXT("GuestToken: %s"), *UserData->GuestToken);
		
		FAsyncSaveGameToSlotDelegate Delegate;
		Delegate.BindUObject(this, &AClientGameMode::OnSaveFinished);
		UGameplayStatics::AsyncSaveGameToSlot(UserData, SlotName, LocalPlayerId, Delegate);
	}
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();
	BeginPlayAsync();
}
