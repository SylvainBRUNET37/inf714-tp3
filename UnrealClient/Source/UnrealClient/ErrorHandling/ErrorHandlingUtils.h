#pragma once

#include "CoreMinimal.h"

namespace ErrorHandlingUtils
{
	void BroadcastError(const TNonNullPtr<const UGameInstance> GameInstance, const FString& ErrorMessage);	
};
