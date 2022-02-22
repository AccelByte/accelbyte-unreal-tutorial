// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteServerManager.h"

#include "AccelByteOnlineSystem.h"
#include "Core/AccelByteRegistry.h"
#include "GameServerApi/AccelByteServerDSMApi.h"
#include "GameServerApi/AccelByteServerMatchmakingApi.h"
#include "GameServerApi/AccelByteServerOauth2Api.h"

bool UAccelByteServerManager::CheckCommandLineArgument(const FString& Argument)
{
	TArray<FString> tokens, switches;
	FCommandLine::Parse(FCommandLine::Get(), tokens, switches);
	for (auto& token : tokens) {
		if (token == Argument)
		{
			return true;
		}
	}
	return false;
}

void UAccelByteServerManager::LoginWithCredentials(const FLoginWithCredentialsSuccess& LoginWithCredentialsSuccess, const FSendFailedInfo& LoginWithCredentialsFailed)
{
	FRegistry::ServerOauth2.LoginWithClientCredentials(FVoidHandler::CreateLambda([LoginWithCredentialsSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Login With Credentials success"));

		LoginWithCredentialsSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateLambda([LoginWithCredentialsFailed](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Login With Credentials failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		LoginWithCredentialsFailed.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::ForgetAllCredentials() const
{
	FRegistry::ServerOauth2.ForgetAllCredentials();
}

void UAccelByteServerManager::RegisterLocalServerToDSM(const FRegisterLocalServerToDSMSuccess& RegisterLocalServerToDsmSuccess, const FSendFailedInfo& RegisterLocalServerToDsmFailed)
{
	FRegistry::ServerDSM.RegisterLocalServerToDSM(FString("127.0.0.1"), 7777, FString::Printf(TEXT("localds-"), *FPlatformMisc::GetDeviceId()), FVoidHandler::CreateLambda([RegisterLocalServerToDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Register Local Server to DSM success"));

		RegisterLocalServerToDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateLambda([RegisterLocalServerToDsmFailed](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Register Local Server to DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		RegisterLocalServerToDsmFailed.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::RegisterServerToDSM(const FRegisterServerToDSMSuccess& RegisterServerToDsmSuccess, const FSendFailedInfo& RegisterServerToDsmFailed)
{
	FRegistry::ServerDSM.RegisterServerToDSM(7777, FVoidHandler::CreateLambda([RegisterServerToDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Register Server to DSM success"));

		RegisterServerToDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateLambda([RegisterServerToDsmFailed](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Register Server to DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		RegisterServerToDsmFailed.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::GetSessionId(const FGetSessionIdSuccess& GetSessionIdSuccess, const FSendFailedInfo& GetSessionIdError)
{
	FRegistry::ServerDSM.GetSessionId(THandler<FAccelByteModelsServerSessionResponse>::CreateLambda([GetSessionIdSuccess](const FAccelByteModelsServerSessionResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Get Session Id success"));

		GetSessionIdSuccess.ExecuteIfBound(Result);
	}), FErrorHandler::CreateLambda([GetSessionIdError](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Session Id error, Error Code: %d, Error Message: %s"), ErrorCode, *ErrorMessage);

		GetSessionIdError.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::SetMatchId(const FString& MatchId)
{
	FRegistry::ServerCredentials.SetMatchId(MatchId);
}

void UAccelByteServerManager::QuerySessionStatus(const FQuerySessionStatusSuccess& QuerySessionStatusSuccess, const FSendFailedInfo& QuerySessionStatusError)
{
	FRegistry::ServerMatchmaking.QuerySessionStatus(FRegistry::ServerCredentials.GetMatchId(), THandler<FAccelByteModelsMatchmakingResult>::CreateLambda([QuerySessionStatusSuccess](const FAccelByteModelsMatchmakingResult& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Query Session Status success"));

		QuerySessionStatusSuccess.ExecuteIfBound(Result);
	}), FErrorHandler::CreateLambda([QuerySessionStatusError](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Query Session Status error, Error Code: %d, Error Message: %s"), ErrorCode, *ErrorMessage);

		QuerySessionStatusError.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::DeregisterLocalServerFromDSM(const FDeregisterLocalServerFromDSMSuccess& DeregisterLocalServerFromDsmSuccess, const FSendFailedInfo& DeregisterLocalServerFromDsmError)
{
	FRegistry::ServerDSM.DeregisterLocalServerFromDSM(FString::Printf(TEXT("localds:%s"), *FPlatformMisc::GetDeviceId()), FVoidHandler::CreateLambda([DeregisterLocalServerFromDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Deregister Local Server from DSM success"))

		DeregisterLocalServerFromDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateLambda([DeregisterLocalServerFromDsmError](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Deregister Local Server from DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		DeregisterLocalServerFromDsmError.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::SendShutdownToDSM(const FSendShutDownToDSMSuccess& SendShutDownToDsmSuccess,	const FSendFailedInfo& SendShutdownToDsmError)
{
	FRegistry::ServerDSM.SendShutdownToDSM(true, FRegistry::ServerCredentials.GetMatchId(), FVoidHandler::CreateLambda([SendShutDownToDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Send Shutdown to DSM success"));

		SendShutDownToDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateLambda([SendShutdownToDsmError](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Send Shutdown to DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		SendShutdownToDsmError.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

UAccelByteServerManager* UAccelByteServerManager::CreateAccelByteServer(UGameInstance* GameInstance)
{
	UAccelByteServerManager* Request = CreateOnlineRequest<UAccelByteServerManager>(GameInstance);
	return Request;
}

FString UAccelByteServerManager::GetMatchId() const
{
	return FRegistry::ServerCredentials.GetMatchId();
}
