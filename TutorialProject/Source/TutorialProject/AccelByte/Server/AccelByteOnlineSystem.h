// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AccelByteOnlineSystem.generated.h"

/**
* Create Online Request Singleton.
*/
UCLASS()
class TUTORIALPROJECT_API UAccelByteOnlineSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
};

template <class T> T* CreateOnlineRequest(UGameInstance* Instance)
{
	if (Instance)
	{
		auto* System = Instance->GetSubsystem<UAccelByteOnlineSystem>();
		if (System)
		{
			return NewObject<T>(System);
		}
	}

	return nullptr;
}
