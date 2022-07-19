// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteOrderPurchaseEntry.generated.h"

class UAccelByteStatisticObject;
class UAccelByteAchievement;
class UTextBlock;
class UButton;
class UAccelByteOrder;
class ATutorialMenuHUD;

/**
 * Purchase button widget entry for each currency
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteOrderPurchaseEntry : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

private:
	/**
	 * @brief Callback for purchase button
	 */
	UFUNCTION()
	void OnClickPurchaseItem();

	/**
	 * @brief Text Box for currency code on purhcase button
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_CurrencyCode;

	/**
	 * @brief Button to purchase item with specific currency
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Purchase;

	/**
	* @brief Reference to item info
	*/
	FAccelByteModelsItemInfo ItemInfo;

	/**
	* @brief Indicates which currency data to use
	*/
	int32 CurrencyIndex;
	
	/**
	* @brief Pointer to TutorialMenuHUD
	*/
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Pointer to AccelByteStatistic
	 */
	UPROPERTY()
	UAccelByteStatisticObject* AccelByteStatisticObject;

	/**
	 * @brief Pointer to AccelByteAchievement
	 */
	UPROPERTY()
	UAccelByteAchievement* AccelByteAchievement;

public:
	/**
	* @brief Widget component and variable setup
	*/
	void InitWidget(const FAccelByteModelsItemInfo& Item, int32 CurrencyIndex);
};
