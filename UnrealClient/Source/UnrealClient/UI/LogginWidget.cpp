#include "LogginWidget.h"

#include "Components/Button.h"
#include "UnrealClient/ClientPlayerController.h"

void ULogginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(SteamConnexionButton);
	SteamConnexionButton->OnPressed.AddDynamic(this, &ULogginWidget::ConnectWithSteam);
	
	check(DeviceIdConnexionButton)
	DeviceIdConnexionButton->OnPressed.AddDynamic(this, &ULogginWidget::ConnectWithDeviceId);
}

void ULogginWidget::NativeDestruct()
{
	if (DeviceIdConnexionButton)
	{
		DeviceIdConnexionButton->OnPressed.RemoveDynamic(this, &ULogginWidget::ConnectWithDeviceId);
	}
	
	if (SteamConnexionButton)
	{
		SteamConnexionButton->OnPressed.RemoveDynamic(this, &ULogginWidget::ConnectWithSteam);
	}
	
	Super::NativeDestruct();
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
