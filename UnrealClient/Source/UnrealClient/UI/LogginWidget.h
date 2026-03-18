#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LogginWidget.generated.h"

class UEditableText;
class UButton;

UCLASS(Abstract, BlueprintType)
class ULogginWidget : public UUserWidget
{
	GENERATED_BODY()

protected:	
	UPROPERTY(meta = (BindWidget))
	UButton* SteamConnexionButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* DeviceIdConnexionButton;
	
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void ConnectWithSteam();
	
	UFUNCTION()
	void ConnectWithDeviceId();
};
