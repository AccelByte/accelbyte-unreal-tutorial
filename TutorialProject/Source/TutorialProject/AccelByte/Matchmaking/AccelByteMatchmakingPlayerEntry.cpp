// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteMatchmakingPlayerEntry.h"
#include "Components/TextBlock.h"
#include "Core/AccelByteRegistry.h"

void UAccelByteMatchmakingPlayerEntry::SetListItemObjectInternal(UObject* InObject)
{
	UAccelByteMatchmakingPlayerEntry* Entry = Cast<UAccelByteMatchmakingPlayerEntry>(InObject);

	if (!Entry->DisplayName.IsEmpty())
	{
		Tb_PlayerUsername->SetText(FText::FromString(Entry->DisplayName));
	}
}
