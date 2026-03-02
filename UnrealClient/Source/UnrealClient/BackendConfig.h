#pragma once

namespace BackendConfig
{
	constexpr TCHAR APIBaseURL[] = TEXT("http://localhost:5000/api/");
	
	[[nodiscard]] inline FString GetEndpointURL(const FString& Endpoint)
	{
		return APIBaseURL + Endpoint;
	}
};
