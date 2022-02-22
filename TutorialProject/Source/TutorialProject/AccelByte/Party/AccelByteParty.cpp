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
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteParty::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	AccelByteLobby = TutorialMenuHUD->GetLobbyMenu();

	Btn_CreateParty->OnClicked.AddUniqueDynamic(this, &UAccelByteParty::OnClickedCreateParty);
	Btn_LeaveParty->OnClicked.AddUniqueDynamic(this, &UAccelByteParty::OnClickedLeaveParty);

	TotalPartyMember = TutorialProjectUtilities::DefaultPartyMemberCount;

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
	FRegistry::Lobby.SetCreatePartyResponseDelegate(Lobby::FPartyCreateResponse::CreateUObject(this, &UAccelByteParty::OnCreatePartyResponse),
		FErrorHandler::CreateUObject(this, &UAccelByteParty::OnCreatePartyFailed));

	FRegistry::Lobby.SetLeavePartyResponseDelegate(Lobby::FPartyLeaveResponse::CreateUObject(this, &UAccelByteParty::OnLeavePartyResponse),
		FErrorHandler::CreateUObject(this, &UAccelByteParty::OnLeavePartyFailed));

	FRegistry::Lobby.SetInvitePartyResponseDelegate(Lobby::FPartyInviteResponse::CreateUObject(this, &UAccelByteParty::OnInvitePartyResponse),
		FErrorHandler::CreateUObject(this, &UAccelByteParty::OnInvitePartyFailed));

	FRegistry::Lobby.SetPartyGetInvitedNotifDelegate(Lobby::FPartyGetInvitedNotif::CreateUObject(this, &UAccelByteParty::OnInvitePartyGetInviteNotice));
	FRegistry::Lobby.SetPartyKickNotifDelegate(Lobby::FPartyKickNotif::CreateUObject(this, &UAccelByteParty::OnInvitePartyKickedNotice));
	FRegistry::Lobby.SetPartyLeaveNotifDelegate(Lobby::FPartyLeaveNotif::CreateUObject(this, &UAccelByteParty::OnLeavePartyNotice));
	FRegistry::Lobby.SetPartyDataUpdateResponseDelegate(Lobby::FPartyDataUpdateNotif::CreateUObject(this, &UAccelByteParty::OnPartyDataUpdateResponse));
}

void UAccelByteParty::RefreshPartyList()
{
	Sb_Player->ClearChildren();

	PartyPlayerEntryWidgetArray.Empty();

	if (PartyInfo.PartyId.IsEmpty())
	{
		return;
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

void UAccelByteParty::SetCreatePartyButtonEnabled(const bool& bIsCreateParty) const
{
	Btn_CreateParty->SetIsEnabled(bIsCreateParty);
	Btn_LeaveParty->SetIsEnabled(!bIsCreateParty);
}

void UAccelByteParty::CheckTotalMemberGameMode()
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

void UAccelByteParty::OnCreatePartyResponse(const FAccelByteModelsCreatePartyResponse& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Create Party Success!"));

	TutorialMenuHUD->GetChatMenu()->CreateTabButtonWidget(EChatTabType::Party);
}

void UAccelByteParty::OnCreatePartyFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Create Party Failed : %d , %s"), ErrorCode, *ErrorMessage));
}

void UAccelByteParty::OnLeavePartyResponse(const FAccelByteModelsLeavePartyResponse& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Leave Party Success!"));

	const FAccelByteModelsPartyDataNotif EmptyData;
	PartyInfo = EmptyData;

	RefreshPartyEntries();

	TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Party);
}

void UAccelByteParty::OnLeavePartyFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Leave Party Failed : %d , %s"), ErrorCode, *ErrorMessage));
}

void UAccelByteParty::OnInvitePartyResponse(const FAccelByteModelsPartyInviteResponse& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Invite Party Success!"));
}

void UAccelByteParty::OnInvitePartyFailed(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Invite Party Failed : %d , %s"), ErrorCode, *ErrorMessage));
}

void UAccelByteParty::OnInvitePartyGetInviteNotice(const FAccelByteModelsPartyGetInvitedNotice& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("On Get Invite to Party Notice!"));

	const TWeakObjectPtr<UAccelBytePartyInvitationPopUp> InvitationPopUpWidget = MakeWeakObjectPtr<UAccelBytePartyInvitationPopUp>(
		CreateWidget<UAccelBytePartyInvitationPopUp>(this, PartyInvitationPopUpClass.Get()));

	InvitationPopUpWidget.Get()->InitData(Result);
	InvitationPopUpWidget->AddToViewport();
}

void UAccelByteParty::OnInvitePartyKickedNotice(const FAccelByteModelsGotKickedFromPartyNotice& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("You have been kicked from the party!"));

	const FAccelByteModelsPartyDataNotif EmptyData;
	PartyInfo = EmptyData;

	RefreshPartyEntries();

	TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Party);
}

void UAccelByteParty::OnLeavePartyNotice(const FAccelByteModelsLeavePartyNotice& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("On Some Player Leaved Notice!"));
}

void UAccelByteParty::OnPartyDataUpdateResponse(const FAccelByteModelsPartyDataNotif& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("On Party Data Update Response!"));

	PartyInfo = Result;

	RefreshPartyList();
}