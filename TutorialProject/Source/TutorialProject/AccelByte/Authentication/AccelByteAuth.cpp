// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAuth.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectGameInstance.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"
#include "TutorialProject/AccelByte/Settings/AccelByteSettingsMenu.h"

void UAccelByteAuth::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	TutorialMenuHUD->GetSettingsMenu()->InitDefaultGameSettings();
	
	if (FParse::Param(FCommandLine::Get(), *TutorialProjectUtilities::LaunchArgsSteam))
	{
		SteamLogin();
	}
	else
	{
		CP_LoginMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Btn_Login->OnClicked.AddUniqueDynamic(this, &UAccelByteAuth::OnClickLoginButton);
	}
}

void UAccelByteAuth::OnClickLoginButton()
{
	FRegistry::User.LoginWithUsername(
		Etb_Username->GetText().ToString(),
		Etb_Password->GetText().ToString(),
		FVoidHandler::CreateUObject(this, &UAccelByteAuth::LoginSuccess),
		FErrorHandler::CreateUObject(this, &UAccelByteAuth::LoginFailed));
}

void UAccelByteAuth::OnClickLogoutButton()
{
	FRegistry::User.Logout(
		FVoidHandler::CreateUObject(this, &UAccelByteAuth::LogoutSuccess),
		FErrorHandler::CreateUObject(this, &UAccelByteAuth::LogoutFailed));
	
	if (FRegistry::Lobby.IsConnected())
	{
		FRegistry::Lobby.Disconnect();
	}
}

void UAccelByteAuth::LoginSuccess()
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString("Login Success"));
	
	if(GetOwningPlayer() && GetOwningPlayer()->GetHUD())
	{
		TutorialMenuHUD->InitLegal();
		this->RemoveFromParent();
	}
}

void UAccelByteAuth::LoginFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Login Failed : %d , %s"), ErrorCode, *ErrorMessage));
}

void UAccelByteAuth::LogoutSuccess()
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Logout Success")));
	
	check(TutorialMenuHUD != nullptr)
	TutorialMenuHUD->CloseMainMenu();
}

void UAccelByteAuth::LogoutFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Logout Failed : %d , %s"), ErrorCode, *ErrorMessage));
}

void UAccelByteAuth::SteamLogin()
{
	auto OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OSS)
	{
		OSS->GetIdentityInterface()->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateLambda([this](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Success? %s Error: %s"), *UKismetStringLibrary::Conv_BoolToString(bWasSuccessful), *Error));

			auto OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
			
			if (bWasSuccessful)
			{
				if (OSS)
				{
					TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Login with Steam Success"));
					
					FString SteamAuthTicket = OSS->GetIdentityInterface()->GetAuthToken(0);
					FString PlayerNickName = OSS->GetIdentityInterface()->GetPlayerNickname(0);

					FRegistry::User.LoginWithOtherPlatform(EAccelBytePlatformType::Steam, SteamAuthTicket, FVoidHandler::CreateLambda([this]()
					{
						TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Login with Other Platform Success"));
						
						TutorialMenuHUD->InitLegal();
						this->RemoveFromParent();
					}), FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
					{
						TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Login with Other Platform Failed, ErrorCode: %d ErrorMessage: %s"), ErrorCode, *ErrorMessage));
					}));
				}
				else
				{
					TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Failed to load Online Subsystem Steam"));
				}
			}
			else
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Error Login with Steam, Error Message: %s"), *Error));
			}
		}));

		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Logging into Steam"));
		
		FOnlineAccountCredentials NewAccount;
		OSS->GetIdentityInterface()->Login(0, NewAccount);
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Failed Initialized Online Subsystem Steam"));
	}
}