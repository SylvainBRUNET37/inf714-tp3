#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ClientPlayerController.generated.h"

UCLASS()
class UNREALCLIENT_API AClientPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
