// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteStoreCategory.h"

#include "AccelByteStore.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Models/AccelByteEcommerceModels.h"
#include "TutorialProject/TutorialMenuHUD.h"

void UAccelByteStoreCategory::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	check(TutorialMenuHUD);
	
	StoreMenuClass = TutorialMenuHUD->GetStoreMenu();
	check(StoreMenuClass);
}

void UAccelByteStoreCategory::InitCategory(const FAccelByteModelsCategoryInfo& CategoryInfo)
{
	SavedCategoryInfo = CategoryInfo;

	T_CategoryName->SetText(FText::FromString(CategoryInfo.DisplayName));
	Btn_Category->OnClicked.AddUniqueDynamic(this, &UAccelByteStoreCategory::OnClickCategoryButton);
}

void UAccelByteStoreCategory::OnClickCategoryButton()
{
	StoreMenuClass->LoadItems(SavedCategoryInfo.CategoryPath);
}
