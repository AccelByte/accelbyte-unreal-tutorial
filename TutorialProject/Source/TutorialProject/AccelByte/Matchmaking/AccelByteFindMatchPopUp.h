// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteFindMatchPopUp.generated.h"

struct FAccelByteModelsMatchmakingResponse;

class UButton;

/** Find Match Window Pop Up, when match start searched this window will pop up.
* This code covers AccelByte services including :
*
* - Cancel Matchmaking
*/
UCLASS()
class TUTORIALPROJECT_API UAccelByteFindMatchPopUp : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	/**
	* @brief Button for cancel matchmaking.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CancelMatch;

	UFUNCTION()
	/**
	* @brief Button for cancel matchmaking.
	*/
	void CancelMatch();
	
	/**
	* @brief Callback when Canceling matchmaking success.
	*/
	void OnMatchmakingCancelSuccess(const FAccelByteModelsMatchmakingResponse& Result);
	
	/**
	* @brief Callback when Canceling matchmaking failed.
	*/
	void OnMatchmakingCancelFailed(const int32 code, const FString& message);

public:
	
	/**
	 * @brief Set current cancel match button visibility. this will handle if the leader is wanted to cancel the matchmaking.
	 */
	void SetCancelMatchButtonVisibility() const;
};
