// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteLobbyModels.h"
#include "Models/AccelByteUserModels.h"
#include "AccelBytePartyPlayerEntry.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
* Party Player entry. This will hold individual data searched from
* Party member.
* This code covers AccelByte services including :
*
* - Get User by User ID
* - Kick Player
* - Promote Leader
 */
UCLASS()
class TUTORIALPROJECT_API UAccelBytePartyPlayerEntry : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	
	/**
	* @brief Initialize User ID and converted to FSimpleUserData in here.
	*/
	void InitData(const FString& PlayerId);

	/**
	* @brief Enabling button Promote Leader and Kick Party member.
	* @param bEnable Enable all button components.
	*/
	void EnableAllComponents(const bool bEnable) const;
	
	/**
	* @brief Set Image Icon to Party Member.
	* @param IsLeader Checks if the player is the leader of the party.
	*/
	void SetImageIconLeader(const bool IsLeader) const;

	/**
	* @brief Get User Data stored in this entry.
	*/
	FSimpleUserData GetUserData() const;

private:
	
	/**
	* @brief Image to display leader status.
	*/
	UPROPERTY(meta = (BindWidget))
	UImage* Img_LeaderIcon;
	
	/**
	* @brief Text Box for Player Username.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_PlayerUsername;
	
	/**
	* @brief Button for Promoting Party Leader.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_PartyLeader;
	
	/**
	* @brief Button for Kick Party Member.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_KickPlayer;
	
	/**
	* @brief Callback for OnClicked Party Leader Button.
	*/
	UFUNCTION()
	void OnClickedPartyLeader();
	
	/**
	* @brief Callback for OnClicked Kick Party.
	*/
	UFUNCTION()
	void OnClickedKickParty();
	
	/**
	* @brief Response when party promoted to leader.
	*/
	void OnPartyPromoteLeaderResponse(const FAccelByteModelsPartyPromoteLeaderResponse& Result);
	
	/**
	* @brief Response when party kicked.
	*/
	void OnInvitePartyKickMemberResponse(const FAccelByteModelsKickPartyMemberResponse& Result);

	/**
	* @brief Callback for successfully get the user ID.
	*/
	void OnSuccessGetUserId(const FSimpleUserData& Data);
	
	/**
	* @brief Callback for failed getting user ID.
	*/
	void OnFailedGetUserId(int32 ErrorCode, const FString& ErrorMessage);

	/**
	* @brief Models to store User Data contained in this entry.
	*/
	FSimpleUserData UserData;
};
