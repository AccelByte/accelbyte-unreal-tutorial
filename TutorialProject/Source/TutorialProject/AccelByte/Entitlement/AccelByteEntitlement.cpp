// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteEntitlement.h"
#include "AccelByteEntitlementEntry.h"
#include "Api/AccelByteEntitlementApi.h"
#include "Api/AccelByteItemApi.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Wallet/AccelByteWallet.h"

void UAccelByteEntitlement::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());

	check(TutorialMenuHUD);
	
	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::CloseInventoryMenu);

	RefreshInventoryMenu();
	WB_Wallet->UpdateCurrency();
}

void UAccelByteEntitlement::RefreshInventoryMenu()
{
	WS_InventoryMenu->SetActiveWidgetIndex(0);

	GP_Entitlements->ClearChildren();
	
	FRegistry::Entitlement.QueryUserEntitlements("", "", 0, 99, THandler<FAccelByteModelsEntitlementPagingSlicedResult>::CreateLambda([this](const FAccelByteModelsEntitlementPagingSlicedResult& Result)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Success Get User Entitlements, Total Entitlements: %d"), Result.Data.Num()));

		int32 RowCount = 0;
		int32 ColumnCount = 0;

		for (const FAccelByteModelsEntitlementInfo Data : Result.Data)
		{
			UAccelByteEntitlementEntry* Entry = CreateWidget<UAccelByteEntitlementEntry>(this, TutorialMenuHUD->GetInventoryItemClass().Get());
			
			Entry->SetPadding(FMargin(10.0f));

			GP_Entitlements->AddChildToGrid(Entry, RowCount, ColumnCount);

			if (ColumnCount == MaximumGridColumn - 1)
			{
				RowCount++;
				ColumnCount = 0;
			}
			else
			{
				ColumnCount++;
			}

			FRegistry::Item.GetItemById(Data.ItemId, TEXT("en"), TEXT("US"), THandler<FAccelByteModelsPopulatedItemInfo>::CreateLambda([Entry, Data](const FAccelByteModelsPopulatedItemInfo& Result)
			{
				Entry->InitData(Result, Data.UseCount, Data.Id);
			}),
			FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Get Item by ID Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
			}));
		}
		
		WS_InventoryMenu->SetActiveWidgetIndex(1);
	}),
	FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Query User Entitlements Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
	}),	EAccelByteEntitlementClass::NONE, EAccelByteAppType::NONE);
}
