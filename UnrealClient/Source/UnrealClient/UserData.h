#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "UserData.generated.h"

UCLASS()
class UNREALCLIENT_API UUserData : public USaveGame
{
	GENERATED_BODY()

public:
	UUserData() = default;

	UUserData(FString PlayerName, FString UserId, FString GuestToken)
		: PlayerName(std::move(PlayerName))
		  , UserId(std::move(UserId))
		  , GuestToken(std::move(GuestToken))
	{
	}

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName{};

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString UserId{};

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString GuestToken{};
};
