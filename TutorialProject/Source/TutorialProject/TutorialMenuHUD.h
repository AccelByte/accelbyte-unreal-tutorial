// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByte/Gallery/AccelByteGallery.h"
#include "GameFramework/HUD.h"
#include "Models/AccelByteEcommerceModels.h"
#include "TutorialMenuHUD.generated.h"

class UAccelByteStore;
class UAccelByteAuth;
class UAccelByteLegalDocument;
class UAccelByteLobby;
class UAccelByteChat;
class UAccelByteEntitlement;
class UAccelByteEntitlementEntry;
class UAccelByteMainMenu;
class UAccelByteFriends;
class UAccelByteFindFriend;
class UAccelByteParty;
class UAccelByteMatchmaking;
class UAccelByteFindMatchPopUp;
class UAccelByteMatchmakingPlayerEntry;
class UAccelByteSettingsMenu;
class UAccelByteWallet;
class UAccelByteOrder;
class UAccelByteOrderPriceEntry;
class UAccelByteOrderPurchaseEntry;

/**
 * Menu Widget Controller. All Widget functionality controlled here.
 */
UCLASS()
class TUTORIALPROJECT_API ATutorialMenuHUD : public AHUD
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

public:
	
	/**
	* @brief Shows Login Menu on screen
	*/
	void OpenLoginMenu();
	
	/**
	* @brief Shows Main Menu on screen and destroys Login Menu
	*/
	void OpenMainMenu();

	/**
	 * @brief Init the construct function when the first time open main menu;
	 */
	void InitMainMenu();

	/**
	* @brief Shows Legal Menu on screen if the player is not comply already.
	*/
	void InitLegal();
	
	/**
	* @brief Init game settings to call cloud save services.
	*/
	void InitGameSettings();
	
	/**
	* @brief Shows Lobby Menu which adds Party Menu in Sb_Party and destroys Main Menu
	*/
	UFUNCTION()
	void OpenLobbyMenu();

	/**
	* @brief Shows Inventory Menu to show all player's inventory
	*/
	UFUNCTION()
	void OpenInventoryMenu();

	/**
	* @brief Shows Gallery Menu to show player's gallery
	*/
	UFUNCTION()
	void OpenGalleryMenu();
	
	/**
	* @brief Shows Friends Menu on screen
	*/
	void OpenFriendsMenu();
	
	/**
	* @brief Shows Find Friends Menu on screen
	*/
	void OpenFindFriendsMenu();
	
	/**
	* @brief Shows Matchmaking Menu on screen
	*/
	void OpenMatchmakingMenu();
	
	/**
	* @brief Shows Find Matchmaking Popup on screen
	*/
	void OpenFindMatchmakingPopUp();

	/**
	* @brief Shows Settings Menu on screen
	*/
	UFUNCTION()
	void OpenSettingsMenu();

	/**
	* @brief Shows Order confirmation pop up
	*/
	UFUNCTION()
	void OpenOrderConfirmationPopUp(const FAccelByteModelsItemInfo& ItemInfo);

	/**
	* @brief Destroys Main Menu widget
	*/
	void CloseMainMenu();
	
	/**
	* @brief Destroys Lobby Menu widget and shows Main Menu
	*/
	void CloseLobbyMenu();

	/**
	* @brief Destroys Inventory Menu widget and shows Main Menu
	*/
	UFUNCTION()
	void CloseInventoryMenu();
	
	/**
	* @brief Destroys Friend Menu widget
	*/
	void CloseFriendMenu();
	
	/**
	* @brief Destroys Find Friend Menu widget
	*/
	void CloseFindFriendsMenu();
	
	/**
	* @brief Destroys Find Matchmaking Popup widget
	*/
	void CloseFindMatchmakingPopUp();

	/**
	* @brief Destroys Settings Menu widget
	*/
	void CloseSettingsMenu();

	/**
	* @brief Destroys Order confirmation Popup
	*/
	UFUNCTION()
	void CloseOrderConfirmationPopUp();

	/**
	* @brief Refreshes party entries and close lobby menu
	*/
	void OnCloseLobbyMenu();

	/**
	* @brief Refreshes party entries and close lobby menu
	*/
	UFUNCTION()
	void OpenStoreMenu();

	/**
	* @brief Refreshes party entries and close lobby menu
	*/
	void CloseStoreMenu();

protected:
	
	/**
	* @brief Login Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteAuth> LoginMenuClass;

	/**
	* @brief Legal Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteLegalDocument> LegalMenuClass;
	
	/**
	* @brief Lobby Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteLobby> LobbyMenuClass;
	
	/**
	* @brief Chat Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteChat> ChatMenuClass;

	/**
	* @brief Inventory Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteEntitlement> InventoryMenuClass;

	/**
	* @brief Gallery Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteGallery> GalleryMenuClass;

	/**
	* @brief Inventory  Item widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteEntitlementEntry> InventoryItemClass;
	
	/**
	* @brief Main Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteMainMenu> MainMenuClass;
	
	/**
	* @brief Friends Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteFriends> FriendsMenuClass;
	
	/**
	* @brief Find Friends Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteFindFriend> FindFriendsMenuClass;
	
	/**
	* @brief Party Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteParty> PartyClass;
	
	/**
	* @brief Matchmaking Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteMatchmaking> MatchmakingClass;
	
	/**
	* @brief Find Matchmaking Popup widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteFindMatchPopUp> FindMatchmakingClass;

	/**
	* @brief Matchmaking Player Entry widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteMatchmakingPlayerEntry> MatchmakingPlayerEntryClass;

	/**
	* @brief Store Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteStore> StoreMenuClass;
	
	/**
	* @brief Settings Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteSettingsMenu> SettingsMenuClass;

	/**
	* @brief Wallet widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteWallet> WalletClass;

	/**
	* @brief Order confirmation widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteOrder> OrderConfirmationClass;

	/**
	* @brief Order confirmation widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteOrderPriceEntry> OrderConfirmationPriceEntryClass;

	/**
	* @brief Order purchase button widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteOrderPurchaseEntry> OrderConfirmationPurchaseEntryClass;	
public:
	
	/**
	* @brief Getter for Login Menu widget
	*/
	UAccelByteAuth* GetLoginMenu() const {return LoginMenu; }
	
	/**
	* @brief Getter for Main Menu widget
	*/
	UAccelByteMainMenu* GetMainMenu() const {return MainMenu; }
	
	/**
	* @brief Getter for Lobby Menu widget
	*/
	UAccelByteLobby* GetLobbyMenu() const {return LobbyMenu; }

	/**
	* @brief Getter for Gallery Menu widget
	*/
	UAccelByteGallery* GetGalleryMenu() const {return GalleryMenu; }
	
	/**
	* @brief Getter for Lobby Menu widget
	*/
	UAccelByteChat* GetChatMenu() const {return ChatMenu; }
	
	/**
	* @brief Getter for Friends Menu widget
	*/
	UAccelByteFriends* GetFriendsMenu() const {return FriendsMenu; }
	
	/**
	* @brief Getter for Find Friends Menu widget
	*/
	UAccelByteFindFriend* GetFindFriendsMenu() const {return FindFriendsMenu; }
	
	/**
	* @brief Getter for Party Menu widget
	*/
	UAccelByteParty* GetPartyMenu() const {return PartyMenu; }
	
	/**
	* @brief Getter for Matchmaking Menu widget
	*/
	UAccelByteMatchmaking* GetMatchmakingMenu() const {return MatchmakingMenu; }
	
	/**
	* @brief Getter for FindMatchmaking Popup widget
	*/
	UAccelByteFindMatchPopUp* GetFindMatchmakingMenu() const {return FindMatchmakingMenu; }

	/**
	* @brief Getter for Wallet Menu widget
	*/
	UAccelByteWallet* GetWalletMenu() const { return WalletMenu; }
	
	/**
	* @brief Getter for Order Popup widget
	*/
	UAccelByteOrder* GetOrderConfirmationMenu() const {return OrderConfirmationMenu; }	
	/**
	* @brief Getter for Matchmaking Player Entry Class
	*/
	TSubclassOf<UAccelByteMatchmakingPlayerEntry> GetMatchmakingPlayerEntryClass() const {return MatchmakingPlayerEntryClass; }
	
	/**
	* @brief Getter for Store Menu widget
	*/
	UAccelByteStore* GetStoreMenu() const {return StoreMenu; }
	
	/**
	* @brief Getter for Inventory Menu Widget
	*/
	UAccelByteEntitlement* GetInventoryMenu() const {return InventoryMenu; }

	/**
	* @brief Getter for Order Confirmation Price Entry Class
	*/
	TSubclassOf<UAccelByteOrderPriceEntry> GetOrderPriceEntryClass() const {return OrderConfirmationPriceEntryClass; }

	/**
	* @brief Getter for Order Confirmation Purchase Button Entry Class
	*/
	TSubclassOf<UAccelByteOrderPurchaseEntry> GetOrderPurchaseEntryClass() const {return OrderConfirmationPurchaseEntryClass; }
	
	/**
	* @brief Getter for Inventory Item Class
	*/
	TSubclassOf<UAccelByteEntitlementEntry> GetInventoryItemClass() const {return InventoryItemClass; }

	/**
	 * @brief Getter for Settings Menu widget
	 */
	UAccelByteSettingsMenu* GetSettingsMenu() const {return SettingsMenu; }
	
private:
	
	/**
	* @brief Login Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteAuth* LoginMenu;

	/**
	* @brief Legal Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteLegalDocument* LegalMenu;
	
	/**
	* @brief Main Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteMainMenu* MainMenu;
	
	/**
	* @brief Lobby Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteLobby* LobbyMenu;
	
	/**
	* @brief Chat Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteChat* ChatMenu;

	/**
	* @brief Inventory Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteEntitlement* InventoryMenu;

	/**
	* @brief Gallery Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteGallery* GalleryMenu;
	
	/**
	* @brief Friends Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteFriends* FriendsMenu;
	
	/**
	* @brief Find Friends Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteFindFriend* FindFriendsMenu;
	
	/**
	* @brief Party Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteParty* PartyMenu;
	
	/**
	* @brief Matchmaking Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteMatchmaking* MatchmakingMenu;
	
	/**
	* @brief Find Matchmaking Popup widget pointer
	*/
	UPROPERTY()
	UAccelByteFindMatchPopUp* FindMatchmakingMenu;

	/**
	* @brief Store Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteStore* StoreMenu;
	
	/**
	* @brief Settings Menu widget pointer
	*/
	UPROPERTY()
	UAccelByteSettingsMenu* SettingsMenu;

	/**
	* @brief Virtual Currency wallet widget pointer
	*/
	UPROPERTY()
	UAccelByteWallet* WalletMenu;
	
	/**
	* @brief Order Menu popup widget pointer
	*/
	UPROPERTY()
	UAccelByteOrder* OrderConfirmationMenu;
};