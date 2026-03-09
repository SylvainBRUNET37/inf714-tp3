#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "UserSession.generated.h"

UCLASS()
class UNREALCLIENT_API UUserSession : public USaveGame
{
	GENERATED_BODY()

public:
	UUserSession() = default;

	UUserSession(FString SessionToken)
		: SessionToken(std::move(SessionToken))
	{
	}

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SessionToken{};
};
