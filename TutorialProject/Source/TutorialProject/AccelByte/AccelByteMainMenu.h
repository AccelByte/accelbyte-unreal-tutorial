// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "AccelByteMainMenu.generated.h"

class UButton;
class UTextBlock;
class USizeBox;
class ATutorialMenuHUD;

/**
 * Main Menu controller.
 */

UCLASS()
class TUTORIALPROJECT_API UAccelByteMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

	/**
	* @brief Button for Accessing Lobby Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Lobby;

	/**
	* @brief Button for Accessing Lobby Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Store;

	/**
	* @brief Button for Open the Inventory Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Inventory;

	/**
	* @brief Button for Open the Gallery Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Gallery;

	/**
	* @brief Button for Quit Game.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Logout;

	/**
	* @brief Button for Accessing Settings Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Settings;

	/**
	* @brief Button for Accessing Leaderboard Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Leaderboard;

	/**
	* @brief Button for Accessing Achievement Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Achievement;

	/**
	* @brief Text Box for displaying Player Name.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_PlayerName;

	/**
	* @brief Scrollbox to handle the max width and height of the Lobby button.
	*/	
	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Lobby;

	/**
	* @brief Scrollbox to handle the max width and height of the Logout button.
	*/
	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Logout;

	/**
	* @brief Quit Game functionality.
	*/
	UFUNCTION()
	void LogOut();

	/**
	* @brief A Tutorial Menu HUD to handle all instantiate and casting to the HUD framework
	*/
	ATutorialMenuHUD* TutorialMenuHUD;
};
