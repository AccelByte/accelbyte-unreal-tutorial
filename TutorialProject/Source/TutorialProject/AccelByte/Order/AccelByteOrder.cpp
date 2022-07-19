// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteOrder.h"
#include "AccelByteOrderPriceEntry.h"
#include "AccelByteOrderPurchaseEntry.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "Core/AccelByteRegistry.h"
#include "Api/AccelByteOrderApi.h"
#include "TutorialProject/AccelByte/Store/AccelByteStore.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"

#define PAYMENT_GATEWAY_TEXT "Payment Gateway"
#define PURCHASE_CONFIRMATION_TEXT "Purchase Confirmation"

void UAccelByteOrder::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	check(TutorialMenuHUD);
	
	Btn_Close->OnClicked.AddUniqueDynamic(this, &UAccelByteOrder::OnClosePopup);
	Btn_Back->OnClicked.AddUniqueDynamic(this, &UAccelByteOrder::OnClosePopup);
}

void UAccelByteOrder::InitPopUp(const FAccelByteModelsItemInfo& ItemInfo)
{
	TArray<UAccelByteOrderPriceEntry*> WB_PriceEntries;
	TArray<UAccelByteOrderPurchaseEntry*> WB_PurchaseEntries;
	const FText ItemName = FText::FromString(ItemInfo.Title);

	SwitchPopUpWidget(EOrderPopUpType::PURCHASE_CONFIRMATION);
	T_ItemName->SetText(ItemName);
	T_SuccessItemName->SetText(ItemName);

	FCacheBrush CacheOrderBrush = TutorialProjectUtilities::GetImageFromCache(TutorialProjectUtilities::DefaultImage);
	if(ItemInfo.Images.Num() > 0)
	{
		const FString OrderImageId = FBase64::Encode(ItemInfo.Images[0].SmallImageUrl);
		CacheOrderBrush = TutorialProjectUtilities::GetImageFromCache(OrderImageId);
	}
	
	if(CacheOrderBrush.IsValid())
	{
		Img_Item->SetBrush(*CacheOrderBrush.Get());
	}
	else
	{
		const FString OrderImageId = FBase64::Encode(ItemInfo.Images[0].SmallImageUrl);
		const FString OrderImageUrl = ItemInfo.Images[0].SmallImageUrl;
		TutorialProjectUtilities::GetImageFromURL(
			ItemInfo.Images[0].SmallImageUrl,
			OrderImageId,
			FOnImageReceived::CreateWeakLambda(this, 
				[this](const FCacheBrush Result){
					Img_Item->SetBrush(*Result.Get());
				}));
	}
	
	HB_PriceList->ClearChildren();
	HB_PurchaseButtonList->ClearChildren();

	for(int i = 0; i < ItemInfo.RegionData.Num(); i++)
	{
		FNumberFormattingOptions Formatting;
		Formatting.MinimumFractionalDigits = 2;
		const int32 Price = ItemInfo.RegionData[i].Price;
		const FText PriceFormatted = 
			ItemInfo.RegionData[i].CurrencyType == EAccelByteItemCurrencyType::REAL ?
			FText::AsNumber(static_cast<float>(Price) / 100, &Formatting) : FText::AsNumber(Price);

		UAccelByteOrderPriceEntry* OrderPriceEntry = CreateWidget<UAccelByteOrderPriceEntry>(this, TutorialMenuHUD->GetOrderPriceEntryClass());
		const FText CurrencyCode = FText::FromString(ItemInfo.RegionData[i].CurrencyCode);
		OrderPriceEntry->InitWidget(CurrencyCode, PriceFormatted);
		WB_PriceEntries.Add(OrderPriceEntry);

		UAccelByteOrderPurchaseEntry* OrderPurchaseEntry = CreateWidget<UAccelByteOrderPurchaseEntry>(this, TutorialMenuHUD->GetOrderPurchaseEntryClass());
		OrderPurchaseEntry->InitWidget(ItemInfo, i);
		WB_PurchaseEntries.Add(OrderPurchaseEntry);

		HB_PurchaseButtonList->AddChildToHorizontalBox(WB_PurchaseEntries[i]);
		HB_PriceList->AddChildToHorizontalBox(WB_PriceEntries[i]);
	}
}

void UAccelByteOrder::OnPurchaseFailed(const FString& ErrorMessage) const
{
	SwitchPopUpWidget(EOrderPopUpType::PURCHASE_RESULT);
	WS_Result->SetActiveWidget(VB_PurchaseResultError);
	T_Error->SetText(FText::FromString(ErrorMessage));
}

void UAccelByteOrder::SwitchPopUpWidget(const EOrderPopUpType& Index) const
{
	switch (Index)
	{
	case EOrderPopUpType::PURCHASE_CONFIRMATION:
		WS_ParentPanel->SetActiveWidget(VB_PurchaseConfirmation);
		break;
	case EOrderPopUpType::PURCHASE_RESULT:
		WS_ParentPanel->SetActiveWidget(VB_PurchaseResult);
		break;
	case EOrderPopUpType::WEB_BROWSER:
		WS_ParentPanel->SetActiveWidget(WBrowser_Payment);
		break;
	default:
		WS_ParentPanel->SetActiveWidget(CT_Loading);
		break;
	}
	WS_Result->SetActiveWidget(VB_PurchaseResultSuccess);

	T_ConfirmTitle->SetText(
		FText::FromString(Index == EOrderPopUpType::PURCHASE_RESULT ? PAYMENT_GATEWAY_TEXT : PURCHASE_CONFIRMATION_TEXT)
	);
}

void UAccelByteOrder::LoadPaymentGateway(const FString& Url, const FString& OrderNo)
{
	SwitchPopUpWidget(EOrderPopUpType::WEB_BROWSER);
	
	const IWebBrowserCookieManager::FCookie Cookie = IWebBrowserCookieManager::FCookie{
		"access_token",
		FRegistry::Credentials.GetAccessToken(),
		"",
		"/",
		true,
		true,
		true
	};
	
	const TSharedPtr<IWebBrowserCookieManager> CookieManager = IWebBrowserModule::Get().GetSingleton()->GetCookieManager();
	if (CookieManager.IsValid())
	{
		CookieManager->SetCookie(Url, Cookie, [this, Url, OrderNo](bool bIsSuccess)
		{
			WBrowser_Payment->LoadURL(Url);
			
			// determine payment success by manually checking order status periodically
			GetOwningPlayer()->GetWorldTimerManager().SetTimer(OrderStatusCheckTimer, FTimerDelegate::CreateWeakLambda(this, [this, OrderNo]()
			{
				FRegistry::Order.GetUserOrder(
					OrderNo,
			        THandler<FAccelByteModelsOrderInfo>::CreateWeakLambda(this, [this](const FAccelByteModelsOrderInfo& Response)
					{
			        	UE_LOG(LogTemp, Log, TEXT("Get order status"));
					    
						const EAccelByteOrderStatus OrderStatus = Response.Status;
						if (OrderStatus == EAccelByteOrderStatus::FULFILLED)
						{
							UE_LOG(LogTemp, Log, TEXT("Order fulfilled"));
						    
							GetOwningPlayer()->GetWorldTimerManager().ClearTimer(OrderStatusCheckTimer);
							TutorialMenuHUD->GetStoreMenu()->UpdateWallet();
							SwitchPopUpWidget(EOrderPopUpType::PURCHASE_RESULT);
						}}),
					FErrorHandler::CreateWeakLambda(this, [this](int ErrorCode, const FString& ErrorMessage)
					{
						GetOwningPlayer()->GetWorldTimerManager().ClearTimer(OrderStatusCheckTimer);
						OnPurchaseFailed(ErrorMessage);
					}));
			}), 3.0f, true, 15.0f);
		});
	}
}

void UAccelByteOrder::OnClosePopup()
{
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(OrderStatusCheckTimer);
	TutorialMenuHUD->CloseOrderConfirmationPopUp();
}