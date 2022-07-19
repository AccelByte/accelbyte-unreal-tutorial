// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/AccelByteError.h"
#include "AccelByteStatisticObject.generated.h"

/**
 * UObject class to handle Statistic services, used in different levels.
 * This class provide these functionalities:
 * 
 * - Create Statistic Items from Client-side and Server-side
 * - Increment Statistic Items value from Client-side and Server-side
 * - Get Statistic Items from Client-side and Server-side
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteStatisticObject : public UObject
{
	GENERATED_BODY()

#pragma region Server SDK Statistic
	
private:
	/**
	 * @brief Create User's Statistic Items that need to Set By SERVER
	 * @param UserId User id of the player/client
	 * @param StatMap Map of User's StatItems information, consist of StatCode and its increment value
	 */
	void CreateUserServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap);

public:
	/**
	 * @brief Increment User's Statistic Items value that need to Set By SERVER
	 * @param UserId User id of the player/client
	 * @param StatMap Map of User's StatItems information, consist of StatCode and its increment value
	 * @param OnSuccess
	 * @param OnError
	 */
	void IncrementServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap, const AccelByte::FVoidHandler& OnSuccess, const AccelByte::FErrorHandler& OnError);

	/**
	 * @brief Retrieve User's Statistic Items from Server SDK
	 * @param UserId
	 * @param StatCodes 
	 * @param Tags 
	 */
	void GetServerStatistic(const FString& UserId, const TArray<FString>& StatCodes, const TArray<FString>& Tags);

#pragma endregion 
	
#pragma region Client SDK Statistic
	
private:
	/**
	 * @brief Create User's Statistic Items that need to Set By CLIENT
	 * @param StatMap Map of User's StatItems information, consist of StatCode and its increment value
	 */
	void CreateUserStatistic(const TMap<FString, int32>& StatMap);

public:
	/**
	 * @brief Increment User's Statistic Items value that need to Set By SERVER
	 * @param StatMap Map of User's StatItems information, consist of StatCode and its increment value
	 * @param OnSuccess
	 */
	void IncrementUserStatistic(const TMap<FString, int32>& StatMap, const AccelByte::FVoidHandler& OnSuccess);

	/**
	 * @brief Retrieve User's Statistic Items from Client SDK
	 * @param StatCodes 
	 * @param Tags 
	 */
	void GetUserStatistic(const TArray<FString>& StatCodes, const TArray<FString>& Tags);

#pragma endregion
	
};
