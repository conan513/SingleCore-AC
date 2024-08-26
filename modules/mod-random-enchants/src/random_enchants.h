/*
* Converted from the original LUA script to a module for Azerothcore (Sunwell)
*/

#include "ScriptMgr.h"
#include "Player.h"
#include "Configuration/Config.h"
#include "Chat.h"

enum itemQuality
{
    GREY = 0,
    WHITE,
    GREEN,
    BLUE,
    PURPLE,
    ORANGE
};

void rollPossibleEnchant(Player* player, Item* item);
[[nodiscard]] uint32 getRandEnchantment(Item* item);

class RandomEnchantsPlayer : public PlayerScript
{
public:
    RandomEnchantsPlayer() : PlayerScript("RandomEnchantsPlayer") { }

    void OnLogin(Player* player) override;
    void OnLootItem(Player* player, Item* item, uint32 /*count*/, ObjectGuid /*lootguid*/) override;
    void OnCreateItem(Player* player, Item* item, uint32 /*count*/) override;
    void OnQuestRewardItem(Player* player, Item* item, uint32 /*count*/) override;
    void OnGroupRollRewardItem(Player* player, Item* item, uint32 /*count*/, RollVote /*voteType*/, Roll* /*roll*/) override;
    void OnAfterStoreOrEquipNewItem(Player* player, uint32 /*vendorslot*/, Item* item, uint8 /*count*/, uint8 /*bag*/, uint8 /*slot*/, ItemTemplate const* /*pProto*/, Creature* /*pVendor*/, VendorItem const* /*crItem*/, bool /*bStore*/) override;
};

void AddRandomEnchantsScripts()
{
    new RandomEnchantsPlayer();
}
