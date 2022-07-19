// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "AccelByteMatchmakingPlayerEntry.generated.h"

class UImage;
class UTextBlock;

/**
* Matchmaking Player entry. This will hold individual data searched from
* Matchmaking Team Party member.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteMatchmakingPlayerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
	/**
	 * @brief This will handle every time the object is listed to the list view widget component
	 * @param InObject The object that are currently created to the list, in this case will bi AccelByteMatchmakingPlayerEntry object
	 */
	virtual void NativeOnListItemObjectSet(UObject* InObject) override;

	/**
	* @brief Text block to display player name.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_PlayerUsername;

	/**
	 * @brief Image to get the ability property widget
	 */
	UPROPERTY(meta = (BindWidget))
	UImage* I_Ability;

	/**
	* @brief Image to get the badge property widget
	*/
	UPROPERTY(meta = (BindWidget))
	UImage* I_Badge;

public:
	
	/**
	 * @brief This will handle the current player entry Display Name
	 */
	FString DisplayName;

	/**
	 * @brief User Id to get the player cloud save equipment
	 */
	FString UserId;
};
