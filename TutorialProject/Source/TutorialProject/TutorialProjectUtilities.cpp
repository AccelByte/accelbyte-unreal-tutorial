// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutorialProjectUtilities.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#pragma region Server
FString TutorialProjectUtilities::LaunchArgsLocalDS = TEXT("localds");
FString TutorialProjectUtilities::LaunchArgsSteam = TEXT("steam");
FString TutorialProjectUtilities::LocalServerName = FString::Printf(TEXT("%s-%s"), *TutorialProjectUtilities::LaunchArgsLocalDS, *FPlatformMisc::GetDeviceId());
#pragma endregion 

#pragma region Cloud Save
FString TutorialProjectUtilities::GameSettingCloudSaveKey = TEXT("game-settings-key");
FString TutorialProjectUtilities::CloudSaveJsonKeyA = TEXT("DisplayMode");
FString TutorialProjectUtilities::CloudSaveJsonKeyB = TEXT("CloudSaveState");
EWindowMode::Type TutorialProjectUtilities::DefaultDisplaySettings = EWindowMode::Type::Windowed;
FIntPoint TutorialProjectUtilities::DefaultResolutionSettings = FIntPoint(1280, 720);
int32 TutorialProjectUtilities::DefaultDisplayModeIndex = 2;
int32 TutorialProjectUtilities::DefaultCloudSaveStateIndex = 0;
#pragma endregion

#pragma region Cloud Storage
FString TutorialProjectUtilities::ResourcePathForSaveGalleryImage = TEXT("Caches/MyScreenshotGallery");
FString TutorialProjectUtilities::GalleryLabel = TEXT("Gallery Label");
FString TutorialProjectUtilities::GalleryTag = TEXT("Gallery Tag");
#pragma endregion 

#pragma region General Utilities
FString TutorialProjectUtilities::GameModeAttributeName = TEXT("GameMode");
FString TutorialProjectUtilities::VirtualCurrencyCode = TEXT("VC");
FString TutorialProjectUtilities::DefaultImage = TEXT("AccelByte.png");
TMap<FString, EImageFormat> TutorialProjectUtilities::ImageFormatMap =
{
	{"image/jpeg", EImageFormat::JPEG},
	{"image/png", EImageFormat::PNG},
	{"image/bmp", EImageFormat::BMP}
};
int32 TutorialProjectUtilities::DefaultPartyMemberCount = 4;
#pragma endregion

void TutorialProjectUtilities::ShowLog(const ELogVerbosity::Type& LogVerbosity, const FString& InitLog)
{
	FColor LogColor = FColor::Cyan;
	switch (LogVerbosity)
	{
		case ELogVerbosity::Log:
			UE_LOG(LogTemp, Log, TEXT("%s"), *InitLog);
			LogColor = FColor::Cyan;
			break;
		
		case ELogVerbosity::Warning:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *InitLog);
			LogColor = FColor::Yellow;
			break;
		
		case ELogVerbosity::Error:
			UE_LOG(LogTemp, Error, TEXT("%s"), *InitLog);
			LogColor = FLinearColor(0.75f, 0.f, 0.f, 1.0f).ToFColor(true);
			break;
		
		case ELogVerbosity::Fatal:
			UE_LOG(LogTemp, Fatal, TEXT("%s"), *InitLog);
			LogColor = FColor::Red;
			break;
		
		default:
			break;
	}
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, LogColor, InitLog);
}

void TutorialProjectUtilities::GetImageFromURL(const FString& Url, const FString& ImageId, const FOnImageReceived& OnReceived)
{
	const FHttpRequestPtr ImageRequest = FHttpModule::Get().CreateRequest();
	ImageRequest->SetVerb("GET");
	ImageRequest->SetURL(Url);
	ImageRequest->OnProcessRequestComplete().BindLambda([OnReceived, ImageId](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid())
		{
			const FString ContentType = Response->GetHeader("Content-Type");
			
			if (ImageFormatMap.Contains(ContentType))
			{
				const FString ResourceName = FPaths::ProjectSavedDir() / TEXT("Caches") / ImageId;
				TArray<uint8> ImageData = Response->GetContent();
				IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
				FCacheBrush ImageBrush;
				
				const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormatMap[ContentType]);
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
				{
					constexpr ERGBFormat RGBFormat = ERGBFormat::BGRA;
					constexpr uint8 BitDepth = 8;
					
					TArray<uint8> DecodedImage;
					if (ImageWrapper->GetRaw(RGBFormat, BitDepth, DecodedImage))
					{						
						if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(FName(*ResourceName), ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), DecodedImage))
						{
							ImageBrush = MakeShareable(new FSlateDynamicImageBrush(FName(*ResourceName), FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
							FFileHelper::SaveArrayToFile(ImageData, *ResourceName);
						}
					}
				}
				
				OnReceived.ExecuteIfBound(ImageBrush);
			}
		}
	});
	ImageRequest->ProcessRequest();
}

FCacheBrush TutorialProjectUtilities::GetImageFromCache(const FString& ImageId)
{
	TArray<uint8> ImageData;
	FCacheBrush ImageBrush;
	const FString ResourceName = FPaths::ProjectSavedDir() / TEXT("Caches") / ImageId;
	
	if (FFileHelper::LoadFileToArray(ImageData, *ResourceName))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		
		constexpr EImageFormat ImageFormat =  EImageFormat::PNG;
		const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
		{
			constexpr ERGBFormat RGBFormat = ERGBFormat::BGRA;
			constexpr uint8 BitDepth = 8;
			TArray<uint8> DecodedImage;
			
			if (ImageWrapper->GetRaw(RGBFormat, BitDepth, DecodedImage))
			{
				if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(FName(*ResourceName), ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), DecodedImage))
				{
					ImageBrush = MakeShareable(new FSlateDynamicImageBrush(FName(*ResourceName), FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
				}
			}
		}
	}
	return ImageBrush;
}

void TutorialProjectUtilities::GetImageFromBytes(const TArray<uint8>& BinaryFile, const FString& ResourceName, const FOnImageReceived& OnReceived)
{	
	TArray<uint8> ImageData = BinaryFile;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	constexpr EImageFormat ImageFormat =  EImageFormat::PNG;
	const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		constexpr ERGBFormat RGBFormat = ERGBFormat::BGRA;
		constexpr uint8 BitDepth = 8;
					
		TArray<uint8> DecodedImage;
		if (ImageWrapper->GetRaw(RGBFormat, BitDepth, DecodedImage))
		{						
			if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(FName(*ResourceName), ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), DecodedImage))
			{
				const FCacheBrush ImageBrush = MakeShareable(new FSlateDynamicImageBrush(FName(*ResourceName), FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
				FFileHelper::SaveArrayToFile(ImageData, *ResourceName);

				OnReceived.ExecuteIfBound(ImageBrush);
			}
		}
	}
}