#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ClientGameMode.generated.h"

UCLASS()
class UNREALCLIENT_API AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
};
