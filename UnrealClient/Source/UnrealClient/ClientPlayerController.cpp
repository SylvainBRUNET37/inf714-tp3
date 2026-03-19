#include "ClientPlayerController.h"

#include "UnrealClient/Backend/BackendSubsystem.h"
#include "UnrealClient/Serialization/SerializationUtils.h"
#include "UnrealClient/User/UserData.h"
#include "UnrealClient/User/UserSession.h"
#include "Kismet/GameplayStatics.h"

UE5Coro::TCoroutine<> AClientPlayerController::ChangeUserName(const FString& NewName) const
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	
	if (co_await BackendSubsystem->ChangeUserName(UserData->UserId, UserSession->SessionToken, NewName))
	{
		UE_LOG(LogTemp, Log, TEXT("ChangeUserName: Successfully changed username to %s"), *NewName);
	}
}

UE5Coro::TCoroutine<FString> AClientPlayerController::GetUserName() const
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	
	if (const auto UserName = co_await BackendSubsystem->GetUserName(UserData->UserId, UserSession->SessionToken))
	{
		co_return SerializationUtils::DeserializeUserName(*UserName);
	}
	
	co_return FString("Unknown user name");
}

void AClientPlayerController::ConnectWithSteam()
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	
	BackendSubsystem->GetSteamAuthTicketForWebApi(FGetSteamAuthTicketForWebApiResponse::CreateLambda(
		[this](const bool bSuccess, const FString& AuthTicket)
		{
			if (not bSuccess)
			{
				UE_LOG(LogTemp, Error, TEXT("ConnectWithSteam: Failed to get Steam auth ticket"));
				return;
			}
			
			SteamAuthTicket = AuthTicket;
			
			[[maybe_unused]] const auto _ = LoginWithSteam();
		}));
}

UE5Coro::TCoroutine<> AClientPlayerController::ConnectWithDeviceID()
{
	const bool bSuccessfullyLoggedIn = co_await LoginAsTempUser();
	
	if (not bSuccessfullyLoggedIn)
	{
		UE_LOG(LogTemp, Error, TEXT("ConnectWithDeviceID: Failed to log in and create session"));
		co_return;
	}

	OnLoginSuccess.Broadcast();
}

void AClientPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputModeToUIOnly();
	
	TryRefreshSession();
}

void AClientPlayerController::SetInputModeToUIOnly()
{
	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(nullptr);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

UE5Coro::TCoroutine<> AClientPlayerController::TryRefreshSession()
{
	const bool bSuccessfullyLoggedIn = 
		UGameplayStatics::DoesSaveGameExist(UserDataSlotName, UserIndex) and co_await RetrieveUserData();

	if (not bSuccessfullyLoggedIn)
	{
		UE_LOG(LogTemp, Log, TEXT("User has no saved data, could not refresh session"));
		co_return;
	}
	
	OnLoginSuccess.Broadcast();
}

UE5Coro::TCoroutine<bool> AClientPlayerController::CreateAndSaveSteamUser()
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();
			
	const auto Response = co_await BackendSubsystem->CreateSteamUser(SteamAuthTicket);
	if (not Response)
	{
		co_return false;
	}
	
	UserData = SerializationUtils::DeserializeUserData(*Response);
	check(UserData);

	SerializationUtils::SaveUserData(UserData, UserDataSlotName, UserIndex);

	co_return true;
}

UE5Coro::TCoroutine<bool> AClientPlayerController::CreateAndSaveSteamSession()
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();

	const auto Response = 
		co_await BackendSubsystem->CreateSessionWithSteam(UserData->UserId, SteamAuthTicket);
	if (not Response)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateAndSaveSteamSession: Failed to create session with steam"));
		co_return false;
	}
	
	UserSession = SerializationUtils::DeserializeUserSession(*Response);
	check(UserSession);

	SerializationUtils::SaveUserSession(UserSession, SessionSlotName, UserIndex);
	UE_LOG(LogTemp, Log, TEXT("CreateAndSaveSteamSession Complete"));
	
	co_return true;
}

UE5Coro::TCoroutine<> AClientPlayerController::LoginWithSteam()
{
	if (not co_await CreateAndSaveSteamUser() or not co_await CreateAndSaveSteamSession())
	{
		UE_LOG(LogTemp, Error, TEXT("LoginWithSteam: Failed to log in with steam"));
		co_return;
	}
	
	OnLoginSuccess.Broadcast();
}

UE5Coro::TCoroutine<bool> AClientPlayerController::LoginAsTempUser()
{
	co_return co_await CreateAndSaveTempUser() and co_await CreateAndSaveSession();
}

UE5Coro::TCoroutine<bool> AClientPlayerController::RetrieveUserData()
{
	UserData = SerializationUtils::DeserializeUserData(UserDataSlotName, UserIndex);
	check(UserData);

	UE_LOG(LogTemp, Log, TEXT("Loaded UserData from save."));

	const bool bHasSession = UGameplayStatics::DoesSaveGameExist(SessionSlotName, UserIndex);

	if (bHasSession)
	{
		UserSession = SerializationUtils::DeserializeUserSession(SessionSlotName, UserIndex);
		check(UserSession);

		UE_LOG(LogTemp, Log, TEXT("Loaded UserSession from save."));
	}

	co_return co_await (bHasSession
		? RefreshAndSaveSession()
		: CreateAndSaveSession());
}

UE5Coro::TCoroutine<bool> AClientPlayerController::CreateAndSaveTempUser()
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();

	const auto Response = co_await BackendSubsystem->CreateTempUser();
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

UE5Coro::TCoroutine<bool> AClientPlayerController::CreateAndSaveSession()
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();

	const auto Response = co_await BackendSubsystem->CreateSession(UserData->UserId, UserData->GuestToken);
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

UE5Coro::TCoroutine<bool> AClientPlayerController::RefreshAndSaveSession()
{
	const TNonNullPtr <const UBackendSubsystem> BackendSubsystem = 
		GetGameInstance()->GetSubsystem<UBackendSubsystem>();

	const auto Response =
		co_await BackendSubsystem->RefreshSession(UserData->UserId, UserSession->SessionToken);
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
