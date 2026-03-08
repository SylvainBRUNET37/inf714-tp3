#include "ClientGameMode.h"

#include "BackendSubsystem.h"
#include "SerializationUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UserSession.h"
#include "GenericPlatform/GenericPlatformHttp.h"

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();

	BeginPlayAsync();
}

UE5Coro::TCoroutine<> AClientGameMode::BeginPlayAsync()
{
	const FString UserDataSlotName = "UserDataSlot";
	const FString SessionSlotName = "SessionSlot";
	static constexpr int32 UserIndex = 0;

	if (UGameplayStatics::DoesSaveGameExist(UserDataSlotName, UserIndex))
	{
		UserData = SerializationUtils::DeserializeUserData(UserDataSlotName, UserIndex);
		check(UserData);

		UE_LOG(LogTemp, Log, TEXT("Loaded UserData from save."));

		if (UGameplayStatics::DoesSaveGameExist(SessionSlotName, UserIndex))
		{
			UserSession = SerializationUtils::DeserializeUserSession(SessionSlotName, UserIndex);
			check(UserSession);
			
			UE_LOG(LogTemp, Log, TEXT("Loaded UserSession from save."));
			
			if (not co_await RefreshAndSaveSession(SessionSlotName, UserIndex))
			{
				co_return;
			}
		}
		else
		{
			if (not co_await CreateAndSaveSession(SessionSlotName, UserIndex))
			{
				co_return;
			}
		}
	}
	else
	{
		if (not co_await LoginAndSaveUser(UserDataSlotName, UserIndex))
		{
			co_return;
		}

		if (not co_await CreateAndSaveSession(SessionSlotName, UserIndex))
		{
			co_return;
		}
	}
}

UE5Coro::TCoroutine<bool> AClientGameMode::LoginAndSaveUser(const FString& UserDataSlotName, const int32 UserIndex)
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const FHttpRequestRef Request =	BackendSubsystem->CreateSimpleHttpRequest(
		"Post",
		"users"
	);
	
	const auto Response =
		co_await BackendSubsystem->MakeHttpRequest(Request);

	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("LoginAndSaveUser: Failed to connect to backend"));
		co_return false;
	}

	UserData = SerializationUtils::DeserializeUserData(*Response);
	check(UserData);

	SerializationUtils::SaveUserData(UserData, UserDataSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("LoginAndSaveUser Complete"));

	co_return true;
}

UE5Coro::TCoroutine<bool> AClientGameMode::CreateAndSaveSession(const FString& SessionSlotName, const int32 UserIndex)
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const FHttpRequestRef Request =
		BackendSubsystem->CreateSimpleHttpRequest(
			"Post",
			"users/" + UserData->UserId + "/sessions/create",
			FString::Printf(TEXT("guestToken=%s"), *FGenericPlatformHttp::UrlEncode(UserData->GuestToken))
		);

	const auto Response =
		co_await BackendSubsystem->MakeHttpRequest(Request);

	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateAndSaveSession: Failed to connect to backend"));
		co_return false;
	}

	UserSession = SerializationUtils::DeserializeUserSession(*Response);
	check(UserSession);

	SerializationUtils::SaveUserSession(UserSession, SessionSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("CreateAndSaveSession Complete"));

	co_return true;
}

UE5Coro::TCoroutine<bool> AClientGameMode::RefreshAndSaveSession(const FString& SessionSlotName, const int32 UserIndex)
{
	const UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);

	const FString AuthHeader = FString::Printf(TEXT("Bearer %s"), *UserSession->SessionToken);
	const FHttpRequestRef Request = BackendSubsystem->CreateSimpleHttpRequest(
		"Post", "users/" + UserData->UserId + "/sessions/refresh");
	Request->SetHeader("Authorization", AuthHeader);

	const auto Response =
		co_await BackendSubsystem->MakeHttpRequest(Request);

	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("RefreshAndSaveSession: Failed to connect to backend"));
		co_return false;
	}

	UserSession = SerializationUtils::DeserializeUserSession(*Response);
	check(UserSession);

	SerializationUtils::SaveUserSession(UserSession, SessionSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("RefreshAndSaveSession Complete"));

	co_return true;
}

/*

UE5Coro::TCoroutine<> AClientGameMode::BeginPlayAsync()
{
	const FString SlotName = "PlayerSave";
	const FString SessionSlotName = "SessionSave";
	constexpr int32 LocalPlayerId = 0; 
	
	bool bCreateSession = true;
	
	// Loading existing guest account
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("Save exists, loading."));
		LoadAccountFromSave(SlotName, LocalPlayerId);
		LogUserData(UserData);
		
		bCreateSession = not UGameplayStatics::DoesSaveGameExist(SessionSlotName, 0);
		
		// Refresh the session
		if (not bCreateSession)
		{
			LoadSessionFromSave(SessionSlotName, LocalPlayerId);
			if (IsValid(UserSession))
			{
				UE_LOG(LogTemp, Log, TEXT("Trying to Refresh Session"));
				UserSession = co_await UBackendSubsystem::RefreshSession(UserData->UserId, UserData->GuestToken, UserSession->SessionToken);
				bCreateSession = not IsValid(UserSession);
			}
		}
	}
	// Creating a new guest account
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save not found, requesting guest account."));
		UserData = co_await UBackendSubsystem::LoginWithDeviceID();
		if (!IsValid(UserData))
		{
			UE_LOG(LogTemp, Error, TEXT("Creation failed."));
			DisplayLoginState(false);
			co_await UE5Coro::Latent::Seconds(2.f);
			co_return;
		}
		SaveData(UserData, SlotName, LocalPlayerId);		
	}
	
	// Session Creation
	if (bCreateSession)
	{
		UE_LOG(LogTemp, Log, TEXT("Trying to Create Session"));
		UserSession = co_await UBackendSubsystem::CreateSession(UserData->UserId, UserData->GuestToken);
	}
	
	if (!IsValid(UserSession))
	{
		DisplayLoginState(false);
		co_return;
	}
	
	// Save to storage
	SaveData(UserData, SlotName, LocalPlayerId);
	SaveData(UserSession, SessionSlotName, LocalPlayerId);
	
	UE_LOG(LogTemp, Log, TEXT("Login Complete."));
	DisplayLoginState(true);
	co_return;
}

void AClientGameMode::DisplayLoginState(const bool& bLoginSucceeded) const
{
	if
	(
		const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PC
	)
	{
		if
		(
			const AClientHUD* HUD = Cast<AClientHUD>(PC->GetHUD());
			HUD
		)
		{
			HUD->LogLoginState(bLoginSucceeded);
		}
	}
}

void AClientGameMode::SaveData(USaveGame* Data, const FString& SlotName, const int32 UserIndex)
{
	if (IsValid(Data))
	{
		UE_LOG(LogTemp, Log, TEXT("Saving guest : %s."), *Data->GetClass()->GetName());
		FAsyncSaveGameToSlotDelegate Delegate;
		Delegate.BindUObject(this, &AClientGameMode::OnSaveFinished);
		UGameplayStatics::AsyncSaveGameToSlot(Data, SlotName, UserIndex, Delegate);
	}
}

void AClientGameMode::LogUserData(const UUserData* Data)
{
	if (IsValid(Data))
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerName: %s"), *Data->PlayerName);
		UE_LOG(LogTemp, Log, TEXT("UserId: %s"), *Data->UserId);
		UE_LOG(LogTemp, Log, TEXT("GuestToken: %s"), *Data->GuestToken);
	}
}
*/
