// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutorialProjectUtilities.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/Online/HTTP/Public/Http.h"

#pragma region Server
FString TutorialProjectUtilities::LaunchArgsLocalDS = TEXT("localds");
FString TutorialProjectUtilities::LaunchArgsSteam = TEXT("steam");
FString TutorialProjectUtilities::LocalServerName = FString::Printf(TEXT("%s-%s"), *TutorialProjectUtilities::LaunchArgsLocalDS, *TutorialProjectUtilities::GetDeviceId());
FString TutorialProjectUtilities::LocalServerIpAddress = "127.0.0.1";
int32 TutorialProjectUtilities::LocalServerPort = 7777;
int32 TutorialProjectUtilities::ServerPort = 7777;
#pragma endregion 

#pragma region Cloud Save
FString TutorialProjectUtilities::GameSettingCloudSaveKey = TEXT("game-settings-key");
FString TutorialProjectUtilities::CloudSaveJsonKeyA = TEXT("DisplayMode");
FString TutorialProjectUtilities::CloudSaveJsonKeyB = TEXT("CloudSaveState");
EWindowMode::Type TutorialProjectUtilities::DefaultDisplaySettings = EWindowMode::Type::Windowed;
FIntPoint TutorialProjectUtilities::DefaultResolutionSettings = FIntPoint(1280, 720);
int32 TutorialProjectUtilities::DefaultCloudSaveStateIndex = 0;
FString TutorialProjectUtilities::PublicCloudSaveEquipmentKey = TEXT("player-equipment");
FString TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey = TEXT("usedItem");
FString TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey = TEXT("equippedItem");
FString TutorialProjectUtilities::PublicCloudSaveNoneValue = TEXT("None");
FString TutorialProjectUtilities::PublicCloudSaveAbilityBronze = TEXT("Ability_Bronze");
FString TutorialProjectUtilities::PublicCloudSaveAbilitySilver = TEXT("Ability_Silver");
#pragma endregion

#pragma region Cloud Storage
FString TutorialProjectUtilities::ResourcePathForSaveGalleryImage = TEXT("Caches/MyScreenshotGallery");
FString TutorialProjectUtilities::GalleryLabel = TEXT("Gallery Label");
FString TutorialProjectUtilities::GalleryTag = TEXT("Gallery Tag");
#pragma endregion

#pragma region Reward and Pop Up Notification
FString TutorialProjectUtilities::RewardNotificationTopic = TEXT("e-commerce/rewards");
FString TutorialProjectUtilities::RewardTopic = TEXT("rewardTopic");
FString TutorialProjectUtilities::AchievementEventTopic = TEXT("Achievement");
FString TutorialProjectUtilities::RewardCondition = TEXT("rewardCondition");
FString TutorialProjectUtilities::AchievementCodeSplitString = TEXT("achievementCode");
FString TutorialProjectUtilities::EntitlementSummariesObjectField = TEXT("entitlementSummaries");
FString TutorialProjectUtilities::ItemIdStringField = TEXT("itemId");
FString TutorialProjectUtilities::CreditSummariesObjectField = TEXT("creditSummaries");
FString TutorialProjectUtilities::AmountStringField = TEXT("amount");
#pragma

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
int32 TutorialProjectUtilities::DefaultReadyConsentTimeOut = 30;
FLinearColor TutorialProjectUtilities::SelectedTabGreyColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);
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

FString TutorialProjectUtilities::LocalDeviceId()
{
	FString NewDeviceId;

	//check then read file
	FString CurrentDirectory = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectDir());
	CurrentDirectory.Append(TEXT("DeviceIdHelper/DeviceId.txt"));
	CurrentDirectory.ReplaceInline(TEXT("/"), TEXT("\\"));
	FFileHelper::LoadFileToString(NewDeviceId, *CurrentDirectory);

	//generate one if still empty
	if(NewDeviceId.IsEmpty())
	{
		FString FilePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectDir());
		FilePath.Append("DeviceIdHelper/DeviceId.txt");

		TCHAR valid[] = {'0', '1', '2', '3', '4', '5', '6','7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
						'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
		
		for(int i = 0; i < 30; i++)
		{
			NewDeviceId = NewDeviceId + valid[FMath::RandRange(0, 35)];
		}
		
		FFileHelper::SaveStringToFile(NewDeviceId, *FilePath);
	}
	return NewDeviceId;
}

FString TutorialProjectUtilities::GetDeviceId()
{
	FString DeviceId = FPlatformMisc::GetDeviceId();
	if (DeviceId.IsEmpty())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		const TArray<uint8> MacAddr = FPlatformMisc::GetMacAddress();
		PRAGMA_ENABLE_DEPRECATION_WARNINGS
		FString MacAddressString;
		for (TArray<uint8>::TConstIterator it(MacAddr);it;++it)
		{
			MacAddressString += FString::Printf(TEXT("%02x"),*it);
		}
		DeviceId = FMD5::HashAnsiString(*MacAddressString);

		if(DeviceId.IsEmpty())
		{
			DeviceId = *LocalDeviceId();
			UE_LOG(LogTemp, Warning, TEXT("LocalDeviceId Device Id: %s"), *DeviceId);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FMD5::HashAnsiString(*MacAddressString) Device Id: %s"), *DeviceId);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FPlatformMisc::GetDeviceId() Device Id: %s"), *DeviceId);
	}
	return  DeviceId;
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