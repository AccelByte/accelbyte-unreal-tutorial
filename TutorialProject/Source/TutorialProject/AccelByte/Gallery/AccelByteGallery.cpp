// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteGallery.h"
#include "Api/AccelByteCloudStorageApi.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteGallery::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(this, &UAccelByteGallery::OnBackToMainMenuButtonClicked);
}

void UAccelByteGallery::OnBackToMainMenuButtonClicked()
{
	this->RemoveFromParent();
	Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->OpenMainMenu();
}

void UAccelByteGallery::UpdateUserInterface()
{
	FRegistry::CloudStorage.GetSlot(ModelSlot.SlotId, THandler<TArray<uint8>>::CreateWeakLambda(this, [this](const TArray<uint8>& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Get Slot Success"));

		if (Result.Num() != 0)
		{
			const FString ResourceName = FPaths::ProjectSavedDir() / TutorialProjectUtilities::ResourcePathForSaveGalleryImage;
			TutorialProjectUtilities::GetImageFromBytes(Result, ResourceName, FOnImageReceived::CreateWeakLambda(this, [this](const FCacheBrush Brush)
			{
				I_Screenshot->SetBrush(FSlateBrush{});
				Tb_ScreenshotName->SetText(FText::FromString(FString("Picture Name: ") + ModelSlot.CustomAttribute));
				I_Screenshot->SetBrush(*Brush.Get()); 
			}));
		}
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Slot Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
	}));
}

void UAccelByteGallery::RefreshGallery()
{
	FRegistry::CloudStorage.GetAllSlots(THandler<TArray<FAccelByteModelsSlot>>::CreateWeakLambda(this, [this](const TArray<FAccelByteModelsSlot>& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Get All Slots Success"));
		
		FAccelByteModelsSlot Slot = {};
		for (const FAccelByteModelsSlot& CurrentSlot : Result)
		{
			if (CurrentSlot.Label == TutorialProjectUtilities::GalleryLabel)
			{
				Slot = CurrentSlot;
				break;
			}
		}

		if (!Slot.Label.IsEmpty())
		{
			ModelSlot = Slot;
			UpdateUserInterface();
		}
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Get All Slots Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
	}));
}

void UAccelByteGallery::TakeScreenshot()
{
	UE_LOG(LogTemp, Log, TEXT("Take a Screenshot"));

	const FString FileName = FDateTime::Now().ToString();
	const FString FullFileName = FPaths::ProjectSavedDir() + "Screenshots/" + FileName + ".png";

	FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
	FScreenshotRequest::OnScreenshotRequestProcessed().AddWeakLambda(this, [this, FullFileName, FileName]()
	{
		TArray<uint8> BinaryData;
		if (FFileHelper::LoadFileToArray(BinaryData, *FullFileName))
		{
			const THandler<FAccelByteModelsSlot> OnSuccess = THandler<FAccelByteModelsSlot>::CreateWeakLambda(this, [this](const FAccelByteModelsSlot& Result)
			{
				UE_LOG(LogTemp, Log, TEXT("Create or Update Slot Success"));

				ModelSlot = Result;

				UpdateUserInterface();
			});
			const FHttpRequestProgressDelegate OnProgress = FHttpRequestProgressDelegate::CreateWeakLambda(this, [](FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceieved)
			{
				UE_LOG(LogTemp, Log, TEXT("Upload progress : %d / %d"), BytesSent, Request->GetContentLength());
			});
			const FErrorHandler OnCreateSlotError = FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Create Slot Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			});
			const FErrorHandler OnUpdateSlotError = FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("Update Slot Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			});
			
			if (ModelSlot.Label == TutorialProjectUtilities::GalleryLabel)
			{
				FRegistry::CloudStorage.UpdateSlot(ModelSlot.SlotId, BinaryData, FileName, TArray<FString>{TutorialProjectUtilities::GalleryTag}, FString(TutorialProjectUtilities::GalleryLabel), FileName, OnSuccess, OnProgress, OnUpdateSlotError);
			}
			else
			{
				FRegistry::CloudStorage.CreateSlot(BinaryData, FileName, TArray<FString>{TutorialProjectUtilities::GalleryTag}, FString(TutorialProjectUtilities::GalleryLabel), FileName, OnSuccess, OnProgress, OnCreateSlotError);
			}
		}
	});
	
	FScreenshotRequest::RequestScreenshot(FullFileName, true, false);
}