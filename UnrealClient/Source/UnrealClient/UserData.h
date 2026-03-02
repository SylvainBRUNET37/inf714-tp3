#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "UserData.generated.h"

UCLASS()
class UNREALCLIENT_API UUserData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName{};
};