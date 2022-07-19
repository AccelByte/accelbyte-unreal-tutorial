// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByteAchievementEntry.h"
#include "AccelByteAchievementObject.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteAchievementModels.h"
#include "AccelByteAchievement.generated.h"

class UAccelByteAchievementPopUpEntry;
class ATutorialMenuHUD;
class UGridPanel;
class UWidgetSwitcher;
class UWidget;
class UButton;

/**
 * Achievement Menu Widget.
 * This class provide this functionalities:
 *
 * - Display all Achievements
 * - Get Player's Achievements data
 * - Get All Achievements basic data
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteAchievement : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	/**
	 * @brief This to handle reference of HUD framework
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Pointer to Achievement Object
	 */
	UPROPERTY()
	UAccelByteAchievementObject* AccelByteAchievementObject;

#pragma region UI Binding

	/**
	 * @brief Widget Switcher for Achievement Menu List
	 */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_AchievementMenu;

	/**
	 * @brief Loading widget for Achievement Menu List
	 */
	UPROPERTY(meta = (BindWidget))
	UWidget* CT_Loading;

	/**
	 * @brief Content widget for Achievement Menu List
	 */
	UPROPERTY(meta = (BindWidget))
	UWidget* ScB_AchievementsList;
	
	/**
	 * @brief Grid Panel for Achievement Menu List
	 */
	UPROPERTY(meta = (BindWidget))
	UGridPanel* GP_Achievements;

	/**
	 * @brief Button to Back to Main Menu
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

#pragma endregion

public:
	
	/**
	 * @brief Achievement Entry widget class
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteAchievementEntry> AchievementEntryClass;
	
	/**
	 * @brief Maximum column value for Achievement Menu List's Grid Panel
	 */
	UPROPERTY(EditDefaultsOnly)
	uint8 MaximumGridColumn;
	
#pragma region UI Display

private:

	/**
	 * @brief Display Achievement List
	 */
	void DisplayAchievement();

	/**
	 * @brief Initialize and display Achievement Entry to Achievement List 
	 * @param PlayerAchievements Player's Achievements Data
	 * @param PublicAchievements Basic's Achievements Data
	 */
	void InitAchievementEntry(const TArray<FAccelByteModelsUserAchievement>& PlayerAchievements, const TArray<FAccelByteModelsPublicAchievement>& PublicAchievements);
	
#pragma endregion 
	
#pragma region Achievement SDK

private:
	
	/**
	 * @brief Get Player's Achievements data
	 * @param SortBy sort method for the query result
	 * @param Offset offset value of the query result
	 * @param Limit limit value of the query result
	 * @param PreferUnlocked bool value of list order method with Unlocked Achievements as priority
	 */
	void QueryUserAchievements(const EAccelByteAchievementListSortBy& SortBy, const int32& Offset, const int32& Limit, bool bPreferUnlocked);

	/**
	 * @brief Get All Achievements basic data
	 * @param Language default language setting
	 * @param SortBy sort method for the query result
	 * @param Offset offset value of the query result
	 * @param Limit limit value of the query result
	 * @param UserAchievements Player's Achievement Data
	 */
	void QueryAllAchievements(const FString& Language, const EAccelByteAchievementListSortBy& SortBy, const int32& Offset, const int32& Limit, const TArray<FAccelByteModelsUserAchievement>& UserAchievements);
	
#pragma endregion
	
};