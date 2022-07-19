// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutorialProjectGameInstance.h"

#include "TutorialProjectUtilities.h"
#include "Api/AccelByteLobbyApi.h"
#include "Core/AccelByteRegistry.h"

void UTutorialProjectGameInstance::Init()
{
	Super::Init();

	ServerManager = UAccelByteServerManager::CreateAccelByteServer(this);

	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogTemp, Log, TEXT("Check If Running on DS"));
		
		ServerManager->GameServerLogin();
	}
	else
	{
		if (!AccelByteAchievementPopUp)
		{
			AccelByteAchievementPopUp = CreateWidget<UAccelByteAchievementPopUp>(this, AccelByteAchievementPopUpClass.Get());
			check(AccelByteAchievementPopUp);
		}
	}
}

void UTutorialProjectGameInstance::Shutdown()
{
	if (IsRunningDedicatedServer())
	{
		ServerManager->ShutDown();
	}
	
	Super::Shutdown();
}

UAccelByteAchievementPopUp* UTutorialProjectGameInstance::GetAchievementPopUp() const
{
	if (AccelByteAchievementPopUp)
	{
		return AccelByteAchievementPopUp;
	}
	return nullptr;
}
