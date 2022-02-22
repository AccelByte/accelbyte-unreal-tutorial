// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteChatEntry.generated.h"

class UTextBlock;

/**
 * UserWidget class to handle chat entry.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteChatEntry : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialization

public:
	
	/**
	 * @brief Initialize chat entry data.
	 * @param ChatContent The content of chat as string.
	 */
	void InitData(const FString& ChatContent) const;

#pragma endregion 

#pragma region Widget Components
	
protected:
	
	/**
	 * @brief Text Block for chat content.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_ChatContent;

#pragma endregion 
	
};
