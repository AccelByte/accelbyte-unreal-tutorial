// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutorialProjectGameInstance.h"

#include "TutorialProjectUtilities.h"
#include "Api/AccelByteLobbyApi.h"
#include "Core/AccelByteRegistry.h"

void UTutorialProjectGameInstance::Init()
{
	Super::Init();

	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Log, TEXT("Check If Running on DS"));
		
		GameServerLogin();
	}
}

void UTutorialProjectGameInstance::Shutdown()
{
	if (FRegistry::Lobby.IsConnected())
	{
		FRegistry::Lobby.SendLeavePartyRequest();
	}
	
	if (IsRunningDedicatedServer())
	{
		ServerManager->ForgetAllCredentials();
		
		if (ServerManager->CheckCommandLineArgument(TutorialProjectUtilities::LaunchArgsLocalDS))
		{
			ServerManager->DeregisterLocalServerFromDSM(FDeregisterLocalServerFromDSMSuccess::CreateLambda([]()
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully DeregisterLocalServerFromDSM"));
			}),
			FSendFailedInfo::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Error DeregisterLocalServerFromDSM : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
			}));
		}
		else
		{
			ServerManager->SendShutdownToDSM(FSendShutDownToDSMSuccess::CreateLambda([]()
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully DeregisterLocalServerFromDSM"));
			}),
			FSendFailedInfo::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Error DeregisterLocalServerFromDSM : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
			}));
		}
	}
	
	Super::Shutdown();
}

void UTutorialProjectGameInstance::GameServerLogin()
{
	ServerManager = UAccelByteServerManager::CreateAccelByteServer(this);
	
	ServerManager->LoginWithCredentials(FLoginWithCredentialsSuccess::CreateLambda([this]()
	{
		if (ServerManager->CheckCommandLineArgument(TutorialProjectUtilities::LaunchArgsLocalDS))
		{
			ServerManager->RegisterLocalServerToDSM(FRegisterLocalServerToDSMSuccess::CreateLambda([]()
			{
				UE_LOG(LogTemp, Log, TEXT("Local Server Running"));
			}),
				FSendFailedInfo::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Local Server Failed Error %i Message %s"), ErrorCode, *ErrorMessage);
			}));
		}
		else
		{
			ServerManager->RegisterServerToDSM(FRegisterServerToDSMSuccess::CreateLambda([]()
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully Registered Server to DSM"));
			}),
			FSendFailedInfo::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Error RegisterServerToDSM : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
			}));
		}
	}),
	FSendFailedInfo::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Error LoginWithCredentials : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
	}));
}
