// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByte/Achievement/AccelByteAchievementPopUp.h"
#include "AccelByte/Server/AccelByteServerManager.h"
#include "Engine/GameInstance.h"
#include "TutorialProjectGameInstance.generated.h"

/**
* @brief This will handle the current state of the game mode that has selected.
*/
UENUM(BlueprintType)
enum class EGameMode : uint8
{
	ONEPLAYER	UMETA(DisplayName = "1v1"),
	TWOPLAYER	UMETA(DisplayName = "2v2"),
	THREEPLAYER	UMETA(DisplayName = "3v3"),
	FOURPLAYER	UMETA(DisplayName = "4v4"),
	FFA			UMETA(DisplayName = "FFA")
};

/**
 * When Game Instance spawned, Server will created
 * On server client.
 */
UCLASS()
class TUTORIALPROJECT_API UTutorialProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	
	virtual void Shutdown() override;

public:
	
	/**
	 * @brief Getter for Achievement Pop Up Box widget
	 */
	UAccelByteAchievementPopUp* GetAchievementPopUp() const;

protected:
	
	/**
	* @brief Accessing Server Manager wrapper.
	*/
	UPROPERTY()
	UAccelByteServerManager* ServerManager;

	/**
	 * @brief Pointer to Achievement Pop Up Box Widget
	 */
	UPROPERTY()
	UAccelByteAchievementPopUp* AccelByteAchievementPopUp;

	/**
	 * @brief Getter for Achievement Pop Up Box Class
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteAchievementPopUp> AccelByteAchievementPopUpClass;
};
