// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteLeaderboardRankingEntry.generated.h"

class UTextBlock;
class UBorder;

/**
 * 
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteLeaderboardRankingEntry : public UUserWidget
{
	GENERATED_BODY()

private:

	static constexpr FLinearColor OddColor = FLinearColor(0.02f, 0.02f, 0.02f, 1.0f);
	static constexpr FLinearColor EvenColor = FLinearColor(0.01f, 0.01f, 0.01f, 1.0f);
	static constexpr FLinearColor PlayerColor = FLinearColor(0.04f, 0.3f, 0.8f, 1.0f);

#pragma region UI Binding

	/**
	 * @brief TextBox to show rank
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Rank;

	/**
	 * @brief TextBox to show current rank's display name
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_DisplayName;

	/**
	 * @brief TextBox to show current rank's point
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Point;

	/**
	 * @brief Border to change current rank's entry background color
	 */
	UPROPERTY(meta = (BindWidget))
	UBorder* B_RankBackground;
	
#pragma endregion

public:

	/**
	 * @brief Initialize ranking entry data
	 */
	void InitData(const FText& Rank, const FText& DisplayName, const FText& Point, bool bIsPlayer = false, bool bIsEven = false) const;
};
