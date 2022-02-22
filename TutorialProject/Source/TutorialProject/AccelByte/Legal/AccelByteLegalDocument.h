#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteLegalDocument.generated.h"

/**
 * @brief All legal document that required for player to accept the document
 */
USTRUCT()
struct FRequiredDocument
{
	GENERATED_BODY()

	FString Title;
	FString CreatedAt;
	FString Version;
	FString DocumentUrl;
	FString PolicyId;
	FString PolicyVersionId;
	FString LocalizedPolicyVersionId;
};

class UTextBlock;
class UCheckBox;
class UButton;
class ATutorialMenuHUD;
class UWidgetSwitcher;

/**
 * Legal Document is used to check if the current state of player is already comply to the legal agreement or policy or not
 */

UCLASS()
class TUTORIALPROJECT_API UAccelByteLegalDocument : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_LegalTitle;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_LegalDescription;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_PolicyCreatedDate;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_PolicyVersion;

	UPROPERTY(meta =(BindWidget))
	UCheckBox* Cb_Agreement;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_AgreementText;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Submit;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_LegalDocument;

	ATutorialMenuHUD* TutorialMenuHUD;

	int32 LegalState;

	TArray<FRequiredDocument> RequiredDocs;

	/**
	 * @brief Showing the legal document from the URL that we retrieve from query eligibilities.
	 */
	UFUNCTION()
	void ShowLegalDocument();

	/**
	 * @brief Try to bulk accept the policy if all required docs is accepted by he player, then try to refresh the new token.
	 */
	UFUNCTION()
	void OnSubmitButtonClicked();

	/**
	 * @brief Try to check if the player checked the legal or not.
	 * @param bIsChecked Is the current state is checked or not.
	 */
	UFUNCTION()
	void OnCheckedLegalChanged(bool bIsChecked);
	
public:

	/**
	 * @brief call QueryLegalEligibilities() function to get all updated legal documents.
	 */
	UFUNCTION()
	void InitLegal();
};
