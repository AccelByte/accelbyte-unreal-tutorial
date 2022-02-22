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
	FRegistry::CloudStorage.GetSlot(ModelSlot.SlotId, THandler<TArray<uint8>>::CreateLambda([this](const TArray<uint8>& Result)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Get Slot Success"));

		if (Result.Num() != 0)
		{
			const FString ResourceName = FPaths::ProjectSavedDir() / TutorialProjectUtilities::ResourcePathForSaveGalleryImage;
			TutorialProjectUtilities::GetImageFromBytes(Result, ResourceName, FOnImageReceived::CreateLambda([this](const FCacheBrush Brush)
			{
				I_Screenshot->SetBrush(FSlateBrush{});
				Tb_ScreenshotName->SetText(FText::FromString(FString("name: ") + ModelSlot.CustomAttribute));
				I_Screenshot->SetBrush(*Brush.Get());
			}));
		}
	}),
	FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Get Slot Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
	}));
}

void UAccelByteGallery::RefreshGallery()
{
	FRegistry::CloudStorage.GetAllSlots(THandler<TArray<FAccelByteModelsSlot>>::CreateLambda([this](const TArray<FAccelByteModelsSlot>& Result)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Get All Slots Success"));
		
		FAccelByteModelsSlot Slot = {};
		for (const FAccelByteModelsSlot& ModelSlot : Result)
		{
			if (ModelSlot.Label == TutorialProjectUtilities::GalleryLabel)
			{
				Slot = ModelSlot;
				break;
			}
		}

		if (!Slot.Label.IsEmpty())
		{
			ModelSlot = Slot;
			UpdateUserInterface();
		}
	}),
	FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Get All Slots Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
	}));
}

void UAccelByteGallery::TakeScreenshot()
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Take a Screenshot"));

	const FString FileName = FDateTime::Now().ToString();
	const FString FullFileName = FPaths::ProjectSavedDir() + "Screenshots/" + FileName + ".png";

	FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
	FScreenshotRequest::OnScreenshotRequestProcessed().AddWeakLambda(this, [this, FullFileName, FileName]()
	{
		TArray<uint8> BinaryData;
		if (FFileHelper::LoadFileToArray(BinaryData, *FullFileName))
		{
			const THandler<FAccelByteModelsSlot> OnSuccess = THandler<FAccelByteModelsSlot>::CreateLambda([this](const FAccelByteModelsSlot& Result)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Create or Update Slot Success"));

				UpdateUserInterface();
			});
			const FHttpRequestProgressDelegate OnProgress = FHttpRequestProgressDelegate::CreateLambda([](FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceieved)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Upload progress : %d / %d"), BytesSent, Request->GetContentLength()));
			});
			const FErrorHandler OnCreateSlotError = FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Create Slot Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
			});
			const FErrorHandler OnUpdateSlotError = FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Update Slot Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
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