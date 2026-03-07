#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "ClientHUD.generated.h"

UCLASS()
class UNREALCLIENT_API AClientHUD : public AHUD
{
	GENERATED_BODY()

public:
	AClientHUD();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> StartHUDClass;

	UPROPERTY()
	UUserWidget* StartHUD;
	
public:
	static void LogLoginComplete();
};
