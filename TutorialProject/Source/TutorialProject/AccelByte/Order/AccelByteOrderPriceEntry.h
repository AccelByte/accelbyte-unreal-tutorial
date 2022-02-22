// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteOrderPriceEntry.generated.h"

class UTextBlock;

/**
 * Entry widget to show currency code and price of the current purchase order.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteOrderPriceEntry : public UUserWidget
{
	GENERATED_BODY()
	
private:
	/**
	 * @brief Text Box to show currency code
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_CurrencyCode;
	
	/**
	 * @brief Text Box to show price
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Price;

public:
	/**
	* @brief Widget variable and component setup
	*/
	void InitWidget(const FText& CurrencyCode, const FText& Price) const;
};
