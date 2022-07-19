// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteStore.generated.h"

class ATutorialMenuHUD;
class UAccelByteStoreCategory;
class UAccelByteStoreItemEntry;
class UAccelByteWallet;

class UTextBlock;
class UButton;
class UScrollBox;
class UUniformGridPanel;
class UOverlay;

/**
 * Part of the AccelByte Store SDK Implementation
 * The Store Handler to display the Store Menu and the items available at the Store.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteStore : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	/**
	* @brief Button to go back to Main Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

	/**
	* @brief Button to show all items, regardless of category.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_AllCategory;
	
	/**
	* @brief Uniform Grid Panel to display store items.
	*/
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* UGP_StoreGridPanel;

	/**
	* @brief Horizontal Box to contain Store Category widgets.
	*/
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HB_StoreCategories;

	/**
	* @brief Overlay to contain Wallet widget.
	*/
	UPROPERTY(meta = (BindWidget))
	UAccelByteWallet* WB_Wallet;

	/**
	* @brief Button Click Event Function to go back to Main Menu.
	*/
	UFUNCTION()
	void OnClickBackToMainMenu();

	/**
	 * @brief Button Click Event Function to call LoadItems() with an empty parameter.
	 */
	UFUNCTION()
	void OnClickAllCategory();

	/**
	* @brief Function to populate the GridPanel with Store Item Entries.
	*/
	void PopulateGridPanel(const FAccelByteModelsItemPagingSlicedResult& Result);

	/**
	* @brief Function to Initialize Category Buttons.
	*/
	void InitCategoryButtons(const TArray<FAccelByteModelsCategoryInfo>& CategoryResult);

	/**
	 * @brief Tutorial Menu HUD pointer reference.
	 */
	UPROPERTY()
	ATutorialMenuHUD* TutorialMenuHUD;

public:
	/**
	 * @brief Reference to Store Category widget class.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteStoreCategory> AccelByteStoreCategoryClass;

	/**
	 * @brief Function to Load Store Items with a specific Category Path.
	 */
	void LoadItems(const FString& StoreCategoryPath);

	/**
	 * @brief Function to update the wallet widget.
	 */
	void UpdateWallet() const;

	/**
	 * @brief Default Maximum Column for UGP_StoreGridPanel.
	 */
	uint8 MaxItemColumn = 2;

private:
	/**
	 * @brief Reference to Store Item Entry class to spawn inside UGP_StoreGridPanel.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteStoreItemEntry> StoreItemEntryClass;
};
