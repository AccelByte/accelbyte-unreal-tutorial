// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialProject/TutPlayerControllerCountGame.h"
#include "AccelByteScoreboardEntry.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteScoreboardEntry : public UUserWidget
{
	GENERATED_BODY()	

protected:

	virtual void NativeConstruct() override;

	/**
	* @brief Text Block that displays the user's username
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Username;

	/**
	* @brief Text Block that displays the user's score
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_UserScore;

	/**
	* @brief Integer that store this entry's User Score.
	*/
	UPROPERTY()
	int EntryUserScore = 0;

public:
	
	/**
	* @brief Functionality to Initialize this entry's data.
	*/
	UFUNCTION()
	void InitData(const FPlayerData& CurrentPlayerData);
	
	/**
	* @brief Functionality to set this entry's content.
	*/
	UFUNCTION()
	void SetScore(const int& UserScore);
};
