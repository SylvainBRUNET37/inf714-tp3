#include "ErrorDisplayWidget.h"

#include "Components/MultiLineEditableTextBox.h"
#include "UnrealClient/ErrorHandling/ErrorSubystem.h"

class UErrorSubystem;

void UErrorDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(ErrorBox);
	
	ErrorBox->SetText(FText::FromString(""));
	ErrorBox->SetIsReadOnly(true);
	
	const TNonNullPtr ErrorSubsystem = 
		GetGameInstance()->GetSubsystem<UErrorSubystem>();
	
	ErrorSubsystem->OnError.AddDynamic(this, &UErrorDisplayWidget::DisplayErrorMessage);
}

void UErrorDisplayWidget::DisplayErrorMessage(const FString& ErrorMessage)
{
	const FString CurrentText = ErrorBox->GetText().ToString();
	const FString NewText = CurrentText + '\n' + ErrorMessage;
	
	ErrorBox->SetText(FText::FromString(NewText));
}
