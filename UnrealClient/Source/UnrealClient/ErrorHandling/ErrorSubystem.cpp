#include "ErrorSubystem.h"

void UErrorSubystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	OnError.AddDynamic(this, &UErrorSubystem::LogError);
}

void UErrorSubystem::LogError(const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
}
