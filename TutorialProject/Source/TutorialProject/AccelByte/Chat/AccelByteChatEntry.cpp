// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteChatEntry.h"
#include "Components/TextBlock.h"

#pragma region Initialization

void UAccelByteChatEntry::InitData(const FString& ChatContent) const
{
	Tb_ChatContent->SetText(FText::FromString(ChatContent));
}

#pragma endregion