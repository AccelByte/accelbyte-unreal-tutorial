// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteScoreboardEntry.h"
#include "Components/TextBlock.h"

void UAccelByteScoreboardEntry::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAccelByteScoreboardEntry::InitData(const FPlayerData& CurrentPlayerData)
{
	T_Username->SetText(FText::FromString(CurrentPlayerData.DisplayName));
	SetScore(0);
}

void UAccelByteScoreboardEntry::SetScore(const int& UserScore)
{
	T_UserScore->SetText(FText::AsNumber(UserScore));
}
