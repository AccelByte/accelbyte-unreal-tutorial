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
		THandler<FAccelByteModelsWalletInfo>::CreateWeakLambda(this, 
			[this](const FAccelByteModelsWalletInfo& Response)
			{
				UE_LOG(LogTemp, Log, TEXT("Success Get Wallet Profile: %d"), Response.Balance);
				Tb_Balance->SetText(FText::AsNumber(Response.Balance));
			}
		),
		FErrorHandler::CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Log, TEXT("Fail Get Wallet Profile: %s (%d)"), &ErrorMessage, ErrorCode);
			}
		)
	);
}