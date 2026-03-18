#include "ClientHUD.h"

#include "ErrorDisplayWidget.h"
#include "LoggedWidget.h"
#include "LogginWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "UnrealClient/ClientPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AClientHUD::AClientHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ErrorDisplayWidgetBpClass(TEXT("/Game/UI/WP_ErrorDisplay"));
	ensure(ErrorDisplayWidgetBpClass.Succeeded());
	
	static ConstructorHelpers::FClassFinder<UUserWidget> LogginWidgetBpClass(TEXT("/Game/UI/WP_Loggin"));
	ensure(LogginWidgetBpClass.Succeeded());
	
	static ConstructorHelpers::FClassFinder<UUserWidget> LoggedWidgetBpClass(TEXT("/Game/UI/WP_Logged"));
	ensure(LoggedWidgetBpClass.Succeeded());

	ErrorDisplayWidgetClass = ErrorDisplayWidgetBpClass.Class;
	LogginWidgetClass = LogginWidgetBpClass.Class;
	LoggedWidgetClass = LoggedWidgetBpClass.Class;
}

void AClientHUD::BeginPlay()
{
	Super::BeginPlay();

	check(ErrorDisplayWidgetClass);
	ErrorDisplayWidget = CreateWidget<UUserWidget>(GetWorld(), ErrorDisplayWidgetClass);
	if (ensure(ErrorDisplayWidget))
	{
		ErrorDisplayWidget->AddToViewport();
	}
	
	check(LogginWidgetClass);
	LogginWidget = CreateWidget<ULogginWidget>(GetWorld(), LogginWidgetClass);
	if (ensure(LogginWidget))
	{
		LogginWidget->AddToViewport();
	}
	
	check(LoggedWidgetClass);
	LoggedWidget = CreateWidget<ULoggedWidget>(GetWorld(), LoggedWidgetClass);
	ensure(LoggedWidget);
	
	const TNonNullPtr OwningPlayerController = 
		Cast<AClientPlayerController>(GetOwningPlayerController());
	
	OwningPlayerController->OnLoginSuccess.AddDynamic(this, &AClientHUD::DisplayLoggedWidget);
}

void AClientHUD::DisplayLoggedWidget()
{
	[[maybe_unused]] const auto _ = GetAndDisplayUserName();
	
	LoggedWidget->AddToViewport();
}

UE5Coro::TCoroutine<> AClientHUD::GetAndDisplayUserName() const
{
	const TNonNullPtr<const AClientPlayerController> OwningPlayerController = 
		Cast<AClientPlayerController>(GetOwningPlayerController());
	
	const FString UserName = co_await OwningPlayerController->GetUserName();
	
	LoggedWidget->SetUserName(UserName);
}
