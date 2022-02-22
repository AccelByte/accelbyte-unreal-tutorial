// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelBytePartyPlayerEntry.h"
#include "AccelByteParty.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelBytePartyPlayerEntry::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_PartyLeader->OnClicked.AddUniqueDynamic(this, &UAccelBytePartyPlayerEntry::OnClickedPartyLeader);
	Btn_KickPlayer->OnClicked.AddUniqueDynamic(this, &UAccelBytePartyPlayerEntry::OnClickedKickParty);
}

void UAccelBytePartyPlayerEntry::InitData(const FString& PlayerId)
{
	UserData.UserId = PlayerId;

	FRegistry::User.GetUserByUserId(
		PlayerId,
		THandler<FSimpleUserData>::CreateUObject(this, &UAccelBytePartyPlayerEntry::OnSuccessGetUserId),
		FErrorHandler::CreateUObject(this, &UAccelBytePartyPlayerEntry::OnFailedGetUserId));
}

void UAccelBytePartyPlayerEntry::EnableAllComponents(const bool bEnable) const
{
	if (bEnable)
	{
		Btn_KickPlayer->SetVisibility(ESlateVisibility::Visible);
		Btn_PartyLeader->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Btn_KickPlayer->SetVisibility(ESlateVisibility::Hidden);
		Btn_PartyLeader->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UAccelBytePartyPlayerEntry::SetImageIconLeader(const bool IsLeader) const
{
	if (IsLeader)
	{
		Img_LeaderIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Img_LeaderIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

FSimpleUserData UAccelBytePartyPlayerEntry::GetUserData() const
{
	return UserData;
}

void UAccelBytePartyPlayerEntry::OnClickedPartyLeader()
{
	FRegistry::Lobby.SetPartyPromoteLeaderResponseDelegate(Api::Lobby::FPartyPromoteLeaderResponse::CreateUObject(this, &UAccelBytePartyPlayerEntry::OnPartyPromoteLeaderResponse));
	FRegistry::Lobby.SendPartyPromoteLeaderRequest(UserData.UserId);
}

void UAccelBytePartyPlayerEntry::OnClickedKickParty()
{
	FRegistry::Lobby.SetInvitePartyKickMemberResponseDelegate(Api::Lobby::FPartyKickResponse::CreateUObject(this, &UAccelBytePartyPlayerEntry::OnInvitePartyKickMemberResponse));
	FRegistry::Lobby.SendKickPartyMemberRequest(UserData.UserId);
}

void UAccelBytePartyPlayerEntry::OnPartyPromoteLeaderResponse(const FAccelByteModelsPartyPromoteLeaderResponse& Result)
{
	if (Result.Code == "0")
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Promote Party Leader Success!"));
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Promote Party Leader Failed!"));
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, "Unable to Promote Party Leader!");
	}
}

void UAccelBytePartyPlayerEntry::OnInvitePartyKickMemberResponse(const FAccelByteModelsKickPartyMemberResponse& Result)
{
	if (Result.Code == "0")
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Kick Party Member Success!"));
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Kick Party Member Failed!"));
	}
}

void UAccelBytePartyPlayerEntry::OnSuccessGetUserId(const FSimpleUserData& Data)
{
	UserData = Data;
	Tb_PlayerUsername->SetText(FText::FromString(UserData.DisplayName));
}

void UAccelBytePartyPlayerEntry::OnFailedGetUserId(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Get User Id Failed : %d , %s"), ErrorCode, *ErrorMessage));
}