// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteCloudStorageModels.h"
#include "AccelByteGallery.generated.h"

class UImage;
class UTextBlock;
class UButton;

/**
 * Gallery Menu for Cloud Storage
 */

UCLASS()
class TUTORIALPROJECT_API UAccelByteGallery : public UUserWidget
{
	GENERATED_BODY()

protected:

	/**
	 * @brief Native construct to handle the delegate of back to main menu button
	 */
	virtual void NativeConstruct() override;

	/**
	 * @brief Image of screenshot to hold the image of current player screenshot
	 */
	UPROPERTY(meta =(BindWidget))
	UImage* I_Screenshot;

	/**
	 * @brief Text to hold the name of the screenshot, in this case our naming format is after the date and time it was taken
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_ScreenshotName;

	/**
	 * @brief Button to go back to main menu
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

	/**
	 * @brief Function to handle if the back to main menu button is clicked
	 */
	UFUNCTION()
	void OnBackToMainMenuButtonClicked();

	/**
	 * @brief Update the Interface of current gallery menu such as Image and Text of the screenshot
	 */
	void UpdateUserInterface();

	/**
	 * @brief Model slot to handle the slot of current player's cloud storage
	 */
	FAccelByteModelsSlot ModelSlot;

public:

	/**
	 * @brief Refresh Gallery when the first time player open the game or main menu
	 */
	void RefreshGallery();

	/**
	 * @brief Start to take a screenshot and save or update to a Cloud Storage
	 */
	void TakeScreenshot();
	
};
