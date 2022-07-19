// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteAuth.generated.h"

class UVerticalBox;
class UEditableTextBox;
class UButton;
class ATutorialMenuHUD;
class UTextBlock;

/**
* Component for logging in a user with the AccelByte back end as well as methods for grabbing information relating to that user.
* This code covers AccelByte services including :
*
* - Login with username
*/

UCLASS()
class TUTORIALPROJECT_API UAccelByteAuth : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

	/**
	* @brief Editable Text Box for Username inside MainMenu Widget.
	*/
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_Username;
	
	/**
	* @brief Editable Text Box for Password inside MainMenu Widget.
	*/
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_Password;
	
	/**
	* @brief Take Button Login inside MainMenu Widget.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Login;

	/**
	 * @brief Text Block to display Login Status 
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_LoginStatus;
	
	/**
	 * @brief Instantiate all casting to the main menu HUD
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	* @brief Logging in with launcher
	*/
	void LauncherLogin();

	/**
	 * @brief Logging in with steam
	 */
	void SteamLogin();
	
public:
	
	/**
	* @brief Log an account in using the AccelByte SDK. This is executed automatically on component construction unless
	* otherwise configured.
	*/
	UFUNCTION()
	void OnLoginButtonClicked();

	/**
	* @brief Logout a session using the AccelByte SDK. This is executed automatically on component construction unless
	* otherwise configured.
	*/
	UFUNCTION()
	void OnLogoutButtonClicked();

private:
	
	/**
	* @brief Function behaviour when Login success. This function called inside AccelByte Login OnSuccess
	* delegates inside lambda. Can be improved or changed how login behaviour works inside this function.
	*/
	void LoginSuccess();
	
	/**
	* @brief Function behaviour when Login failed. This function called inside AccelByte Login OnFailed
	* delegates inside lambda. Can be improved or changed how login behaviour works inside this function.
	*
	* @param ErrorCode error code HTTP request. e.g 404.
	* @param ErrorMessage error message HTTP request. e.g Unauthorized.
	* @param ErrorJson error message for OAuth
	*/
	void LoginFailed(int32 ErrorCode, const FString& ErrorMessage, const FJsonObject& ErrorJson);

	/**
	* @brief Function behaviour when Logout success. This function called inside AccelByte Logout OnSuccess
	* delegates inside lambda. Can be improved or changed how Logout behaviour works inside this function.
	*/
	void LogoutSuccess();
	
	/**
	* @brief Function behaviour when Logout failed. This function called inside AccelByte Logout OnFailed
	* delegates inside lambda. Can be improved or changed how Logout behaviour works inside this function.
	*
	* @param ErrorCode error code HTTP request. e.g 404.
	* @param ErrorMessage error message HTTP request. e.g Unauthorized.
	*/
	void LogoutFailed(int32 ErrorCode, const FString& ErrorMessage);

};
