#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LoggedWidget.generated.h"

class UEditableText;
class UButton;

UCLASS(Abstract, BlueprintType)
class ULoggedWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetUserName(const FString& UserName) const;

protected:	
	UPROPERTY(meta = (BindWidget))
	UEditableText* UserNameTextBox;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ValidateButton;
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void ChangeUserName();
};
