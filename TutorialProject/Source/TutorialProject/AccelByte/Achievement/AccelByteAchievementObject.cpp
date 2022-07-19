// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAchievementObject.h"

#include "Api/AccelByteAchievementApi.h"
#include "Core/AccelByteRegistry.h"

void UAccelByteAchievementObject::UnlockAchievement(const FString& AchievementCode, const FVoidHandler& OnSuccess)
{
	FRegistry::Achievement.UnlockAchievement(
		AchievementCode,
		FVoidHandler::CreateWeakLambda(this, 
			[this, OnSuccess]()
			{
				// Do something if UnlockAchievement has been successful
				OnSuccess.ExecuteIfBound();
			}),
		FErrorHandler:: CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Do something if UnlockAchievement has an error
				UE_LOG(LogTemp, Error, TEXT("Error UnlockAchievement, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}));
}

void UAccelByteAchievementObject::QueryUserAchievements(const EAccelByteAchievementListSortBy& SortBy, const int32& Offset, const int32& Limit, bool bPreferUnlocked, const FGetPlayerAchievementsData& OnSuccess)
{
	FRegistry::Achievement.QueryUserAchievements(
		SortBy,
		THandler<FAccelByteModelsPaginatedUserAchievement>::CreateWeakLambda(this, 
			[OnSuccess](const FAccelByteModelsPaginatedUserAchievement& Result)
			{
				// Do something if QueryUserAchievements has been successful
				OnSuccess.ExecuteIfBound(Result.Data);
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Do something if QueryUserAchievements has an error
				UE_LOG(LogTemp, Error, TEXT("Query User Achievements Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}),
		Offset,
		Limit,
		bPreferUnlocked);
}

void UAccelByteAchievementObject::QueryAllAchievements(const FString& Language, const EAccelByteAchievementListSortBy& SortBy, const int32& Offset, const int32& Limit, const FGetPublicAchievementsData& OnSuccess)
{
	FRegistry::Achievement.QueryAchievements(
		Language,
		SortBy,
		THandler<FAccelByteModelsPaginatedPublicAchievement>::CreateWeakLambda(this, 
			[OnSuccess](const FAccelByteModelsPaginatedPublicAchievement& Result)
			{
				// Do something if QueryAchievements has been successful
				OnSuccess.ExecuteIfBound(Result.Data);
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Do something if QueryAchievements has an error
				UE_LOG(LogTemp, Error, TEXT("Query All Achievements Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
			}),
		Offset,
		Limit);
}
