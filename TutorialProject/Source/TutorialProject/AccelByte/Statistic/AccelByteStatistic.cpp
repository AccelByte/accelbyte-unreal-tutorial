// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteStatistic.h"

#include "Api/AccelByteStatisticApi.h"
#include "Core/AccelByteRegistry.h"
#include "GameServerApi/AccelByteServerStatisticApi.h"
#include "Models/AccelByteStatisticModels.h"
#include "TutorialProject/TutorialProjectUtilities.h"

#pragma region Server SDK Statistic

void UAccelByteStatistic::CreateUserServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap)
{
	// Prepare the User's Stat Items using the StatCodes' info from StatMap
	TArray<FString> StatCodes;
	StatMap.GenerateKeyArray(StatCodes);

	FRegistry::ServerStatistic.CreateUserStatItems(
		UserId,
		StatCodes,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateLambda(
			[this, UserId, StatMap](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ServerSDK] Successfully create statistic item for user")));

				// Call the Increment Statistic again since the stat items have existed
				IncrementServerStatistic(UserId, StatMap);
			}),
		FErrorHandler::CreateLambda(
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ServerSDK] Error CreateUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
			}));
}

void UAccelByteStatistic::IncrementServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap)
{
	// Prepare Stat Items and their increment value using the Stat Info from StatMap
	TArray<FAccelByteModelsBulkStatItemInc> Data;
	for (const TPair<FString, int32>& StatCode : StatMap)
	{
		FAccelByteModelsBulkStatItemInc StatData;
		StatData.statCode = StatCode.Key;
		StatData.inc = StatCode.Value;
		Data.Add(StatData);
	}
	
	FRegistry::ServerStatistic.IncrementUserStatItems(
		UserId,
		Data,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateLambda(
			[](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				// Print the log if successful
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ServerSDK] Successfully increment value of user's statistic item")));
			}),
		FErrorHandler::CreateLambda(
			[this, UserId, StatMap](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ServerSDK] Error IncrementUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));

				// Create the necessary Stat Items for the user
				CreateUserServerStatistic(UserId, StatMap);
			}));
}

void UAccelByteStatistic::GetServerStatistic(const FString& UserId, const TArray<FString>& StatCodes, const TArray<FString>& Tags) const
{
	FRegistry::ServerStatistic.GetUserStatItems(
		UserId,
		StatCodes,
		Tags,
		THandler<FAccelByteModelsUserStatItemPagingSlicedResult>::CreateLambda(
			[](const FAccelByteModelsUserStatItemPagingSlicedResult& Result)
			{
				// Print the log if successful
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ServerSDK] Successfully retrieve user's statistic items")));
			}),
			FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ServerSDK] Error GetUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
			}));
}

#pragma endregion 

#pragma region Client SDK Statistic

void UAccelByteStatistic::CreateUserStatistic(const TMap<FString, int32>& StatMap)
{
	// Prepare the User's Stat Items using the StatCodes' info from StatMap
	TArray<FString> StatCodes;
	StatMap.GenerateKeyArray(StatCodes);

	FRegistry::Statistic.CreateUserStatItems(
		StatCodes,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateLambda(
			[this, StatMap](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ClientSDK] Successfully create statistic item for user")));

				// Call the Increment Statistic again since the stat items have existed
				IncrementUserStatistic(StatMap);
			}),
		FErrorHandler::CreateLambda(
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ClientSDK] Error CreateUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
			}));
}

void UAccelByteStatistic::IncrementUserStatistic(const TMap<FString, int32>& StatMap)
{
	// Prepare Stat Items and their increment value using the Stat Info from StatMap
	TArray<FAccelByteModelsBulkStatItemInc> Data;
	for (const TPair<FString, int32>& StatCode : StatMap)
	{
		FAccelByteModelsBulkStatItemInc StatData;
		StatData.statCode = StatCode.Key;
		StatData.inc = StatCode.Value;
		Data.Add(StatData);
	}

	FRegistry::Statistic.IncrementUserStatItems(
		Data,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateLambda(
			[](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				// Print the log if successful
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ClientSDK] Successfully increment value of user's statistic item")));
			}),
		FErrorHandler::CreateLambda(
			[this, StatMap](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ClientSDK] Error IncrementUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));

				// Create the necessary Stat Items for the user
				CreateUserStatistic(StatMap);
			}));
}

void UAccelByteStatistic::GetUserStatistic(const TArray<FString>& StatCodes, const TArray<FString>& Tags) const
{
	FRegistry::Statistic.GetUserStatItems(
		StatCodes,
		Tags,
		THandler<FAccelByteModelsUserStatItemPagingSlicedResult>::CreateLambda(
			[](const FAccelByteModelsUserStatItemPagingSlicedResult& Result)
			{
				// Print the log if successful
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ClientSDK] Successfully retrieve user's statistic items")));
			}),
			FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("[ClientSDK] Error GetUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
			}));
}

#pragma endregion 