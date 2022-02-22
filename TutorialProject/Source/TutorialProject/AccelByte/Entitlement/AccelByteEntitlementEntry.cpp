// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteEntitlementEntry.h"
#include "AccelByteEntitlement.h"
#include "Api/AccelByteEntitlementApi.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteEntitlementEntry::InitData(const FAccelByteModelsPopulatedItemInfo& Data, int32 DataCount, const FString& EntitlementId)
{
	WS_EntitlementEntry->SetActiveWidgetIndex(0);

	EntitlementIdentity = EntitlementId;
	
	Tb_ItemName->SetText(FText::FromString(Data.Name));
	Tb_ItemDescription->SetText(FText::FromString(Data.Description));
	
	if (Data.EntitlementType == EAccelByteEntitlementType::CONSUMABLE)
	{
		Sb_Consume->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Btn_Consume->OnClicked.AddUniqueDynamic(this, &UAccelByteEntitlementEntry::OnConsumeButtonClicked);
		Sb_UseCount->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_UseCount->SetText(FText::FromString(FString::FromInt(DataCount)));
	}
	
	if (Data.Images.Num() > 0)
	{
		const FString ImageId = FBase64::Encode(Data.Images[0].SmallImageUrl);
		const FCacheBrush Brush = TutorialProjectUtilities::GetImageFromCache(ImageId);
		
		if (Brush.IsValid())
		{
			I_ItemImage->SetBrush(*Brush.Get());
			WS_EntitlementEntry->SetActiveWidgetIndex(1);
		}
		else
		{
			TutorialProjectUtilities::GetImageFromURL(Data.Images[0].SmallImageUrl, ImageId, FOnImageReceived::CreateLambda([this](const FCacheBrush Result)
			{
				I_ItemImage->SetBrush(*Result.Get());
				WS_EntitlementEntry->SetActiveWidgetIndex(1);
			}));
		}
	}
	else
	{
		const FCacheBrush DefaultBrush = TutorialProjectUtilities::GetImageFromCache(TutorialProjectUtilities::DefaultImage);
		if (DefaultBrush.IsValid())
		{
			I_ItemImage->SetBrush(*DefaultBrush.Get());
			WS_EntitlementEntry->SetActiveWidgetIndex(1);
		}
	}
}

void UAccelByteEntitlementEntry::OnConsumeButtonClicked()
{
	FRegistry::Entitlement.ConsumeUserEntitlement(EntitlementIdentity, 1, THandler<FAccelByteModelsEntitlementInfo>::CreateLambda([this](const FAccelByteModelsEntitlementInfo& Result)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Consume User Entitlement"));
		Tb_UseCount->SetText(FText::FromString(FString::FromInt(Result.UseCount)));

		if (Result.UseCount == 0)
		{
			ATutorialMenuHUD* TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
			TutorialMenuHUD->GetInventoryMenu()->RefreshInventoryMenu();
		}
	}),
	FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Consume Error Entitlement Error, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
	}));
}