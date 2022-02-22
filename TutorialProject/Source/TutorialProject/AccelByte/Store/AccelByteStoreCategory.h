// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteStoreCategory.generated.h"

class ATutorialMenuHUD;
class UAccelByteStore;
struct FAccelByteModelsCategoryInfo;
class UButton;
class UTextBlock;

/**
 * Part of the AccelByte Store SDK Implementation
 * Store Category widget to change the category when calling FRegistry::Item.GetItemsByCriteria().
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteStoreCategory : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	/**
	 * @brief Button to show a specific category.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Category;

	/**
	 * @brief Text Block that shows the category's name.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_CategoryName;

	/**
	 * @brief Pointer to TutorialMenuHUD.
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Pointer to StoreMenu.
	 */
	UAccelByteStore* StoreMenuClass;
	
	/**
	 * @brief Locally saved Category Info Model.
	 */
	FAccelByteModelsCategoryInfo SavedCategoryInfo;

public:
	/**
	 * @brief Function to initialize data in category widget.
	 */
	void InitCategory(const FAccelByteModelsCategoryInfo& CategoryInfo);

	/**
	 * @brief Button Click Event Function that calls StoreMenu's LoadItems()
	 * with the saved Info's Category Path as parameter.
	 */
	UFUNCTION()
	void OnClickCategoryButton();
};
