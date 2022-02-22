// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteEcommerceModels.h"
#include "AccelByteWallet.generated.h"

class UTextBlock;

/**
 * Virtual Currency Widget
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteWallet : public UUserWidget
{
	GENERATED_BODY()

	protected:

	/**
	* @brief Text for showing current virtual currency balance.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Balance;

public:

	/**
	* @brief Update virtual currency balance count.
	*/
	UFUNCTION()
	void UpdateCurrency();
};
