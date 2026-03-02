#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginMenuWidget.generated.h"

UCLASS()
class UNREALCLIENT_API ULoginMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (LoginWithDeviceID))
	class UButton* LoginWithDeviceIDButton;

	UFUNCTION()
	void LoginWithDeviceID();
};
