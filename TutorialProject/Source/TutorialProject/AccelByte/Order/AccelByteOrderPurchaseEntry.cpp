// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteOrderPurchaseEntry.h"
#include "Core/AccelByteRegistry.h"
#include "Api/AccelByteOrderApi.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "AccelByteOrder.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/AccelByte/Statistic/AccelByteStatistic.h"
#include "TutorialProject/AccelByte/Store/AccelByteStore.h"
#include "TutorialProject/AccelByte/Wallet/AccelByteWallet.h"

#define STAT_VC_SPENDING "vc-spending"

void UAccelByteOrderPurchaseEntry::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	AccelByteStatistic = NewObject<UAccelByteStatistic>();
	Btn_Purchase->OnClicked.AddUniqueDynamic(this, &UAccelByteOrderPurchaseEntry::OnClickPurchaseItem);
}

void UAccelByteOrderPurchaseEntry::InitWidget(const FAccelByteModelsItemInfo& Item, int32 Index)
{
	ItemInfo = Item;
	CurrencyIndex = Index;
	T_CurrencyCode->SetText(FText::FromString(ItemInfo.RegionData[Index].CurrencyCode));
}

void UAccelByteOrderPurchaseEntry::OnClickPurchaseItem()
{
	const UAccelByteOrder* OrderConfirmationMenu = TutorialMenuHUD->GetOrderConfirmationMenu(); 
	OrderConfirmationMenu->SwitchPopUpWidget(EOrderPopUpType::LOADING);

	FAccelByteModelsOrderCreate OrderRequest;

	OrderRequest.ItemId = ItemInfo.ItemId;
	OrderRequest.Quantity = 1;
	OrderRequest.Price = ItemInfo.RegionData[CurrencyIndex].Price;
	OrderRequest.DiscountedPrice = ItemInfo.RegionData[CurrencyIndex].DiscountedPrice;
	OrderRequest.CurrencyCode = ItemInfo.RegionData[CurrencyIndex].CurrencyCode;
	OrderRequest.Region = ItemInfo.Region;
	OrderRequest.Language = ItemInfo.Language;

	FRegistry::Order.CreateNewOrder(
		OrderRequest,
		THandler<FAccelByteModelsOrderInfo>::CreateLambda(
			[this, OrderConfirmationMenu](const FAccelByteModelsOrderInfo& Response)
			{
				TutorialProjectUtilities::ShowLog(
					ELogVerbosity::Log,
					FString::Printf(TEXT("Success Order Item: %s"), *Response.OrderNo));
				
				if(ItemInfo.RegionData[CurrencyIndex].CurrencyType == EAccelByteItemCurrencyType::REAL)
				{
					TutorialMenuHUD->GetOrderConfirmationMenu()->LoadPaymentGateway(Response.PaymentStationUrl, Response.OrderNo);
				}
				else
				{
					// Create a Map that contains StatCode info and set the Item Price as its increment value
					TMap<FString, int32> StatCode;
					StatCode.Add(STAT_VC_SPENDING, Response.Price);
					
					AccelByteStatistic->IncrementUserStatistic(StatCode);
					
					TutorialMenuHUD->GetStoreMenu()->UpdateWallet();
					OrderConfirmationMenu->SwitchPopUpWidget(EOrderPopUpType::PURCHASE_RESULT);
				}
			}
		),
		FErrorHandler::CreateLambda(
			[this, OrderConfirmationMenu](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(
					ELogVerbosity::Log,
					FString::Printf(TEXT("Failed Order Item: %s (%d)"), *ErrorMessage, ErrorCode));
				OrderConfirmationMenu->OnPurchaseFailed(ErrorMessage);
			}
		)
	);
}

#undef STAT_VC_SPENDING