// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteStatisticObject.h"

#include "Api/AccelByteStatisticApi.h"
#include "Core/AccelByteRegistry.h"
#include "GameServerApi/AccelByteServerStatisticApi.h"
#include "Misc/DefaultValueHelper.h"
#include "Models/AccelByteStatisticModels.h"
#include "TutorialProject/TutPlayerControllerCountGame.h"

#pragma region Server SDK Statistic

void UAccelByteStatisticObject::CreateUserServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap)
{
	// Prepare the User's Stat Items using the StatCodes' info from StatMap
	TArray<FString> StatCodes;
	StatMap.GenerateKeyArray(StatCodes);

	FRegistry::ServerStatistic.CreateUserStatItems(
		UserId,
		StatCodes,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateWeakLambda(this, 
			[this, UserId, StatMap](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				UE_LOG(LogTemp, Log, TEXT("[ServerSDK] Successfully create statistic item for user"));

				// Call the Increment Statistic again since the stat items have existed
				IncrementServerStatistic(
					UserId,
					StatMap,
					FVoidHandler::CreateWeakLambda(this, [](){}),
					FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage) {}));
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				UE_LOG(LogTemp, Error, TEXT("[ServerSDK] Error CreateUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
}

void UAccelByteStatisticObject::IncrementServerStatistic(const FString& UserId, const TMap<FString, int32>& StatMap, const FVoidHandler& OnSuccess, const FErrorHandler& OnError)
{
	// Prepare Stat Items and their increment value using the Stat Info from StatMap
	TArray<FAccelByteModelsBulkStatItemInc> Data;
	for (const TPair<FString, int32>& StatCode : StatMap)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ServerSDK] StatCode %s"), *StatCode.Key);
		FAccelByteModelsBulkStatItemInc StatData;
		StatData.statCode = StatCode.Key;
		StatData.inc = StatCode.Value;
		Data.Add(StatData);
	}
	
	FRegistry::ServerStatistic.IncrementUserStatItems(
		UserId,
		Data,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateWeakLambda(this, 
			[this, OnSuccess](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				// Print the log if successful
				UE_LOG(LogTemp, Log, TEXT("[ServerSDK] Successfully increment value of user's statistic item"));

				OnSuccess.ExecuteIfBound();
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[this, UserId, StatMap, OnError](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("[ServerSDK] Error IncrementUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

				// Create the necessary Stat Items for the user
				CreateUserServerStatistic(UserId, StatMap);

				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));
}

void UAccelByteStatisticObject::GetServerStatistic(const FString& UserId, const TArray<FString>& StatCodes, const TArray<FString>& Tags)
{
	FRegistry::ServerStatistic.GetUserStatItems(
		UserId,
		StatCodes,
		Tags,
		THandler<FAccelByteModelsUserStatItemPagingSlicedResult>::CreateWeakLambda(this, 
			[](const FAccelByteModelsUserStatItemPagingSlicedResult& Result)
			{
				// Print the log if successful
				UE_LOG(LogTemp, Log, TEXT("[ServerSDK] Successfully retrieve user's statistic items"));
			}),
			FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				UE_LOG(LogTemp, Error, TEXT("[ServerSDK] Error GetUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
}

#pragma endregion 

#pragma region Client SDK Statistic

void UAccelByteStatisticObject::CreateUserStatistic(const TMap<FString, int32>& StatMap)
{
	// Prepare the User's Stat Items using the StatCodes' info from StatMap
	TArray<FString> StatCodes;
	StatMap.GenerateKeyArray(StatCodes);

	FRegistry::Statistic.CreateUserStatItems(
		StatCodes,
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateWeakLambda(this, 
			[this, StatMap](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				UE_LOG(LogTemp, Log, TEXT("[ClientSDK] Successfully create statistic item for user"));

				// Call the Increment Statistic again since the stat items have existed
				IncrementUserStatistic(
					StatMap,
					FVoidHandler::CreateWeakLambda(this, [](){}));
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				UE_LOG(LogTemp, Error, TEXT("[ClientSDK] Error CreateUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
}

void UAccelByteStatisticObject::IncrementUserStatistic(const TMap<FString, int32>& StatMap, const FVoidHandler& OnSuccess)
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
		THandler<TArray<FAccelByteModelsBulkStatItemOperationResult>>::CreateWeakLambda(this, 
			[this, Data, OnSuccess](const TArray<FAccelByteModelsBulkStatItemOperationResult>& Result)
			{
				// Print the log if successful
				UE_LOG(LogTemp, Log, TEXT("[ClientSDK] Successfully increment value of user's statistic item"));

				OnSuccess.ExecuteIfBound();
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[this, StatMap](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("[ClientSDK] Error IncrementUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

				// Create the necessary Stat Items for the user
				CreateUserStatistic(StatMap);
			}));
}

void UAccelByteStatisticObject::GetUserStatistic(const TArray<FString>& StatCodes, const TArray<FString>& Tags)
{
	FRegistry::Statistic.GetUserStatItems(
		StatCodes,
		Tags,
		THandler<FAccelByteModelsUserStatItemPagingSlicedResult>::CreateWeakLambda(this, 
			[](const FAccelByteModelsUserStatItemPagingSlicedResult& Result)
			{
				// Print the log if successful
				UE_LOG(LogTemp, Log, TEXT("[ClientSDK] Successfully retrieve user's statistic items"));
			}),
			FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Print the Error Code and Message if error happened
				UE_LOG(LogTemp, Error, TEXT("[ClientSDK] Error GetUserStatItems, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
}

#pragma endregion