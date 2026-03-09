#include "ClientHUD.h"

#include "ErrorDisplayWidget.h"
#include "LoggedWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "UnrealClient/ClientPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AClientHUD::AClientHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ErrorDisplayWidgetBpClass(TEXT("/Game/UI/WP_ErrorDisplay"));
	ensure(ErrorDisplayWidgetBpClass.Succeeded());
	
	static ConstructorHelpers::FClassFinder<UUserWidget> LoggedWidgetBpClass(TEXT("/Game/UI/WP_Logged"));
	ensure(LoggedWidgetBpClass.Succeeded());

	ErrorDisplayWidgetClass = ErrorDisplayWidgetBpClass.Class;
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
	
	check(LoggedWidgetClass);
	LoggedWidget = CreateWidget<UUserWidget>(GetWorld(), LoggedWidgetClass);
	ensure(LoggedWidget);
	
	AClientPlayerController* OwningPlayerController = Cast<AClientPlayerController>(GetOwningPlayerController());
	check(OwningPlayerController);
	
	OwningPlayerController->OnLoginSuccess.AddDynamic(this, &AClientHUD::DisplayLoggedWidget);
}

void AClientHUD::DisplayLoggedWidget()
{
	LoggedWidget->AddToViewport();
}
