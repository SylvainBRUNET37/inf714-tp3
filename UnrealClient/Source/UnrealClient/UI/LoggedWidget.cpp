#include "LoggedWidget.h"

#include "ClientHUD.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "UnrealClient/ClientPlayerController.h"

void ULoggedWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(UserNameTextBox);
	UserNameTextBox->SetText(FText::FromString("John Unreal"));
	UserNameTextBox->SetIsReadOnly(false);
	
	check(ValidateButton)
	ValidateButton->OnPressed.AddDynamic(this, &ULoggedWidget::ChangeUserName);
	
	check(GoBackButton);
	GoBackButton->OnPressed.AddDynamic(this, &ULoggedWidget::GoBack);
}

void ULoggedWidget::NativeDestruct()
{
	if (GoBackButton)
	{
		GoBackButton->OnPressed.RemoveDynamic(this, &ULoggedWidget::GoBack);
	}
	
	if (ValidateButton)
	{
		ValidateButton->OnPressed.RemoveDynamic(this, &ULoggedWidget::ChangeUserName);
	}
	
	Super::NativeDestruct();
}

void ULoggedWidget::ChangeUserName()
{
	const TNonNullPtr<const AClientPlayerController> OwningPlayerController = 
		Cast<AClientPlayerController>(GetOwningPlayer());
	
	[[maybe_unused]] const auto _ = 
		OwningPlayerController->ChangeUserName(UserNameTextBox->GetText().ToString());
}

void ULoggedWidget::GoBack()
{
	const TNonNullPtr<const AClientPlayerController> OwningPlayerController = 
		Cast<AClientPlayerController>(GetOwningPlayer());
	
	const TNonNullPtr<const AClientHUD> ClientHUD = 
		Cast<AClientHUD>(OwningPlayerController->GetHUD());
	
	ClientHUD->SwitchToLogginWidget();
}

void ULoggedWidget::SetUserName(const FString& UserName) const
{
	UserNameTextBox->SetText(FText::FromString(UserName));
}
