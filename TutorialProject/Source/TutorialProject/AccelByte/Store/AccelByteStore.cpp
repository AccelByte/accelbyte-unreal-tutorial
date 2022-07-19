// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteStore.h"

#include "AccelByteStoreCategory.h"
#include "Api/AccelByteItemApi.h"
#include "Core/AccelByteError.h"
#include "Core/AccelByteRegistry.h"
#include "Models/AccelByteEcommerceModels.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/AccelByte/Wallet/AccelByteWallet.h"
#include "AccelByteStoreItemEntry.h"
#include "Api/AccelByteCategoryApi.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/UniformGridPanel.h"

void UAccelByteStore::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	check(TutorialMenuHUD);

	UpdateWallet();

	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(this, &UAccelByteStore::OnClickBackToMainMenu);
	Btn_AllCategory->OnClicked.AddUniqueDynamic(this, &UAccelByteStore::OnClickAllCategory);

	if (HB_StoreCategories->GetChildrenCount() <= 0)
	{
		// Get Categories
		FRegistry::Category.GetRootCategories(
			"en",
			THandler<TArray<FAccelByteModelsCategoryInfo>>::CreateWeakLambda(this, 
				[this](const TArray<FAccelByteModelsCategoryInfo> Result)
				{
					InitCategoryButtons(Result);
				}),
			FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, FString ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("[Store] Failed to get Store Item list!"));
			})
		);
	}

	LoadItems("");
}

void UAccelByteStore::UpdateWallet() const
{
	WB_Wallet->UpdateCurrency();
}

void UAccelByteStore::InitCategoryButtons(const TArray<FAccelByteModelsCategoryInfo>& CategoryResult)
{
	for (const FAccelByteModelsCategoryInfo& CategoryInfo : CategoryResult)
	{
		const TWeakObjectPtr<UAccelByteStoreCategory> CategoryButton = MakeWeakObjectPtr<UAccelByteStoreCategory>(
			CreateWidget<UAccelByteStoreCategory>(this, AccelByteStoreCategoryClass.Get())
		);
		CategoryButton->InitCategory(CategoryInfo);
		HB_StoreCategories->AddChild(CategoryButton.Get());
	}
}

void UAccelByteStore::LoadItems(const FString& StoreCategoryPath)
{
	FAccelByteModelsItemCriteria StoreItemCriteria;
	StoreItemCriteria.ItemType = EAccelByteItemType::NONE;
	StoreItemCriteria.Region = "";
	StoreItemCriteria.Language = "";
	StoreItemCriteria.CategoryPath = StoreCategoryPath;

	UGP_StoreGridPanel->ClearChildren();
	AccelByte::FRegistry::Item.GetItemsByCriteria(
		StoreItemCriteria,
		0,
		0,
		THandler<FAccelByteModelsItemPagingSlicedResult>::CreateWeakLambda(this, 
			[this](const FAccelByteModelsItemPagingSlicedResult Result)
			{
				PopulateGridPanel(Result);
			}),
		FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, FString ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("[Store] Failed to get Store Item list!"));
		})
	);
}

void UAccelByteStore::PopulateGridPanel(const FAccelByteModelsItemPagingSlicedResult& Result)
{
	int32 CurrentRow = 0;
	int32 CurrentColumn = 0;
	for (const FAccelByteModelsItemInfo& Info : Result.Data)
	{
		if (CurrentColumn >= MaxItemColumn)
		{
			CurrentColumn = 0;
			++CurrentRow;
		}

		const TWeakObjectPtr<UAccelByteStoreItemEntry> StoreItemEntryWidget = MakeWeakObjectPtr<UAccelByteStoreItemEntry>(
			CreateWidget<UAccelByteStoreItemEntry>(this, StoreItemEntryClass.Get())
			);
		StoreItemEntryWidget.Get()->LoadInfo(Info);
		UGP_StoreGridPanel->AddChildToUniformGrid(StoreItemEntryWidget.Get(), CurrentRow, CurrentColumn);
		CurrentColumn++;
	}
}

void UAccelByteStore::OnClickAllCategory()
{
	LoadItems("");
}

void UAccelByteStore::OnClickBackToMainMenu()
{
	TutorialMenuHUD->CloseStoreMenu();
}
