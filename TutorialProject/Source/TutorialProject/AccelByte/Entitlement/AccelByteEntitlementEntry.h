#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteEntitlementEntry.generated.h"

class UTextBlock;
class USizeBox;
class UButton;
class UImage;
class UWidgetSwitcher;

/**
* Component that are using to get the entry of every item that user or player have.
*/

UCLASS()
class TUTORIALPROJECT_API UAccelByteEntitlementEntry : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_ItemDescription;

	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Consume;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Consume;

	UPROPERTY(meta = (BindWidget))
	UImage* I_ItemImage;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_EntitlementEntry;

	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_UseCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_UseCount;
	
	/**
	 * @brief On consume button is clicked.
	 */
	UFUNCTION()
	void OnConsumeButtonClicked();

	/**
	 * @brief This to handle the entitlement identity for a consumable items or any functionality that needs in the future.
	 */
	FString EntitlementIdentity;
	
public:
	/**
	 * @brief Init data when to show the display.
	 * @param Data The data of every entitlement that already got.
	 * @param DataCount Data count is the count of every entitlement that 
	 */
	void InitData(const FAccelByteModelsPopulatedItemInfo& Data, int32 DataCount, const FString& EntitlementId);
};
