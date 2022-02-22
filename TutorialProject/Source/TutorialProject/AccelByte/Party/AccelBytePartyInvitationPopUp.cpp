// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelBytePartyInvitationPopUp.h"
#include "Api/AccelByteLobbyApi.h"
#include "Components/Button.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChat.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChatTab.h"

void UAccelBytePartyInvitationPopUp::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	
	Btn_AcceptParty->OnClicked.AddUniqueDynamic(this, &UAccelBytePartyInvitationPopUp::OnClickedAcceptPartyInvitation);
	Btn_RejectParty->OnClicked.AddUniqueDynamic(this, &UAccelBytePartyInvitationPopUp::OnClickedRejectPartyInvitation);
}

void UAccelBytePartyInvitationPopUp::InitData(const FAccelByteModelsPartyGetInvitedNotice& Result)
{
	InvitationData = Result;
}

void UAccelBytePartyInvitationPopUp::OnClickedAcceptPartyInvitation()
{
	FRegistry::Lobby.SetInvitePartyJoinResponseDelegate(Api::Lobby::FPartyJoinResponse::CreateUObject(this, &UAccelBytePartyInvitationPopUp::OnInvitePartyAcceptResponse));
	FRegistry::Lobby.SendAcceptInvitationRequest(InvitationData.PartyId, InvitationData.InvitationToken);
}

void UAccelBytePartyInvitationPopUp::OnClickedRejectPartyInvitation()
{
	FRegistry::Lobby.SetInvitePartyRejectResponseDelegate(Api::Lobby::FPartyRejectResponse::CreateUObject(this, &UAccelBytePartyInvitationPopUp::OnInvitePartyRejectResponse));  
	FRegistry::Lobby.SendRejectInvitationRequest(InvitationData.PartyId, InvitationData.InvitationToken);
}

void UAccelBytePartyInvitationPopUp::OnInvitePartyAcceptResponse(const FAccelByteModelsPartyJoinReponse& Result)
{
	if (Result.Code == "0")  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Accept Party Success! Member : %d"), Result.Members.Num()));
		
		TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Party);
		TutorialMenuHUD->GetChatMenu()->CreateTabButtonWidget(EChatTabType::Party);
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Accept Party Error!")));
	}
	this->RemoveFromParent();
}

void UAccelBytePartyInvitationPopUp::OnInvitePartyRejectResponse(const FAccelByteModelsPartyRejectResponse& Result)
{
	if (Result.Code == "0")  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Reject Party Success!")));
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Reject Party Failed!")));
	}
	this->RemoveFromParent();
}
