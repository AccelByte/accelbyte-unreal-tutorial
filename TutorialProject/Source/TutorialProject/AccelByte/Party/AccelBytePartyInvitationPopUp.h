// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteLobbyModels.h"
#include "AccelBytePartyInvitationPopUp.generated.h"

class ATutorialMenuHUD;
class UTextBlock;
class UButton;

/**
* Pop up window when player get party invitation from friend.
* This code covers AccelByte services including :
*
* - Get User by User ID
* - Accept Party Invitation
* - Reject Party Invitation
 */

UCLASS()
class TUTORIALPROJECT_API UAccelBytePartyInvitationPopUp : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	/**
	* @brief Text Box displaying Invitation.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_InvitationText;
	
	/**
	* @brief Button for Accepting Party Invitation.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_AcceptParty;
	
	/**
	* @brief Button for Rejecting Party Invitation.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_RejectParty;
	
public:
	
	/**
	* @brief Initialize ResultInvited data to get player response to party host.
	*/
	UFUNCTION()
	void InitData(const FAccelByteModelsPartyGetInvitedNotice& Result);
	
	/**
	* @brief Callback for accepting party invitation.
	*/
	UFUNCTION()
	void OnClickedAcceptPartyInvitation();
	
	/**
	* @brief Callback for rejecting party invitation.
	*/
	UFUNCTION()
	void OnClickedRejectPartyInvitation();

	/**
	* @brief Response when Player Accept the party Invitation.
	*/
	void OnInvitePartyAcceptResponse(const FAccelByteModelsPartyJoinReponse& Result);
	
	/**
	* @brief Response when Reject the party invitation.
	*/
	void OnInvitePartyRejectResponse(const FAccelByteModelsPartyRejectResponse& Result);

	/**
	* @brief Get Invitation notice data to pass player accepting / rejecting
	* party invitation request.
	*/
	FAccelByteModelsPartyGetInvitedNotice InvitationData;

private:
	
	/**
	* @brief Tutorial Menu HUD to handle all functionality of the UI.
	*/
	ATutorialMenuHUD* TutorialMenuHUD;
};
