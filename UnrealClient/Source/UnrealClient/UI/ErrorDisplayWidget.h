#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ErrorDisplayWidget.generated.h"

class UMultiLineEditableTextBox;

UCLASS(Abstract, BlueprintType)
class UErrorDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

protected:	
	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableTextBox* ErrorBox;
	
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void DisplayErrorMessage(const FString& ErrorMessage);
};
