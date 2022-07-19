// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteStoreItemEntry.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "Models/AccelByteEcommerceModels.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialMenuHUD.h"

void UAccelByteStoreItemEntry::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	check(TutorialMenuHUD);
}

void UAccelByteStoreItemEntry::LoadInfo(const FAccelByteModelsItemInfo& Info)
{
	ItemEntryInfo = Info;

	T_ItemName->SetText(FText::FromString(ItemEntryInfo.Name));
	T_ItemDescription->SetText(FText::FromString(ItemEntryInfo.Description));

	if (Info.Tags.Num() > 0)
	{
		// Tags 0 means the item id to get the image from cache folder
		const FString ItemTagId = Info.Tags[0] + ".png";
		const FCacheBrush Brush = TutorialProjectUtilities::GetImageFromCache(ItemTagId);
		if (Brush.IsValid())
		{
			Img_ItemImage->SetBrush(*Brush.Get());
		}
	}

	const FString CurrencyCode = ItemEntryInfo.RegionData[0].CurrencyCode;
	const int32 ItemPrice = ItemEntryInfo.RegionData[0].Price;
	const FString PriceFormatted = Info.RegionData[0].CurrencyType == EAccelByteItemCurrencyType::REAL ?
		FString::SanitizeFloat(ItemPrice / 100, 2) : FString::FromInt(ItemPrice);
	T_Price->SetText(FText::FromString(PriceFormatted + " " + CurrencyCode));

	Btn_BuyItem->OnClicked.AddUniqueDynamic(this, &UAccelByteStoreItemEntry::OnClickedBuy);
}

void UAccelByteStoreItemEntry::OnClickedBuy()
{
	TutorialMenuHUD->OpenOrderConfirmationPopUp(ItemEntryInfo);
}
