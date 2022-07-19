// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AccelByteUe4Sdk/Public/Models/AccelByteDSMModels.h"
#include "AccelByteUe4Sdk/Public/Models/AccelByteMatchmakingModels.h"
#include "AccelByteServerManager.generated.h"

/**
* Wrapper for DS Manager Code
* This code covers AccelByte services including :
*
* - Login With Client Credentials
* - Forget All Credentials
* - Register Local DS to DSM
* - Register DS to DSM
* - Get Session ID
* - Set Match ID
* - Get Match ID
* - Querry Session Status
* - Deregister Local DS from DSM
* - Send Shutdown to DSM
*/

class UGameInstance;

/*
 * These are delegates if Login with Credentials is success or not
 */
DECLARE_DELEGATE(FLoginWithCredentialsSuccess);
/*
 * These are delegates to handle if Register Local DS to DSM is success or failed
 */
DECLARE_DELEGATE(FRegisterLocalServerToDSMSuccess);
/*
 * These are delegates to handle if Register DS to DSM is success or failed
 */
DECLARE_DELEGATE(FRegisterServerToDSMSuccess);
/*
 * These are delegates to handle if Get Session Id is success or error
 */
DECLARE_DELEGATE_OneParam(FGetSessionIdSuccess, const FAccelByteModelsServerSessionResponse& SessionResponse);
/*
 * These are delegates to handle if Query Session Status is success or error
 */
DECLARE_DELEGATE_OneParam(FQuerySessionStatusSuccess, const FAccelByteModelsMatchmakingResult& MatchmakingResult);
/*
 * These are delegates to handle if Deregister Local DS from DSM is success or error
 */
DECLARE_DELEGATE(FDeregisterLocalServerFromDSMSuccess);
/*
 * These are delegates to handle if Send Shut Down to DSM is success or not
 */
DECLARE_DELEGATE(FSendShutDownToDSMSuccess);
/*
 * These are delegates to handle error for all callback
 */
DECLARE_DELEGATE_TwoParams(FSendFailedInfo, int32 ErrorCode, const FString& ErrorMessage);

UCLASS()
class TUTORIALPROJECT_API UAccelByteServerManager : public UObject
{
	GENERATED_BODY()

public:
	/*
	 * @brief Checking command line when DS launched.
	 */
	bool CheckCommandLineArgument(const FString& Argument);

	/*
	 * @brief Login Credentials for Dedicated Server.
	 */
	void LoginWithCredentials(const FLoginWithCredentialsSuccess& LoginWithCredentialsSuccess, const FSendFailedInfo& LoginWithCredentialsFailed);
	
	/*
	 * @brief Remove all registered Credentials from DS.
	 */
	void ForgetAllCredentials() const;
	
	/*
	 * @brief Registering Local DS to AccelByte Dedicated Server Manager.
	 */
	void RegisterLocalServerToDSM(const FRegisterLocalServerToDSMSuccess& RegisterLocalServerToDsmSuccess, const FSendFailedInfo& RegisterLocalServerToDsmFailed);

	/*
	 * @brief Registering DS to AccelByte Dedicated Server Manager.
	 */
	void RegisterServerToDSM(const FRegisterServerToDSMSuccess& RegisterServerToDsmSuccess, const FSendFailedInfo& RegisterServerToDsmFailed);

	/*
	 * @brief Get the Session ID currently run.
	 */
	void GetSessionId(const FGetSessionIdSuccess& GetSessionIdSuccess, const FSendFailedInfo& GetSessionIdError);
	
	/*
	 * @brief Get Match ID to our SDK Credentials.
	 */
	FString GetMatchId() const;

	/*
	 * @brief Set Match ID to our SDK Credentials.
	 */
	void SetMatchId(const FString& MatchId);

	/*
	 * @brief Query Session Status i.g (party id, party member) that has been set up before game DS logic.
	 */
	void QuerySessionStatus(const FQuerySessionStatusSuccess& QuerySessionStatusSuccess, const FSendFailedInfo& QuerySessionStatusError);

	/*
	 * @brief Deregister Local DS from Dedicated Server Manager when it's done being used to prevent
	 * bigger streamline budget when it's running.
	 */
	void DeregisterLocalServerFromDSM(const FDeregisterLocalServerFromDSMSuccess& DeregisterLocalServerFromDsmSuccess, const FSendFailedInfo& DeregisterLocalServerFromDsmError);

	/*
	 * @brief Shutdown the Dedicated Server which already in DSM.
	 */
	void SendShutdownToDSM(const FSendShutDownToDSMSuccess& SendShutDownToDsmSuccess, const FSendFailedInfo& SendShutdownToDsmError);

	/*
	* @brief Singleton UAccelByteServerManager class caller.
	*/
	static UAccelByteServerManager* CreateAccelByteServer(UGameInstance* GameInstance);

	/*
	* @brief ShutDown functions wrapper to be called inside game instance
	*/
	void ShutDown();

	/*
	* @brief GameServerLogin functions wrapper to be called inside game instance
	*/
	void GameServerLogin();
};
