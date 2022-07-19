// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteServerManager.h"

#include "AccelByteOnlineSystem.h"
#include "Core/AccelByteRegistry.h"
#include "GameServerApi/AccelByteServerDSMApi.h"
#include "GameServerApi/AccelByteServerMatchmakingApi.h"
#include "GameServerApi/AccelByteServerOauth2Api.h"
#include "TutorialProject/TutorialProjectUtilities.h"

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
	FRegistry::ServerOauth2.LoginWithClientCredentials(FVoidHandler::CreateWeakLambda(this, [LoginWithCredentialsSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Login With Credentials success"));

		LoginWithCredentialsSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateWeakLambda(this, [LoginWithCredentialsFailed](int32 ErrorCode, const FString& ErrorMessage)
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
	FRegistry::ServerDSM.RegisterLocalServerToDSM(TutorialProjectUtilities::LocalServerIpAddress, TutorialProjectUtilities::LocalServerPort, TutorialProjectUtilities::LocalServerName, FVoidHandler::CreateWeakLambda(this, [RegisterLocalServerToDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Register Local Server to DSM success"));

		RegisterLocalServerToDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateWeakLambda(this, [RegisterLocalServerToDsmFailed](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Register Local Server to DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		RegisterLocalServerToDsmFailed.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::RegisterServerToDSM(const FRegisterServerToDSMSuccess& RegisterServerToDsmSuccess, const FSendFailedInfo& RegisterServerToDsmFailed)
{
	FRegistry::ServerDSM.RegisterServerToDSM(TutorialProjectUtilities::ServerPort, FVoidHandler::CreateWeakLambda(this, [RegisterServerToDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Register Server to DSM success"));

		RegisterServerToDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateWeakLambda(this, [RegisterServerToDsmFailed](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Register Server to DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		RegisterServerToDsmFailed.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::GetSessionId(const FGetSessionIdSuccess& GetSessionIdSuccess, const FSendFailedInfo& GetSessionIdError)
{
	FRegistry::ServerDSM.GetSessionId(THandler<FAccelByteModelsServerSessionResponse>::CreateWeakLambda(this, [GetSessionIdSuccess](const FAccelByteModelsServerSessionResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Get Session Id success"));

		GetSessionIdSuccess.ExecuteIfBound(Result);
	}), FErrorHandler::CreateWeakLambda(this, [GetSessionIdError](int32 ErrorCode, const FString& ErrorMessage)
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
	FRegistry::ServerMatchmaking.QuerySessionStatus(FRegistry::ServerCredentials.GetMatchId(), THandler<FAccelByteModelsMatchmakingResult>::CreateWeakLambda(this, [QuerySessionStatusSuccess](const FAccelByteModelsMatchmakingResult& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Query Session Status success"));

		QuerySessionStatusSuccess.ExecuteIfBound(Result);
	}), FErrorHandler::CreateWeakLambda(this, [QuerySessionStatusError](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Query Session Status error, Error Code: %d, Error Message: %s"), ErrorCode, *ErrorMessage);

		QuerySessionStatusError.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::DeregisterLocalServerFromDSM(const FDeregisterLocalServerFromDSMSuccess& DeregisterLocalServerFromDsmSuccess, const FSendFailedInfo& DeregisterLocalServerFromDsmError)
{
	FRegistry::ServerDSM.DeregisterLocalServerFromDSM(TutorialProjectUtilities::LocalServerName, FVoidHandler::CreateWeakLambda(this, [DeregisterLocalServerFromDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Deregister Local Server from DSM success"))

		DeregisterLocalServerFromDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateWeakLambda(this, [DeregisterLocalServerFromDsmError](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Deregister Local Server from DSM error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		DeregisterLocalServerFromDsmError.ExecuteIfBound(ErrorCode, ErrorMessage);
	}));
}

void UAccelByteServerManager::SendShutdownToDSM(const FSendShutDownToDSMSuccess& SendShutDownToDsmSuccess,	const FSendFailedInfo& SendShutdownToDsmError)
{
	// true = to shutdown the DS, false = if the DS has already shutdown
	FRegistry::ServerDSM.SendShutdownToDSM(true, FRegistry::ServerCredentials.GetMatchId(), FVoidHandler::CreateWeakLambda(this, [SendShutDownToDsmSuccess]()
	{
		UE_LOG(LogTemp, Log, TEXT("Send Shutdown to DSM success"));

		SendShutDownToDsmSuccess.ExecuteIfBound();
	}), FErrorHandler::CreateWeakLambda(this, [SendShutdownToDsmError](int32 ErrorCode, const FString& ErrorMessage)
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

void UAccelByteServerManager::ShutDown()
{
	if (CheckCommandLineArgument(TutorialProjectUtilities::LaunchArgsLocalDS))
	{
		DeregisterLocalServerFromDSM(FDeregisterLocalServerFromDSMSuccess::CreateWeakLambda(this, []()
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully DeregisterLocalServerFromDSM"));
		}),
		FSendFailedInfo::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Error DeregisterLocalServerFromDSM : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
		}));
	}
	else
	{
		SendShutdownToDSM(FSendShutDownToDSMSuccess::CreateWeakLambda(this, []()
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully DeregisterLocalServerFromDSM"));
		}),
		FSendFailedInfo::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Error DeregisterLocalServerFromDSM : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
		}));
	}

	ForgetAllCredentials();
}

void UAccelByteServerManager::GameServerLogin()
{
	LoginWithCredentials(FLoginWithCredentialsSuccess::CreateWeakLambda(this, [this]()
		{
			if (CheckCommandLineArgument(TutorialProjectUtilities::LaunchArgsLocalDS))
			{
				RegisterLocalServerToDSM(FRegisterLocalServerToDSMSuccess::CreateWeakLambda(this, []()
					{
						UE_LOG(LogTemp, Log, TEXT("Local Server Running"));
					}),
					FSendFailedInfo::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
						{
							UE_LOG(LogTemp, Error, TEXT("Local Server Failed Error %i Message %s"), ErrorCode, *ErrorMessage);
						}));
			}
			else
			{
				RegisterServerToDSM(FRegisterServerToDSMSuccess::CreateWeakLambda(this, []()
					{
						UE_LOG(LogTemp, Log, TEXT("Successfully Registered Server to DSM"));
					}),
					FSendFailedInfo::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
						{
							UE_LOG(LogTemp, Error, TEXT("Error RegisterServerToDSM : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
						}));
			}
		}),
		FSendFailedInfo::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Error LoginWithCredentials : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
			}));
}
