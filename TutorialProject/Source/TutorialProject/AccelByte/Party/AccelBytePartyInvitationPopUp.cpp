// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelBytePartyInvitationPopUp.h"

#include "AccelByteParty.h"
#include "Api/AccelByteLobbyApi.h"
#include "Components/Button.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChat.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChatTab.h"

#pragma region Initialization

void UAccelBytePartyInvitationPopUp::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_AcceptParty->SetIsEnabled(true);
	Btn_RejectParty->SetIsEnabled(true);
	
	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	
	Btn_AcceptParty->OnClicked.AddUniqueDynamic(this, &UAccelBytePartyInvitationPopUp::OnClickedAcceptPartyInvitation);
	Btn_RejectParty->OnClicked.AddUniqueDynamic(this, &UAccelBytePartyInvitationPopUp::OnClickedRejectPartyInvitation);
}

void UAccelBytePartyInvitationPopUp::InitData(const FAccelByteModelsPartyGetInvitedNotice& Result)
{
	InvitationData = Result;
}

#pragma endregion

#pragma region Widget Callbacks

void UAccelBytePartyInvitationPopUp::OnClickedAcceptPartyInvitation()
{
	Btn_AcceptParty->SetIsEnabled(false);
	FRegistry::Lobby.SetInvitePartyJoinResponseDelegate(Api::Lobby::FPartyJoinResponse::CreateWeakLambda(this, [this](const FAccelByteModelsPartyJoinResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Accept Party Success! Member : %d"), Result.Members.Num());
		
		TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Party);
		TutorialMenuHUD->GetChatMenu()->CreateTabButtonWidget(EChatTabType::Party);
		
		RemoveFromParent();
	}),
	FErrorHandler::CreateWeakLambda(this, [this](const int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Accept Party Error! %d: %s"), ErrorCode, *ErrorMessage);
		
		TutorialMenuHUD->GetPartyMenu()->ResetPartyInfo();
		TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Party);
		
		RemoveFromParent();
	}));
	
	FRegistry::Lobby.SendAcceptInvitationRequest(InvitationData.PartyId, InvitationData.InvitationToken);
}

void UAccelBytePartyInvitationPopUp::OnClickedRejectPartyInvitation()
{
	Btn_RejectParty->SetIsEnabled(false);
	FRegistry::Lobby.SetInvitePartyRejectResponseDelegate(Api::Lobby::FPartyRejectResponse::CreateWeakLambda(this, [this](const FAccelByteModelsPartyRejectResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Reject Party Success!"));
		
		RemoveFromParent();
	}),
	FErrorHandler::CreateWeakLambda(this, [this](const int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Reject Party Error! %d: %s"), ErrorCode, *ErrorMessage);
		
		RemoveFromParent();
	}));
	
	FRegistry::Lobby.SendRejectInvitationRequest(InvitationData.PartyId, InvitationData.InvitationToken);
}

#pragma endregion