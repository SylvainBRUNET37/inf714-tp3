#include "ClientHUD.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

AClientHUD::AClientHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/UI/WP_StartHUD"));

	if (WidgetBPClass.Succeeded())
	{
		StartHUDClass = WidgetBPClass.Class;
	}
}

void AClientHUD::BeginPlay()
{
	Super::BeginPlay();

	if (StartHUDClass)
	{
		StartHUD = CreateWidget<UUserWidget>(GetWorld(), StartHUDClass);

		if (StartHUD)
		{
			StartHUD->AddToViewport();
		}
	}
}

void AClientHUD::LogLoginState(const bool& bLoginSucceeded)
{
	if (GEngine)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), TEXT("Log LoginState"));
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			bLoginSucceeded ? FColor::Green : FColor::Red,
			bLoginSucceeded ? TEXT("Login successful !") : TEXT("Login failed.")
		);
	}
}
