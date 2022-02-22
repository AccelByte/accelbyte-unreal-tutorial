// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteFindMatchPopUp.h"
#include "AccelByteMatchmaking.h"
#include "Api/AccelByteLobbyApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/Button.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"

void UAccelByteFindMatchPopUp::NativeConstruct()
{
	Super::NativeConstruct();

	SetCancelMatchButtonVisibility();
	
	Btn_CancelMatch->OnClicked.AddUniqueDynamic(this, &UAccelByteFindMatchPopUp::CancelMatch);
}

void UAccelByteFindMatchPopUp::CancelMatch()
{
	FRegistry::Lobby.SetCancelMatchmakingResponseDelegate(Api::Lobby::FMatchmakingResponse::CreateUObject(this, &UAccelByteFindMatchPopUp::OnMatchmakingCancelSuccess),
		FErrorHandler::CreateUObject(this, &UAccelByteFindMatchPopUp::OnMatchmakingCancelFailed));

	FString GameMode = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->GetLobbyMenu()->GetGameModeComboBoxSelectedOption();
	FRegistry::Lobby.SendCancelMatchmaking(GameMode);
}

void UAccelByteFindMatchPopUp::OnMatchmakingCancelSuccess(const FAccelByteModelsMatchmakingResponse& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Cancel Matchmaking Success"));
}

void UAccelByteFindMatchPopUp::OnMatchmakingCancelFailed(const int32 Code, const FString& Message)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Failed to cancel matchmaking ! Code : %i , Message : %s"), Code, *Message));
}

void UAccelByteFindMatchPopUp::SetCancelMatchButtonVisibility()
{
	bool IsVisible = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->GetPartyMenu()->PartyInfo.Leader == FRegistry::Credentials.GetUserId();
	if (IsVisible)
	{
		Btn_CancelMatch->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Btn_CancelMatch->SetVisibility(ESlateVisibility::Collapsed);
	}
}
