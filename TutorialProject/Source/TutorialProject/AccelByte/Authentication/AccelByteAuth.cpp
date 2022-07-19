// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAuth.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
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

	// Check Authorization Code of Justice Launcher
	const FString AuthorizationCode = FPlatformMisc::GetEnvironmentVariable(TEXT("JUSTICE_AUTHORIZATION_CODE"));
	if (!AuthorizationCode.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("Login with Launcher"));

		LauncherLogin();
	}
	else if (FParse::Param(FCommandLine::Get(), *TutorialProjectUtilities::LaunchArgsSteam))
	{
		UE_LOG(LogTemp, Log, TEXT("Login with Steam"));

		SteamLogin();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Login with Username"));

		T_LoginStatus->SetText(FText::FromString("Please Login"));
		Btn_Login->OnClicked.AddUniqueDynamic(this, &UAccelByteAuth::OnLoginButtonClicked);
	}
}

void UAccelByteAuth::OnLoginButtonClicked()
{
	T_LoginStatus->SetText(FText::FromString("Logging in..."));
	
	FRegistry::User.LoginWithUsername(
		Etb_Username->GetText().ToString(),
		Etb_Password->GetText().ToString(),
		FVoidHandler::CreateUObject(this, &UAccelByteAuth::LoginSuccess),
		FCustomErrorHandler::CreateUObject(this, &UAccelByteAuth::LoginFailed));
}

void UAccelByteAuth::OnLogoutButtonClicked()
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
	UE_LOG(LogTemp, Log, TEXT("Login Success"));
	T_LoginStatus->SetText(FText::FromString("Login successful"));
	
	if(GetOwningPlayer() && GetOwningPlayer()->GetHUD())
	{
		TutorialMenuHUD->InitLegal();
		this->RemoveFromParent();
	}
}

void UAccelByteAuth::LoginFailed(int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
{
	UE_LOG(LogTemp, Error, TEXT("Login Failed : %d , %s"), ErrorCode, *ErrorMessage);
	T_LoginStatus->SetText(FText::FromString(FString::Printf(TEXT("Login Failed : %d , %s"), ErrorCode, *ErrorMessage)));
}

void UAccelByteAuth::LogoutSuccess()
{
	UE_LOG(LogTemp, Log, TEXT("Logout Success"));
	
	if (TutorialMenuHUD == nullptr)
	{
		TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	}

	ensure(TutorialMenuHUD != nullptr);
	TutorialMenuHUD->GetPartyMenu()->ResetPartyInfo();
	TutorialMenuHUD->CloseMainMenu();
}

void UAccelByteAuth::LogoutFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Logout Failed : %d , %s"), ErrorCode, *ErrorMessage);
}

void UAccelByteAuth::LauncherLogin()
{
	FRegistry::User.LoginWithLauncher(FVoidHandler::CreateWeakLambda(this, [this]()
	{
		UE_LOG(LogTemp, Log, TEXT("Login with Launcher Success"));

		TutorialMenuHUD->InitLegal();
		this->RemoveFromParent();
	}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Login with Launcher Failed, ErrorCode: %d ErrorMessage: %s"), ErrorCode, *ErrorMessage);
	}));
}

void UAccelByteAuth::SteamLogin()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
	if (OSS)
	{
		OSS->GetIdentityInterface()->AddOnLoginCompleteDelegate_Handle(0, FOnLoginCompleteDelegate::CreateWeakLambda(this, [this](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Success? %s Error: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"), *Error);

			IOnlineSubsystem* OSS = IOnlineSubsystem::Get(STEAM_SUBSYSTEM);
			
			if (bWasSuccessful)
			{
				if (OSS)
				{
					UE_LOG(LogTemp, Log, TEXT("Login with Steam Success"));
					
					FString SteamAuthTicket = OSS->GetIdentityInterface()->GetAuthToken(0);
					FString PlayerNickName = OSS->GetIdentityInterface()->GetPlayerNickname(0);

					FRegistry::User.LoginWithOtherPlatform(EAccelBytePlatformType::Steam, SteamAuthTicket, FVoidHandler::CreateWeakLambda(this, [this]()
					{
						UE_LOG(LogTemp, Log, TEXT("Login with Other Platform Success"));
						
						TutorialMenuHUD->InitLegal();
						this->RemoveFromParent();
					}), FCustomErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson)
					{
						UE_LOG(LogTemp, Error, TEXT("Login with Other Platform Failed, ErrorCode: %d ErrorMessage: %s"), ErrorCode, *ErrorMessage);
					}));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to load Online Subsystem Steam"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Error Login with Steam, Error Message: %s"), *Error);
			}
		}));
		
		UE_LOG(LogTemp, Log, TEXT("Logging into Steam"));

		const FOnlineAccountCredentials NewAccount;
		OSS->GetIdentityInterface()->Login(0, NewAccount);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed Initialized Online Subsystem Steam"));
	}
}