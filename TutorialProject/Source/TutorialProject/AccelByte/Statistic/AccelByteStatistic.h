// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteStatistic.generated.h"

/**
 * @brief UObject class to handle Statistic service
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteStatistic : public UObject
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
	 */
	void IncrementServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap);

	/**
	 * @brief Retrieve User's Statistic Items from Server SDK
	 * @param StatCodes 
	 * @param Tags 
	 */
	void GetServerStatistic(const FString& UserId, const TArray<FString>& StatCodes, const TArray<FString>& Tags) const;

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
	 */
	void IncrementUserStatistic(const TMap<FString, int32>& StatMap);

	/**
	 * @brief Retrieve User's Statistic Items from Client SDK
	 * @param StatCodes 
	 * @param Tags 
	 */
	void GetUserStatistic(const TArray<FString>& StatCodes, const TArray<FString>& Tags) const;

#pragma endregion 
};
