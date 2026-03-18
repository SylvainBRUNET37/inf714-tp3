#include "LogginWidget.h"

#include "Components/Button.h"
#include "UnrealClient/ClientPlayerController.h"

void ULogginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(SteamConnexionButton);
	SteamConnexionButton->OnClicked.AddDynamic(this, &ULogginWidget::ConnectWithSteam);
	
	check(DeviceIdConnexionButton)
	DeviceIdConnexionButton->OnClicked.AddDynamic(this, &ULogginWidget::ConnectWithDeviceId);
}

void ULogginWidget::ConnectWithSteam()
{
	const TNonNullPtr OwningPlayerController = 
		Cast<AClientPlayerController>(GetOwningPlayer());
	
	OwningPlayerController->ConnectWithSteam();
}

void ULogginWidget::ConnectWithDeviceId()
{
	const TNonNullPtr  OwningPlayerController = 
		Cast<AClientPlayerController>(GetOwningPlayer());
	
	OwningPlayerController->ConnectWithDeviceID();
}
