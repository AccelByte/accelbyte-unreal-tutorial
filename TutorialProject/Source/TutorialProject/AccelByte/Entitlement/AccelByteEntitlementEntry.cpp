// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteEntitlementEntry.h"
#include "AccelByteEntitlement.h"
#include "Api/AccelByteCloudSaveApi.h"
#include "Api/AccelByteEntitlementApi.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteEntitlementEntry::InitData(const FAccelByteModelsPopulatedItemInfo& ItemInfo, const FAccelByteModelsEntitlementInfo& EntitlementInfo, const FPlayerEquipment& CurrentPlayerEquipment)
{
	ItemData = ItemInfo;
	EntitlementData = EntitlementInfo;
	PlayerEquipment = CurrentPlayerEquipment;
	
	WS_EntitlementEntry->SetActiveWidget(CT_Loading);
	
	Tb_ItemName->SetText(FText::FromString(ItemData.Name));
	Tb_ItemDescription->SetText(FText::FromString(ItemData.Description));
	
	if (ItemData.EntitlementType == EAccelByteEntitlementType::CONSUMABLE)
	{
		if (PlayerEquipment.UsedItem == TutorialProjectUtilities::PublicCloudSaveNoneValue)
		{
			WS_ConsumeOrEquip->SetActiveWidget(WS_Consume);
			WS_Consume->SetActiveWidget(Sb_Consume);
			Btn_Consume->OnClicked.AddUniqueDynamic(this, &UAccelByteEntitlementEntry::OnConsumeButtonClicked);
		}
		else if (PlayerEquipment.UsedItem == ItemData.Tags[0])
		{
			WS_ConsumeOrEquip->SetActiveWidget(WS_Consume);
			WS_Consume->SetActiveWidget(Sb_InUsed);
		}
		else
		{
			WS_ConsumeOrEquip->SetVisibility(ESlateVisibility::Collapsed);
		}
		Sb_UseCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_UseCount->SetText(FText::FromString(FString::FromInt(EntitlementInfo.UseCount)));
	}
	else if (ItemData.EntitlementType == EAccelByteEntitlementType::DURABLE)
	{
		WS_ConsumeOrEquip->SetActiveWidget(WS_Equip);
		bool bIsEquipButton = false;
		// Tags 0 means the item id to compare the equipped item from cloud save
		if (PlayerEquipment.EquippedItem == TutorialProjectUtilities::PublicCloudSaveNoneValue || PlayerEquipment.EquippedItem != ItemData.Tags[0])
		{
			bIsEquipButton = true;
		}

		if (bIsEquipButton)
		{
			WS_Equip->SetActiveWidget(Sb_Equip);
			Btn_Equip->OnClicked.AddUniqueDynamic(this, &UAccelByteEntitlementEntry::OnEquipButtonClicked);
		}
		else
		{
			WS_Equip->SetActiveWidget(Sb_Unequip);
			Btn_Unequip->OnClicked.AddUniqueDynamic(this, &UAccelByteEntitlementEntry::OnUnequipButtonClicked);
		}
	}

	if (ItemData.Tags.Num() > 0)
	{
		// Tags 0 means the item id to get the image from cache folder
		const FString ItemTagId = ItemData.Tags[0] + ".png";
		const FCacheBrush Brush = TutorialProjectUtilities::GetImageFromCache(ItemTagId);
		if (Brush.IsValid())
		{
			I_ItemImage->SetBrush(*Brush.Get());
			WS_EntitlementEntry->SetActiveWidget(O_Entitlement);
		}
	}
}

void UAccelByteEntitlementEntry::OnConsumeButtonClicked()
{
	Btn_Consume->SetIsEnabled(false);
	const int32 UseCount = 1;
	FRegistry::Entitlement.ConsumeUserEntitlement(EntitlementData.Id, UseCount, THandler<FAccelByteModelsEntitlementInfo>::CreateWeakLambda(this, [this](const FAccelByteModelsEntitlementInfo& Result)
	{
		PlayerEquipment.UsedItem = ItemData.Tags[0];
		const TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(PlayerEquipment);
		UpdatePlayerEquipment(*JsonObject, Btn_Consume);
	}), FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("ConsumeUserEntitlement Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		Btn_Consume->SetIsEnabled(true);
	}));
}

void UAccelByteEntitlementEntry::OnEquipButtonClicked()
{
	PlayerEquipment.EquippedItem = ItemData.Tags[0];
	const TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(PlayerEquipment);
	UpdatePlayerEquipment(*JsonObject, Btn_Equip);
}

void UAccelByteEntitlementEntry::OnUnequipButtonClicked()
{
	PlayerEquipment.EquippedItem = TutorialProjectUtilities::PublicCloudSaveNoneValue;
	const TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(PlayerEquipment);
	UpdatePlayerEquipment(*JsonObject, Btn_Unequip);
}

void UAccelByteEntitlementEntry::UpdatePlayerEquipment(const FJsonObject& JsonObject, UButton* CurrentActiveButton)
{
	CurrentActiveButton->SetIsEnabled(false);
	FRegistry::CloudSave.ReplaceUserRecord(TutorialProjectUtilities::PublicCloudSaveEquipmentKey, JsonObject, true, FVoidHandler::CreateWeakLambda(this, [this]()
	{
		ATutorialMenuHUD* TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
		check(TutorialMenuHUD);
		TutorialMenuHUD->GetInventoryMenu()->RefreshInventoryMenu();
	}), FErrorHandler::CreateWeakLambda(this, [CurrentActiveButton](int32 ErrorCode, const FString& ErrorMessage)
	{
		CurrentActiveButton->SetIsEnabled(true);
		UE_LOG(LogTemp, Error, TEXT("ReplaceUserRecord Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
	}));
}
