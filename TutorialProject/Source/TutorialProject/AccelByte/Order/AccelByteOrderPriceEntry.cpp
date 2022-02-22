// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteOrderPriceEntry.h"
#include "Components/TextBlock.h"

void UAccelByteOrderPriceEntry::InitWidget(const FText& CurrencyCode, const FText& Price) const
{
	T_CurrencyCode->SetText(CurrencyCode);
	T_Price->SetText(Price);
}
