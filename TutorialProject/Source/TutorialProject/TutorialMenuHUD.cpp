// Copyright (c) 2021 - 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutorialMenuHUD.h"
#include "AccelByte/Authentication/AccelByteAuth.h"
#include "AccelByte/Lobby/AccelByteLobby.h"
#include "AccelByte/AccelByteMainMenu.h"
#include "AccelByte/Entitlement/AccelByteEntitlement.h"
#include "AccelByte/Friends/AccelbyteFindFriend.h"
#include "AccelByte/Friends/AccelByteFriends.h"
#include "AccelByte/Leaderboard/AccelByteLeaderboard.h"
#include "AccelByte/Legal/AccelByteLegalDocument.h"
#include "AccelByte/Party/AccelByteParty.h"
#include "AccelByte/Matchmaking/AccelByteMatchmaking.h"
#include "AccelByte/Matchmaking/AccelByteFindMatchPopUp.h"
#include "AccelByte/Store/AccelByteStore.h"
#include "AccelByte/Settings/AccelByteSettingsMenu.h"
#include "AccelByte/Wallet/AccelByteWallet.h"
#include "AccelByte/Order/AccelByteOrder.h"
#include "AccelByte/UserProfile/AccelByteUserProfile.h"
#include "AccelByte/Achievement/AccelByteAchievement.h"

void ATutorialMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	
	check(LoginMenuClass != nullptr);
	check(LegalMenuClass != nullptr);
	check(MainMenuClass != nullptr);
	check(LobbyMenuClass != nullptr);
	check(ChatMenuClass != nullptr);
	check(InventoryMenuClass != nullptr);
	check(GalleryMenuClass != nullptr);
	check(FriendsMenuClass != nullptr);
	check(FindFriendsMenuClass != nullptr);
	check(PartyClass != nullptr);
	check(MatchmakingClass != nullptr);
	check(FindMatchmakingClass != nullptr);
	check(StoreMenuClass != nullptr);
	check(SettingsMenuClass != nullptr);
	check(WalletClass != nullptr);
	check(OrderConfirmationClass != nullptr);
	check(LeaderboardMenuClass != nullptr);
	check(AchievementMenuClass != nullptr);
	
	LoginMenu = CreateWidget<UAccelByteAuth>(PlayerController, LoginMenuClass.Get());
	LegalMenu = CreateWidget<UAccelByteLegalDocument>(PlayerController, LegalMenuClass.Get());
	MainMenu = CreateWidget<UAccelByteMainMenu>(PlayerController, MainMenuClass.Get());
	LobbyMenu = CreateWidget<UAccelByteLobby>(PlayerController, LobbyMenuClass.Get());
	ChatMenu = CreateWidget<UAccelByteChat>(PlayerController, ChatMenuClass.Get());
	InventoryMenu = CreateWidget<UAccelByteEntitlement>(PlayerController, InventoryMenuClass.Get());
	GalleryMenu = CreateWidget<UAccelByteGallery>(PlayerController, GalleryMenuClass.Get());
	FriendsMenu = CreateWidget<UAccelByteFriends>(PlayerController, FriendsMenuClass.Get());
	FindFriendsMenu = CreateWidget<UAccelByteFindFriend>(PlayerController, FindFriendsMenuClass.Get());
	PartyMenu = CreateWidget<UAccelByteParty>(PlayerController, PartyClass.Get());
	MatchmakingMenu = CreateWidget<UAccelByteMatchmaking>(PlayerController, MatchmakingClass.Get());
	FindMatchmakingMenu = CreateWidget<UAccelByteFindMatchPopUp>(PlayerController, FindMatchmakingClass.Get());
	StoreMenu = CreateWidget<UAccelByteStore>(PlayerController, StoreMenuClass.Get());
	SettingsMenu = CreateWidget<UAccelByteSettingsMenu>(PlayerController, SettingsMenuClass.Get());
	UserProfileMenu = CreateWidget<UAccelByteUserProfile>(PlayerController, UserProfileClass.Get());
	WalletMenu = CreateWidget<UAccelByteWallet>(PlayerController, WalletClass.Get());
	OrderConfirmationMenu = CreateWidget<UAccelByteOrder>(PlayerController, OrderConfirmationClass.Get());
	LeaderboardMenu = CreateWidget<UAccelByteLeaderboard>(PlayerController, LeaderboardMenuClass.Get());
	AchievementMenu = CreateWidget<UAccelByteAchievement>(PlayerController, AchievementMenuClass.Get());

	if (!FRegistry::Credentials.GetUserId().IsEmpty())
	{
		InitLegal();
	}
	else
	{
		OpenLoginMenu();
	}
}

void ATutorialMenuHUD::OpenLoginMenu()
{
	LoginMenu->AddToViewport();
}

void ATutorialMenuHUD::OpenMainMenu()
{
	MainMenu->AddToViewport();
}

void ATutorialMenuHUD::InitMainMenu()
{
	// Add Lobby to Viewport and set its visibility as collapsed
	LobbyMenu->AddToViewport();
	LobbyMenu->AddPartyChild(PartyMenu);
	LobbyMenu->AddChatChild(ChatMenu);
	LobbyMenu->ConnectToLobby();
	LobbyMenu->SetVisibility(ESlateVisibility::Collapsed);
	
	OpenMainMenu();
	GalleryMenu->RefreshGallery();
}

void ATutorialMenuHUD::InitLegal()
{
	// Checking player is eligible for legal or not
	if (FRegistry::Credentials.GetAuthToken().Is_comply) 
	{
		InitGameSettings();
	}
	else 
	{
		LegalMenu->InitLegal();
	}
}

void ATutorialMenuHUD::InitGameSettings()
{
	SettingsMenu->InitGameSettings();
}

void ATutorialMenuHUD::InitUserProfile()
{
	UserProfileMenu->InitUserProfile();
}

void ATutorialMenuHUD::OpenLobbyMenu()
{
	LobbyMenu->SetVisibility(ESlateVisibility::Visible);
	PartyMenu->RefreshPartyList();
	MainMenu->RemoveFromParent();
}

void ATutorialMenuHUD::OpenInventoryMenu()
{
	InventoryMenu->AddToViewport();
	MainMenu->RemoveFromParent();
}

void ATutorialMenuHUD::OpenGalleryMenu()
{
	GalleryMenu->AddToViewport();
	MainMenu->RemoveFromParent();
}

void ATutorialMenuHUD::OpenMatchmakingMenu()
{
	LobbyMenu->SetVisibility(ESlateVisibility::Collapsed);
	MatchmakingMenu->AddToViewport();
}

void ATutorialMenuHUD::OpenFindMatchmakingPopUp()
{
	FindMatchmakingMenu->AddToViewport();
}

void ATutorialMenuHUD::OpenFriendsMenu()
{
	if (!FriendsMenu->IsInViewport())
	{
		FriendsMenu->AddToViewport();
	}
	else
	{
		FriendsMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		FriendsMenu->RefreshFriendsList();
	}
}

void ATutorialMenuHUD::OpenFindFriendsMenu()
{
	FindFriendsMenu->AddToViewport();
}

void ATutorialMenuHUD::OpenSettingsMenu()
{
	MainMenu->RemoveFromParent();
	SettingsMenu->AddToViewport();
}

void ATutorialMenuHUD::OpenOrderConfirmationPopUp(const FAccelByteModelsItemInfo& ItemInfo)
{
	OrderConfirmationMenu->AddToViewport();
	OrderConfirmationMenu->InitPopUp(ItemInfo);
}

void ATutorialMenuHUD::OpenLeaderboardMenu()
{
	LeaderboardMenu->AddToViewport();
	LeaderboardMenu->ResetWidget();
}

void ATutorialMenuHUD::OpenAchievementMenu()
{
	AchievementMenu->AddToViewport();
}

void ATutorialMenuHUD::CloseMainMenu()
{
	LoginMenu->AddToViewport();
	MainMenu->RemoveFromViewport();
	LobbyMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseFindFriendsMenu()
{
	FindFriendsMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseFindMatchmakingPopUp()
{
	FindMatchmakingMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseLobbyMenu()
{
	MainMenu->AddToViewport();
	LobbyMenu->SetVisibility(ESlateVisibility::Collapsed);
}

void ATutorialMenuHUD::CloseInventoryMenu()
{
	MainMenu->AddToViewport();
	InventoryMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseFriendMenu()
{
	if (!LobbyMenu->IsInViewport())
	{
		LobbyMenu->AddToViewport();
	}
	FriendsMenu->SetVisibility(ESlateVisibility::Collapsed);
}

void ATutorialMenuHUD::CloseSettingsMenu()
{
	if (!MainMenu->IsInViewport())
	{
		MainMenu->AddToViewport();
	}
	SettingsMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseOrderConfirmationPopUp()
{
	OrderConfirmationMenu->RemoveFromParent();
}

void ATutorialMenuHUD::OnCloseLobbyMenu()
{
	PartyMenu->RefreshPartyEntries();
	CloseLobbyMenu();
}

void ATutorialMenuHUD::OpenStoreMenu()
{
	StoreMenu->AddToViewport();
	MainMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseStoreMenu()
{
	if (!MainMenu->IsInViewport())
	{
		MainMenu->AddToViewport();
	}
	StoreMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseLeaderboardMenu()
{
	if (!MainMenu->IsInViewport())
	{
		MainMenu->AddToViewport();
	}
	LeaderboardMenu->RemoveFromParent();
}

void ATutorialMenuHUD::CloseAchievementMenu()
{
	if (!MainMenu->IsInViewport())
	{
		MainMenu->AddToViewport();
	}
	AchievementMenu->RemoveFromParent();
}
