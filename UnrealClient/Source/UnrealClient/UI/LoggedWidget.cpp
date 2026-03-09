#include "LoggedWidget.h"

#include "Components/EditableText.h"
#include "Components/Button.h"
#include "UnrealClient/ClientPlayerController.h"
#include "UnrealClient/Backend/BackendSubsystem.h"

void ULoggedWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(UserNameTextBox);
	UserNameTextBox->SetText(FText::FromString("John Unreal"));
	UserNameTextBox->SetIsReadOnly(false);
	
	check(ValidateButton)
	ValidateButton->OnClicked.AddDynamic(this, &ULoggedWidget::ChangeUserName);
}

void ULoggedWidget::ChangeUserName()
{
	const AClientPlayerController* OwningPlayerController = Cast<AClientPlayerController>(GetOwningPlayer());
	check(OwningPlayerController);
	
	[[maybe_unused]] const auto _ = 
		OwningPlayerController->ChangeUserName(UserNameTextBox->GetText().ToString());
}

void ULoggedWidget::SetUserName(const FString& UserName) const
{
	UserNameTextBox->SetText(FText::FromString(UserName));
}
