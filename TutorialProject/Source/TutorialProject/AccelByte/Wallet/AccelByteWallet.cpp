// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteWallet.h"
#include "Api/AccelByteWalletApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteWallet::UpdateCurrency()
{
	FRegistry::Wallet.GetWalletInfoByCurrencyCode(
		TutorialProjectUtilities::VirtualCurrencyCode,
		THandler<FAccelByteModelsWalletInfo>::CreateLambda(
			[this](const FAccelByteModelsWalletInfo& Response)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Success Get Wallet Profile: %d"), Response.Balance));
				Tb_Balance->SetText(FText::AsNumber(Response.Balance));
			}
		),
		FErrorHandler::CreateLambda(
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Fail Get Wallet Profile: %s (%d)"), &ErrorMessage, ErrorCode));
			}
		)
	);
}