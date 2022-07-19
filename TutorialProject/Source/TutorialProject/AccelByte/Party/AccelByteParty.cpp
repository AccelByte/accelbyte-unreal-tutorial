// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteParty.h"
#include "AccelBytePartyPlayerEntry.h"
#include "Api/AccelByteLobbyApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"

void UAccelByteParty::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	AccelByteLobby = TutorialMenuHUD->GetLobbyMenu();

	Btn_CreateParty->OnClicked.AddUniqueDynamic(this, &UAccelByteParty::OnClickedCreateParty);
	Btn_LeaveParty->OnClicked.AddUniqueDynamic(this, &UAccelByteParty::OnClickedLeaveParty);

	TotalPartyMember = TutorialProjectUtilities::DefaultPartyMemberCount;

	GetCurrentPartyInfo();

	SetNotificationDelegate();
}

void UAccelByteParty::OnClickedCreateParty()
{
	FRegistry::Lobby.SendCreatePartyRequest();
}

void UAccelByteParty::OnClickedLeaveParty()
{
	FRegistry::Lobby.SendLeavePartyRequest();
}

void UAccelByteParty::OnClickedInviteParty(const FString& UserId)
{
	FRegistry::Lobby.SendInviteToPartyRequest(UserId);
}

void UAccelByteParty::SetNotificationDelegate()
{
	FRegistry::Lobby.SetCreatePartyResponseDelegate(Api::Lobby::FPartyCreateResponse::CreateUObject(this, &UAccelByteParty::OnCreatePartyResponse),
		FErrorHandler::CreateUObject(this, &UAccelByteParty::OnCreatePartyFailed));

	FRegistry::Lobby.SetLeavePartyResponseDelegate(Api::Lobby::FPartyLeaveResponse::CreateUObject(this, &UAccelByteParty::OnLeavePartyResponse),
		FErrorHandler::CreateUObject(this, &UAccelByteParty::OnLeavePartyFailed));

	FRegistry::Lobby.SetInvitePartyResponseDelegate(Api::Lobby::FPartyInviteResponse::CreateUObject(this, &UAccelByteParty::OnInvitePartyResponse),
		FErrorHandler::CreateUObject(this, &UAccelByteParty::OnInvitePartyFailed));

	FRegistry::Lobby.SetPartyGetInvitedNotifDelegate(Api::Lobby::FPartyGetInvitedNotif::CreateUObject(this, &UAccelByteParty::OnInvitePartyGetInviteNotice));
	FRegistry::Lobby.SetPartyKickNotifDelegate(Api::Lobby::FPartyKickNotif::CreateUObject(this, &UAccelByteParty::OnInvitePartyKickedNotice));
	FRegistry::Lobby.SetPartyLeaveNotifDelegate(Api::Lobby::FPartyLeaveNotif::CreateUObject(this, &UAccelByteParty::OnLeavePartyNotice));
	FRegistry::Lobby.SetPartyDataUpdateNotifDelegate(Api::Lobby::FPartyDataUpdateNotif::CreateUObject(this, &UAccelByteParty::OnPartyDataUpdateResponse));
}

void UAccelByteParty::RefreshPartyList()
{
	Sb_Player->ClearChildren();
	PartyPlayerEntryWidgetArray.Empty();

	if (PartyInfo.PartyId.IsEmpty())
	{
		return;
	}

	if (!bIsInParty)
	{
		bIsInParty = true;
		TutorialMenuHUD->GetMatchmakingMenu()->SetMatchmakingNotification();
	}
	
	for (int i = 0; i < TotalPartyMember; ++i)
	{
		TWeakObjectPtr<UAccelBytePartyPlayerEntry> PlayerEntry = MakeWeakObjectPtr<UAccelBytePartyPlayerEntry>(CreateWidget<UAccelBytePartyPlayerEntry>(this, PartyPlayerEntryClass.Get()));

		PlayerEntry->EnableAllComponents(false);

		PartyPlayerEntryWidgetArray.Add(PlayerEntry);

		if (i < PartyInfo.Members.Num())
		{
			PlayerEntry->InitData(PartyInfo.Members[i]);
		}

		Sb_Player->AddChild(PlayerEntry.Get());
	}

	if (PartyInfo.Leader == FRegistry::Credentials.GetUserId())
	{
		CheckTotalMemberGameMode();
	}
	else
	{
		AccelByteLobby->SetFindMatchEnabled(false);
	}

	Tb_PartyID->SetText(FText::FromString(PartyInfo.PartyId));

	SetCreatePartyButtonEnabled(false);
	SetUpdateLeaderIcon(PartyInfo.Leader);
}

void UAccelByteParty::RefreshPartyEntries()
{
	Tb_PartyID->SetText(FText::FromString("##############"));
	SetCreatePartyButtonEnabled(true);
	RefreshPartyList();
	AccelByteLobby->DisableMatchmakingComponents();
}

void UAccelByteParty::SetUpdateLeaderIcon(const FString& LeaderId) const
{
	for (const TWeakObjectPtr<UAccelBytePartyPlayerEntry>& PlayerEntry : PartyPlayerEntryWidgetArray)
	{
		// Leader Entry
		if (LeaderId == PlayerEntry->GetUserData().UserId)
		{
			PlayerEntry->SetImageIconLeader(true);
			PlayerEntry->EnableAllComponents(false);
		}
		// Other entries when local player is leader
		else if (LeaderId == FRegistry::Credentials.GetUserId())
		{
			PlayerEntry->SetImageIconLeader(false);

			if (PlayerEntry->GetUserData().UserId.IsEmpty())
			{
				PlayerEntry->EnableAllComponents(false);
			}
			else
			{
				PlayerEntry->EnableAllComponents(true);
			}
		}
		// Other entries when local player is not leader
		else
		{
			PlayerEntry->SetImageIconLeader(false);
			PlayerEntry->EnableAllComponents(false);
		}
	}

	if (LeaderId == FRegistry::Credentials.GetUserId())
	{
		AccelByteLobby->SetGameModeComboBoxEnabled(true);
	}
	else
	{
		AccelByteLobby->DisableMatchmakingComponents();
	}
}

void UAccelByteParty::SetCreatePartyButtonEnabled(bool bIsCreateParty) const
{
	Btn_CreateParty->SetIsEnabled(bIsCreateParty);
	Btn_LeaveParty->SetIsEnabled(!bIsCreateParty);
}

void UAccelByteParty::CheckTotalMemberGameMode() const
{
	if (PartyInfo.Leader == FRegistry::Credentials.GetUserId())
	{
		const int MaximumPartyMember = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->GetLobbyMenu()->GetMaximumPartyMemberFromGameMode();
		if (PartyInfo.Members.Num() > MaximumPartyMember)
		{
			AccelByteLobby->SetFindMatchEnabled(false);
			AccelByteLobby->ShowErrorFindMatch(PartyInfo.Members.Num() - MaximumPartyMember);
		}
		else
		{
			AccelByteLobby->SetFindMatchEnabled(true);
			AccelByteLobby->RemoveErrorFindMatch();
		}
	}
}

void UAccelByteParty::ResetPartyInfo()
{
	const FAccelByteModelsPartyDataNotif EmptyData;
	PartyInfo = EmptyData;
	bIsInParty = false;
	
	RefreshPartyEntries();

	TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Party);
	TutorialMenuHUD->GetChatMenu()->SwitchChatTab(EChatTabType::Global);
		
	FRegistry::Lobby.UnbindMatchmakingNotifEvents();
}

void UAccelByteParty::GetCurrentPartyInfo()
{
	if (FRegistry::Lobby.IsConnected())
	{
		FRegistry::Lobby.SendInfoPartyRequest(Api::Lobby::FPartyInfoResponse::CreateWeakLambda(this, [this](const FAccelByteModelsInfoPartyResponse& Result)
		{
			UE_LOG(LogTemp, Log, TEXT("Get information about current party success"));

			PartyInfo.PartyId = Result.PartyId;
			PartyInfo.Members = Result.Members;
			PartyInfo.Leader = Result.LeaderId;
			RefreshPartyEntries();

			// make sure to show a party chat tab
			TutorialMenuHUD->GetChatMenu()->CreateTabButtonWidget(EChatTabType::Party);
		}), 
		FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Get information about current party failed, ErrorCode: %d ErrorMessage: %s"), ErrorCode, *ErrorMessage);
					
			RefreshPartyEntries();
		}));
	}
}

void UAccelByteParty::OnCreatePartyResponse(const FAccelByteModelsCreatePartyResponse& Result)
{
	UE_LOG(LogTemp, Log, TEXT("Create Party Success!"));

	TutorialMenuHUD->GetChatMenu()->CreateTabButtonWidget(EChatTabType::Party);
}

void UAccelByteParty::OnCreatePartyFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Create Party Failed : %d , %s"), ErrorCode, *ErrorMessage);
}

void UAccelByteParty::OnLeavePartyResponse(const FAccelByteModelsLeavePartyResponse& Result)
{
	UE_LOG(LogTemp, Log, TEXT("Leave Party Success!"));

	ResetPartyInfo();
}

void UAccelByteParty::OnLeavePartyFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Leave Party Failed : %d , %s"), ErrorCode, *ErrorMessage);
}

void UAccelByteParty::OnInvitePartyResponse(const FAccelByteModelsPartyInviteResponse& Result)
{
	UE_LOG(LogTemp, Log, TEXT("Invite Party Success!"));
}

void UAccelByteParty::OnInvitePartyFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT("Invite Party Failed : %d , %s"), ErrorCode, *ErrorMessage);
}

void UAccelByteParty::OnInvitePartyGetInviteNotice(const FAccelByteModelsPartyGetInvitedNotice& Result)
{
	UE_LOG(LogTemp, Log, TEXT("On Get Invite to Party Notice!"));

	const TWeakObjectPtr<UAccelBytePartyInvitationPopUp> InvitationPopUpWidget = MakeWeakObjectPtr<UAccelBytePartyInvitationPopUp>(
		CreateWidget<UAccelBytePartyInvitationPopUp>(this, PartyInvitationPopUpClass.Get()));

	InvitationPopUpWidget.Get()->InitData(Result);
	InvitationPopUpWidget->AddToViewport();
}

void UAccelByteParty::OnInvitePartyKickedNotice(const FAccelByteModelsGotKickedFromPartyNotice& Result)
{
	UE_LOG(LogTemp, Log, TEXT("You have been kicked from the party!"));

	ResetPartyInfo();
}

void UAccelByteParty::OnLeavePartyNotice(const FAccelByteModelsLeavePartyNotice& Result)
{
	UE_LOG(LogTemp, Log, TEXT("On Some Player Leaved Notice!"));
}

void UAccelByteParty::OnPartyDataUpdateResponse(const FAccelByteModelsPartyDataNotif& Result)
{
	UE_LOG(LogTemp, Log, TEXT("On Party Data Update Response!"));

	PartyInfo = Result;

	RefreshPartyList();
}