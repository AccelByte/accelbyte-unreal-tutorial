// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteStoreItemEntry.generated.h"

class UTextBlock;
class UButton;
class UImage;
class ATutorialMenuHUD;
struct FAccelByteModelsItemInfo;

/**
 * Part of the AccelByte Store SDK Implementation
 * Entry widget to show individual items for sale at the Store.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteStoreItemEntry : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	/**
	* @brief Text Block to display the item name.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_ItemName;

	/**
	* @brief Text Block to display the item description.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_ItemDescription;

	/**
	* @brief Image to display item appearance.
	*/
	UPROPERTY(meta = (BindWidget))
	UImage* Img_ItemImage;

	/**
	* @brief Text Block to display the item's virtual currency price.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Price;

	/**
	* @brief Button to buy item.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BuyItem;

	/**
	* @brief Pointer to Tutorial Menu HUD.
	*/
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	* @brief Struct that contains Item Info.
	*/
	FAccelByteModelsItemInfo ItemEntryInfo;

public:
	/**
	* @brief Functionality to load item info in entry widget.
	*/
	void LoadInfo(const FAccelByteModelsItemInfo& Info);

	/**
	* @brief On click Buy button event to purchase item.
	*/
	UFUNCTION()
	void OnClickedBuy();
};
