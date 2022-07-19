// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteMatchmakingPlayerEntry.h"

#include "Api/AccelByteCloudSaveApi.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteMatchmakingPlayerEntry::NativeOnListItemObjectSet(UObject* InObject)
{
	const UAccelByteMatchmakingPlayerEntry* Entry = Cast<UAccelByteMatchmakingPlayerEntry>(InObject);

	if (!Entry->DisplayName.IsEmpty())
	{
		Tb_PlayerUsername->SetText(FText::FromString(Entry->DisplayName));
	}
	if (!Entry->UserId.IsEmpty())
	{
		FRegistry::CloudSave.GetPublicUserRecord(TutorialProjectUtilities::PublicCloudSaveEquipmentKey, Entry->UserId, THandler<FAccelByteModelsUserRecord>::CreateWeakLambda(this, [this](const FAccelByteModelsUserRecord& Result)
		{
			if (Result.Value.JsonObject->HasField(TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey))
			{
				const FString& EquippedItem = Result.Value.JsonObject->GetStringField(TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey);
				// Hardcode, get the the image from the cache and adding .png as a file name from the result value
				if (EquippedItem != TutorialProjectUtilities::PublicCloudSaveNoneValue)
				{
					I_Badge->SetBrush(*TutorialProjectUtilities::GetImageFromCache(EquippedItem + ".png"));
					I_Badge->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
			}
			
			if (Result.Value.JsonObject->HasField(TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey))
			{
				const FString& UsedItem = Result.Value.JsonObject->GetStringField(TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey);
				// Hardcode, get the the image from the cache and adding .png as a file name from the result value
				if (UsedItem != TutorialProjectUtilities::PublicCloudSaveNoneValue)
				{
					I_Ability->SetBrush(*TutorialProjectUtilities::GetImageFromCache(UsedItem + ".png"));
					I_Ability->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				}
			}
		}), FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("GetPublicUserRecord Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		}));
	}
}
