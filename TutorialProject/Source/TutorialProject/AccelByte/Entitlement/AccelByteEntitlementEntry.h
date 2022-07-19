#pragma once

#include "CoreMinimal.h"
#include "AccelByteEntitlement.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteEntitlementEntry.generated.h"

class UCircularThrobber;
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
	UCircularThrobber* CT_Loading;

	UPROPERTY(meta = (BindWidget))
	UOverlay* O_Entitlement;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_ItemName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_ItemDescription;
	
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

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_ConsumeOrEquip;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_Consume;
	
	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Consume;

	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_InUsed;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_Equip;

	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Equip;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Equip;
	
	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Unequip;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Unequip;
	
	/**
	 * @brief On consume button is clicked.
	 */
	UFUNCTION()
	void OnConsumeButtonClicked();

	/**
	* @brief On equip button is clicked.
	*/
	UFUNCTION()
	void OnEquipButtonClicked();

	/**
	* @brief On unequip button is clicked.
	*/
	UFUNCTION()
	void OnUnequipButtonClicked();

	void UpdatePlayerEquipment(const FJsonObject& JsonObject, UButton* CurrentActiveButton);

	/**
	 * @brief This to handle the entitlement identity for a consumable items or any functionality that needs in the future.
	 */
	FString EntitlementIdentity;

	/**
	 * @brief Player struct equipment to compare with public cloud save json
	 */
	FPlayerEquipment PlayerEquipment;

	/**
	 * @brief Item Data to instantiate current item information
	 */
	FAccelByteModelsPopulatedItemInfo ItemData;

	/**
	* @brief Entitlement Data to instantiate current entitlement information
	*/
	FAccelByteModelsEntitlementInfo EntitlementData;
	
public:
	/**
	 * @brief Init data when to show the display.
	 * @param ItemInfo The item store data.
	 * @param EntitlementInfo The entitlement information.
	 * @param CurrentPlayerEquipment Player Equipment to manage the usable item
	 */
	void InitData(const FAccelByteModelsPopulatedItemInfo& ItemInfo, const FAccelByteModelsEntitlementInfo& EntitlementInfo, const FPlayerEquipment& CurrentPlayerEquipment);
};
