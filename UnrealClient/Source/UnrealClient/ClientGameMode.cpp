#include "ClientGameMode.h"

#include "BackendSubsystem.h"

UE5Coro::TCoroutine<> AClientGameMode::LoginWithDeviceID()
{
	UserData = co_await UBackendSubsystem::LoginWithDeviceID();
	
	UE_LOG(LogTemp, Log, TEXT("PlayerName: %s"), *UserData->PlayerName);
	UE_LOG(LogTemp, Log, TEXT("UserId: %s"), *UserData->UserId);
	UE_LOG(LogTemp, Log, TEXT("GuestToken: %s"), *UserData->GuestToken);
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LoginWithDeviceID();
}
