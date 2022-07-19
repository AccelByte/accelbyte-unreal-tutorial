// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteLegalDocument.h"
#include "Api/AccelByteAgreementApi.h"
#include "Api/AccelByteUserApi.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Core/AccelByteRegistry.h"
#include "Models/AccelByteAgreementModels.h"
#include "TutorialProject/TutorialMenuHUD.h"

void UAccelByteLegalDocument::InitLegal()
{
	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());

	LegalState = 0;
	RequiredDocs.Empty();

	const FString Locale = FInternationalization::Get().GetCurrentLocale()->GetTwoLetterISOLanguageName();

	FRegistry::Agreement.QueryLegalEligibilities(FRegistry::Credentials.GetNamespace(), THandler<TArray<FAccelByteModelsRetrieveUserEligibilitiesResponse>>::CreateWeakLambda(this, [this, Locale](const TArray<FAccelByteModelsRetrieveUserEligibilitiesResponse>& Result)
	{
		for (const FAccelByteModelsRetrieveUserEligibilitiesResponse& Policy : Result)
		{
			if (!Policy.IsAccepted && (Policy.Namespace == FRegistry::Settings.Namespace || Policy.Namespace == FRegistry::Settings.PublisherNamespace))
			{
				if (!Policy.IsMandatory || Policy.PolicyVersions.Num() == 0)
				{
					continue;
				}
						
				for (const FAccelByteModelsPolicyVersionWithLocalizedVersionObject& PolicyVersion : Policy.PolicyVersions)
				{
					if (!PolicyVersion.IsInEffect)
					{
						continue;
					}

					// Find Localized Policy based on Player's Current Language
					FAccelByteModelsLocalizedPolicyVersionObject FindLocalizedPolicy;
					for (const FAccelByteModelsLocalizedPolicyVersionObject& LocalizedPolicy : PolicyVersion.LocalizedPolicyVersions)
					{
						if (Locale == LocalizedPolicy.LocaleCode)
						{
							FindLocalizedPolicy = LocalizedPolicy;
						}
					}

					FRequiredDocument RequiredDoc;
					
					if (FindLocalizedPolicy.Id.IsEmpty() && PolicyVersion.LocalizedPolicyVersions.Num() > 0)
					{
						RequiredDoc.DocumentUrl = Policy.BaseUrls[0] + PolicyVersion.LocalizedPolicyVersions[0].AttachmentLocation;
						RequiredDoc.LocalizedPolicyVersionId = PolicyVersion.LocalizedPolicyVersions[0].Id;
					}
					else
					{
						RequiredDoc.DocumentUrl = Policy.BaseUrls[0] + FindLocalizedPolicy.AttachmentLocation;
						RequiredDoc.LocalizedPolicyVersionId = FindLocalizedPolicy.Id;
					}
					
					RequiredDoc.Title = Policy.PolicyName;
					RequiredDoc.Version = PolicyVersion.DisplayVersion;
					RequiredDoc.CreatedAt = FString::Printf(TEXT("Date: %d-%d-%d"), PolicyVersion.CreatedAt.GetYear(), PolicyVersion.CreatedAt.GetMonth(), PolicyVersion.CreatedAt.GetDay());
					RequiredDoc.PolicyId = Policy.PolicyId;
					RequiredDoc.PolicyVersionId = PolicyVersion.Id;

					RequiredDocs.Add(RequiredDoc);
				}
			}
		}

		if (RequiredDocs.Num() == 0)
		{
			TutorialMenuHUD->InitGameSettings();
		}
		else
		{
			ShowLegalDocument();
			this->AddToViewport();
		}

		UE_LOG(LogTemp, Log, TEXT("Query Legal Eligibilities Success, Total Legal That Require to Accept: %d"), RequiredDocs.Num());
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Query Legal Eligibilities Failed, Error Code: %d, Error Message: %s"), ErrorCode, *ErrorMessage);
	}));
}

void UAccelByteLegalDocument::ShowLegalDocument()
{
	WS_LegalDocument->SetActiveWidgetIndex(0);

	FRegistry::Agreement.GetLegalDocument(RequiredDocs[LegalState].DocumentUrl, THandler<FString>::CreateWeakLambda(this, [this](const FString& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Success Get Legal Document"));
		
		WS_LegalDocument->SetActiveWidgetIndex(1);
		
		Tb_LegalTitle->SetText(FText::FromString(RequiredDocs[LegalState].Title));
		Tb_PolicyCreatedDate->SetText(FText::FromString(RequiredDocs[LegalState].CreatedAt));
		Tb_PolicyVersion->SetText(FText::FromString(RequiredDocs[LegalState].Version));
		Tb_LegalDescription->SetText(FText::FromString(Result));
		Tb_AgreementText->SetText(FText::FromString(FString::Printf(TEXT("I have read and agree to the [%s]"), *RequiredDocs[LegalState].Title)));
		
		Cb_Agreement->SetCheckedState(ECheckBoxState::Unchecked);
		Btn_Submit->SetIsEnabled(false);
		Btn_Submit->OnClicked.AddUniqueDynamic(this, &UAccelByteLegalDocument::OnSubmitButtonClicked);
		Cb_Agreement->OnCheckStateChanged.AddUniqueDynamic(this, &UAccelByteLegalDocument::OnCheckedLegalChanged);
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Get Legal Document Failed, Error Code: %d ErrorMessage: %s"), ErrorCode, *ErrorMessage);

	}));
}

void UAccelByteLegalDocument::OnSubmitButtonClicked()
{
	Btn_Submit->OnClicked.RemoveDynamic(this, &UAccelByteLegalDocument::OnSubmitButtonClicked);
	Cb_Agreement->OnCheckStateChanged.RemoveDynamic(this, &UAccelByteLegalDocument::OnCheckedLegalChanged);
	
	if (LegalState == RequiredDocs.Num() - 1)
	{
		WS_LegalDocument->SetActiveWidgetIndex(0);

		TArray<FAccelByteModelsAcceptAgreementRequest> AgreementRequests;
		for (const FRequiredDocument& RequiredDoc : RequiredDocs)
		{
			FAccelByteModelsAcceptAgreementRequest AgreementRequest;
			AgreementRequest.IsAccepted = true;
			AgreementRequest.PolicyId = RequiredDoc.PolicyId;
			AgreementRequest.PolicyVersionId = RequiredDoc.PolicyVersionId;
			AgreementRequest.LocalizedPolicyVersionId = RequiredDoc.LocalizedPolicyVersionId;

			AgreementRequests.Add(AgreementRequest);
		}
		
		FRegistry::Agreement.BulkAcceptPolicyVersions(AgreementRequests, THandler<FAccelByteModelsAcceptAgreementResponse>::CreateWeakLambda(this, [this](const FAccelByteModelsAcceptAgreementResponse& Result)
		{
			if (Result.Proceed)
			{
				UE_LOG(LogTemp, Log, TEXT("Success Bulk Accept Policy Version"));

				FRegistry::User.LoginWithRefreshToken(FVoidHandler::CreateWeakLambda(this, [this]()
				{
					UE_LOG(LogTemp, Log, TEXT("Success Login With Refresh Token"));

					RemoveFromParent();
					TutorialMenuHUD->InitGameSettings();
				}),
				FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
				{
					UE_LOG(LogTemp, Error, TEXT("Login With Refresh Token Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
				}));
			}
		}),
		FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Bulk Accept Policy Versions Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		}));
	}
	else
	{
		LegalState++;
		ShowLegalDocument();
	}
}

void UAccelByteLegalDocument::OnCheckedLegalChanged(bool bIsChecked)
{
	if (bIsChecked)
	{
		Btn_Submit->SetIsEnabled(true);
	}
	else
	{
		Btn_Submit->SetIsEnabled(false);
	}
}
