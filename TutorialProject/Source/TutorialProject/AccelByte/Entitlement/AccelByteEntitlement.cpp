// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteEntitlement.h"
#include "AccelByteEntitlementEntry.h"
#include "Api/AccelByteCloudSaveApi.h"
#include "Api/AccelByteEntitlementApi.h"
#include "Api/AccelByteItemApi.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
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

void UAccelByteEntitlement::QueryEntitlements(const FPlayerEquipment& PlayerEquipment)
{
	
	FRegistry::Entitlement.QueryUserEntitlements("", "", 0, 99, THandler<FAccelByteModelsEntitlementPagingSlicedResult>::CreateWeakLambda(this, [this, PlayerEquipment](const FAccelByteModelsEntitlementPagingSlicedResult& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Success Get User Entitlements, Total Entitlements: %d"), Result.Data.Num());

		int32 RowCount = 0;
		int32 ColumnCount = 0;

		for (const FAccelByteModelsEntitlementInfo& EntitlementInfo : Result.Data)
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

			FRegistry::Item.GetItemById(EntitlementInfo.ItemId, TEXT("en"), TEXT("US"), THandler<FAccelByteModelsPopulatedItemInfo>::CreateWeakLambda(this, [Entry, EntitlementInfo, PlayerEquipment](const FAccelByteModelsPopulatedItemInfo& ItemInfo)
			{
				Entry->InitData(ItemInfo, EntitlementInfo, PlayerEquipment);
			}),
			FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Get Item by ID Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
		}

		WS_InventoryMenu->SetActiveWidgetIndex(1);
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Query User Entitlements Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
	}),	EAccelByteEntitlementClass::NONE, EAccelByteAppType::NONE);
}

void UAccelByteEntitlement::RefreshInventoryMenu()
{
	WS_InventoryMenu->SetActiveWidgetIndex(0);

	GP_Entitlements->ClearChildren();

	FRegistry::CloudSave.GetPublicUserRecord(
		TutorialProjectUtilities::PublicCloudSaveEquipmentKey,
		FRegistry::Credentials.GetUserId(),
		THandler<FAccelByteModelsUserRecord>::CreateWeakLambda(this, [this](const FAccelByteModelsUserRecord& Result)
	{
		FPlayerEquipment PlayerEquipment = {};
		if (Result.Value.JsonObject->HasField(TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey))
		{
			const FString& EquippedItem = Result.Value.JsonObject->GetStringField(TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey);
			PlayerEquipment.EquippedItem = EquippedItem;
		}
		if (Result.Value.JsonObject->HasField(TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey))
		{
			const FString& UsedItem = Result.Value.JsonObject->GetStringField(TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey);
			PlayerEquipment.UsedItem = UsedItem;
		}
		
		QueryEntitlements(PlayerEquipment);
	}),
	FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPublicUserRecord Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		if (ErrorCode == 18081)
		{
			FPlayerEquipment PlayerEquipment = FPlayerEquipment();
			PlayerEquipment.EquippedItem = TutorialProjectUtilities::PublicCloudSaveNoneValue;
			PlayerEquipment.UsedItem = TutorialProjectUtilities::PublicCloudSaveNoneValue;

			const TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(PlayerEquipment);
			FRegistry::CloudSave.SaveUserRecord(TutorialProjectUtilities::PublicCloudSaveEquipmentKey, *JsonObject, true, FVoidHandler::CreateWeakLambda(this, [this, PlayerEquipment]()
			{
				QueryEntitlements(PlayerEquipment);
			}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("SaveUserRecord Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
		}
	}));
}
