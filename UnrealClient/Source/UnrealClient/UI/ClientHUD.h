#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ClientHUD.generated.h"

class UErrorDisplayWidget;
class ULoggedWidget;

UCLASS()
class UNREALCLIENT_API AClientHUD : public AHUD
{
	GENERATED_BODY()

public:
	AClientHUD();

protected:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UErrorDisplayWidget> ErrorDisplayWidgetClass;
	
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<ULoggedWidget> LoggedWidgetClass;
	
	UPROPERTY()
	UUserWidget* ErrorDisplayWidget;
	
	UPROPERTY()
	UUserWidget* LoggedWidget;
	
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DisplayLoggedWidget();
};
