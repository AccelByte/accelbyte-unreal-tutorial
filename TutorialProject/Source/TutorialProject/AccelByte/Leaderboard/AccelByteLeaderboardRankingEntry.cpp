// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteLeaderboardRankingEntry.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void UAccelByteLeaderboardRankingEntry::InitData(const FText& Rank, const FText& DisplayName, const FText& Point, bool bIsPlayer, bool bIsEven) const
{
	T_Rank->SetText(Rank);
	T_DisplayName->SetText(DisplayName);
	T_Point->SetText(Point);

	if (bIsPlayer)
	{
		B_RankBackground->SetBrushColor(PlayerColor);
	}
	else
	{
		B_RankBackground->SetBrushColor(bIsEven? EvenColor : OddColor);
	}
}
