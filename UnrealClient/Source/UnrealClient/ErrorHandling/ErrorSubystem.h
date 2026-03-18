#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ErrorSubystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErrorSignature, const FString&, ErrorMessage);

UCLASS()
class UNREALCLIENT_API UErrorSubystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	FOnErrorSignature OnError;
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
private:
	UFUNCTION()
	void LogError(const FString& ErrorMessage);
};
