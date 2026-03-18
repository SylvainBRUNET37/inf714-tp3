#include "ErrorHandlingUtils.h"

#include "ErrorSubystem.h"

void ErrorHandlingUtils::BroadcastError(
	const TNonNullPtr<const UGameInstance> GameInstance, 
	const FString& ErrorMessage)
{
	const TNonNullPtr<const UErrorSubystem> ErrorSubsystem = 
		GameInstance->GetSubsystem<UErrorSubystem>();
	
	ErrorSubsystem->OnError.Broadcast(ErrorMessage);
}
