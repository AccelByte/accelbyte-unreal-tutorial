// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteOrder.generated.h"

class ATutorialMenuHUD;
class UTextBlock;
class UButton;
class UHorizontalBox;
class UVerticalBox;
class UWidgetSwitcher;
class UImage;
class UCircularThrobber;
class UWebBrowser;

UENUM()
enum EOrderPopUpType
{
	LOADING = 0,
	PURCHASE_CONFIRMATION,
	PURCHASE_RESULT,
	WEB_BROWSER
};

UENUM()
enum EOrderPopUpResultType
{
	SUCCESS = 0,
	ERROR
};

/**
 * Order confirmation pop up controller
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteOrder : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

private:
	/**
	 * @brief Text Box for pop Up title
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_ConfirmTitle;

	/**
	 * @brief TextBox to show item's name
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_ItemName;

	/**
	 * @brief TextBox to show item's name in success window
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_SuccessItemName;

	/**
	 * @brief TextBox to show purchase error if there is any
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Error;

	/**
	 * @brief Horizontal box to hold prices list
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HB_PriceList;

	/**
	 * @brief Horizontal box to hold buy button list for each currency
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HB_PurchaseButtonList;

	/**
	 * @brief Vertical box to hold purchase confirmation panel
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_PurchaseConfirmation;

	/**
	 * @brief Vertical box to hold purchase result panel
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_PurchaseResult;

	/**
	 * @brief Vertical box hosting purchase success result panel
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_PurchaseResultSuccess;

	/**
	 * @brief Vertical box hosting purchase error result panel
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_PurchaseResultError;

	/**
	 * @brief Button to close order confirmation pop up and cancel ordering
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Close;

	/**
	 * @brief Button to close order confirmation pop up
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Back;

	/**
	 * @brief Widget Switcher to show loading, purchase confirmation, and order status
	 */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_ParentPanel;

	/**
	 * @brief Widget Switcher to show purchase result
	 */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_Result;

	/**
	 * @brief Show item's image
	 */
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Item;

	/**
	 * @brief Loading panel
	 */
	UPROPERTY(meta = (BindWidget))
	UCircularThrobber* CT_Loading;
	
	/**
	 * @brief In-game browser for real money payment gateway
	 */
	UPROPERTY(meta = (BindWidget))
	UWebBrowser* WBrowser_Payment;

	/**
	 * @brief Pointer to TutorialMenuHUD
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Timer handle to check payment status
	 */
	FTimerHandle OrderStatusCheckTimer;

public:
	/**
	 * @brief Initialize item data and purhase pop up widget entries
	 */
	void InitPopUp(const FAccelByteModelsItemInfo& ItemInfo);

	/**
	 * @brief Change Ws_ParentPanel index
	 */
	void SwitchPopUpWidget(const EOrderPopUpType& Index) const;

	/**
	 * @brief On Order failed callback
	 */
	void OnPurchaseFailed(const FString& ErrorMessage) const;

	/**
	 * @brief Open browser and access payment URL
	 */
	void LoadPaymentGateway(const FString& Url, const FString& OrderNo);

	/**
	 * @brief On close popup callback
	 */
	UFUNCTION()
	void OnClosePopup();
};
