#include "ErrorDisplayWidget.h"

#include "Components/MultiLineEditableTextBox.h"
#include "UnrealClient/Backend/BackendSubsystem.h"

void UErrorDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(ErrorBox);
	
	ErrorBox->SetText(FText::FromString(""));
	ErrorBox->SetIsReadOnly(true);
	
	UBackendSubsystem* BackendSubsystem = GetGameInstance()->GetSubsystem<UBackendSubsystem>();
	check(BackendSubsystem);
	
	BackendSubsystem->OnHttpError.AddDynamic(this, &UErrorDisplayWidget::DisplayErrorMessage);
}

void UErrorDisplayWidget::DisplayErrorMessage(const FString& ErrorMessage)
{
	const FString CurrentText = ErrorBox->GetText().ToString();
	const FString NewText = CurrentText + '\n' + ErrorMessage;
	
	ErrorBox->SetText(FText::FromString(NewText));
}
