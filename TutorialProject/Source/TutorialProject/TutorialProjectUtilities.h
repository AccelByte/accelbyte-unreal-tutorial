// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once
#include "IImageWrapper.h"

typedef TSharedPtr<const FSlateBrush> FCacheBrush;
DECLARE_DELEGATE_OneParam(FOnImageReceived, FCacheBrush);

class TutorialProjectUtilities
{

public:
	
	static FString LaunchArgsLocalDS;
	static FString LaunchArgsSteam;
	static FString LocalServerName;
	
	static FString GameSettingCloudSaveKey;
	static FString CloudSaveJsonKeyA;
	static FString CloudSaveJsonKeyB;
	static EWindowMode::Type DefaultDisplaySettings;
	static FIntPoint DefaultResolutionSettings;
	static int32 DefaultDisplayModeIndex;
	static int32 DefaultCloudSaveStateIndex;

	static FString ResourcePathForSaveGalleryImage;
	static FString GalleryLabel;
	static FString GalleryTag;
	
	static FString GameModeAttributeName;
	static FString VirtualCurrencyCode;
	static FString DefaultImage;
	static TMap<FString, EImageFormat> ImageFormatMap;
	static int32 DefaultPartyMemberCount;

	/**
	 * @brief Showing the log from both UE4_LOG and AddOnScreenDebugMessage Log.
	 * @param LogVerbosity Log Verbosity Type.
	 * @param Log The Log as String to show to the player.
	 */
	static void ShowLog(const ELogVerbosity::Type& LogVerbosity, const FString& Log);

	/**
	 * @brief Getting an image from a specific URL.
	 * @param Url URL that are going to download and convert to image brush.
	 * @param ImageId Id of the image to get a unique image from the URL, then gonna creat the file on cache folder based on the ImageId that player's generated.
	 * @param OnReceived Lambda function when generate the image is successful.
	 */
	static void GetImageFromURL(const FString& Url, const FString& ImageId, const FOnImageReceived& OnReceived);

	/**
	 * @brief This function is to get the image from a specific folder called caches on the <ProjectSavedDirectory>/Caches/ folder path.
	 * @param ImageId Id of the image to get a unique image from the URL, then gonna creat the file on cache folder based on the ImageId that player's generated.
	 * @return return the cache brush or pointer of slate brush to passed the image data.
	 */
	static FCacheBrush GetImageFromCache(const FString& ImageId);

	/**
	 * @brief Getting an image from a Binary Image File.
	 * @param BinaryFile Binary File that is going to convert to Cache Brush
	 * @param ResourceName The Resource Name or Path where to save the image
	 * @param OnReceived Lambda function when generate the image is successful.
	 */
	static void GetImageFromBytes(const TArray<uint8>& BinaryFile, const FString& ResourceName, const FOnImageReceived& OnReceived);
};
