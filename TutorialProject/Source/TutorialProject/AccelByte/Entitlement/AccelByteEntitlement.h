#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialProject/AccelByte/Wallet/AccelByteWallet.h"
#include "AccelByteEntitlement.generated.h"

class ATutorialMenuHUD;
class UGridPanel;
class UWidgetSwitcher;
class UButton;
class UOverlay;

/**
* Component that are using to get the user or player entitlements. the player also can consume any item that are a consumable item.
*/

UCLASS()
class TUTORIALPROJECT_API UAccelByteEntitlement : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UGridPanel* GP_Entitlements;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_InventoryMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

	UPROPERTY(meta = (BindWidget))
	UAccelByteWallet* WB_Wallet;

	/**
	 * @brief This to handle reference of HUD framework
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Maximum Grid of the inventory Column
	 */
	UPROPERTY(EditDefaultsOnly) 
	int32 MaximumGridColumn;
	
public:
	
	/**
	 * @brief Refresh Inventory Menu whenever needed
	 */
	UFUNCTION()
	void RefreshInventoryMenu();
};
