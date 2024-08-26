#include "random_enchants.h"

void rollPossibleEnchant(Player* player, Item* item)
{
    // Check global enable option
    if (!sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true))
    {
        return;
    }

    uint32 itemQuality = item->GetTemplate()->Quality;
    uint32 itemClass = item->GetTemplate()->Class;

    /* eliminates enchanting anything that isn't a recognized quality */
    /* eliminates enchanting anything but weapons/armor */
    if ((itemQuality > 5 || itemQuality < 1) || (itemClass != 2 && itemClass != 4))
    {
        return;
    }

    int slotRand[3] = { -1, -1, -1 };
    uint32 slotEnch[3] = { 0, 1, 5 };

    // Fetching the configuration values as float
    float enchantChance1 = sConfigMgr->GetOption<float>("RandomEnchants.EnchantChance1", 100.0f);
    float enchantChance2 = sConfigMgr->GetOption<float>("RandomEnchants.EnchantChance2", 00.0f);
    float enchantChance3 = sConfigMgr->GetOption<float>("RandomEnchants.EnchantChance3", 00.0f);

    if (rand_chance() < enchantChance1)
        slotRand[0] = getRandEnchantment(item);
    if (slotRand[0] != -1 && rand_chance() < enchantChance2)
        slotRand[1] = getRandEnchantment(item);
    if (slotRand[1] != -1 && rand_chance() < enchantChance3)
        slotRand[2] = getRandEnchantment(item);

    for (int i = 0; i < 3; i++)
    {
        if (slotRand[i] != -1)
        {
            if (sSpellItemEnchantmentStore.LookupEntry(slotRand[i]))
            {   //Make sure enchantment id exists
                player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[i]), false);
                item->SetEnchantment(EnchantmentSlot(slotEnch[i]), slotRand[i], 0, 0);
                player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[i]), true);
            }
        }
    }

    ChatHandler chathandle = ChatHandler(player->GetSession());

    uint8 loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
    const ItemTemplate* temp = item->GetTemplate();
    std::string name = temp->Name1;
    if (ItemLocale const* il = sObjectMgr->GetItemLocale(temp->ItemId))
        ObjectMgr::GetLocaleString(il->Name, loc_idx, name);


    if (slotRand[2] != -1)
        chathandle.PSendSysMessage("Newly Acquired |cffFF0000 %s |rhas received|cffFF0000 3 |rrandom enchantments!", name);
    else if (slotRand[1] != -1)
        chathandle.PSendSysMessage("Newly Acquired |cffFF0000 %s |rhas received|cffFF0000 2 |rrandom enchantments!", name);
    else if (slotRand[0] != -1)
        chathandle.PSendSysMessage("Newly Acquired |cffFF0000 %s |rhas received|cffFF0000 1 |rrandom enchantment!", name);
}

uint32 getRandEnchantment(Item* item)
{
    uint32 itemClass = item->GetTemplate()->Class;
    uint32 itemQuality = item->GetTemplate()->Quality;
    uint32 itemLevel = item->GetTemplate()->ItemLevel;
    std::string classQueryString = "";
    std::string wdbString = "";
    uint8 tier = 0;
    bool whiteORgreen = false;

    switch (itemClass)
    {
        case 2:
            classQueryString = "WEAPON";
            break;
        case 4:
            classQueryString = "ARMOR";
            break;
    }

    if (classQueryString == "")
        return -1;

    if (itemLevel <= 10)
        tier = 1;
    else if (itemLevel <= 14)
        tier = 2;
    else if (itemLevel <= 19)
        tier = 3;
    else if (itemLevel <= 24)
        tier = 4;
    else if (itemLevel <= 29)
        tier = 5;
    else if (itemLevel <= 34)
        tier = 6;
    else if (itemLevel <= 39)
        tier = 7;
    else if (itemLevel <= 44)
        tier = 8;
    else if (itemLevel <= 49)
        tier = 9;
    else if (itemLevel <= 54)
        tier = 10;
    else if (itemLevel <= 59)
        tier = 11;
    else if (itemLevel <= 64)
        tier = 12;
    else if (itemLevel <= 102)
        tier = 13;
    else if (itemLevel <= 138)
        tier = 14;
    else if (itemLevel <= 158)
        tier = 15;
    else if (itemLevel <= 178)
        tier = 16;
    else
        tier = 17;

    // Boost tier rank based on quality of item being enchanted
    switch (itemQuality)
    {
        case GREY:
            break;
        case WHITE:
            break;
        case GREEN:
            break;
        case BLUE:
            tier += 1;
            break;
        case PURPLE:
            tier += 2;
            break;
        case ORANGE:
            tier += 3;
            break;
    }

    wdbString = "SELECT `enchantID` FROM `item_enchantment_random_tiers` WHERE `tier`='" + std::to_string(tier);

    // Fetching the configuration values as float
    float professionEnchantChance = sConfigMgr->GetOption<float>("RandomEnchants.ProfessionEnchantChance", 4.0f);
    float mountEnchantChance = sConfigMgr->GetOption<float>("RandomEnchants.MountEnchantChance", 2.0f);

    // Check if Item is White or Green
    if (itemQuality == WHITE || itemQuality == GREEN)
        whiteORgreen = true;
    
    if (sConfigMgr->GetOption<bool>("RandomEnchants.IncludeProfessionEnchants", true) && whiteORgreen == true && rand_chance() < professionEnchantChance)
        wdbString += "' AND `category`='PROFESSION' ORDER BY RAND() LIMIT 1";
    else if (sConfigMgr->GetOption<bool>("RandomEnchants.IncludeMountEnchants", true) && whiteORgreen == true && rand_chance() < mountEnchantChance)
        wdbString += "' AND `category`='MOUNT' ORDER BY RAND() LIMIT 1";
    else if (classQueryString == "WEAPON")
        if (rand_chance() < 5.0f)
            wdbString += "' AND `class`='" + classQueryString + "' AND `category` IS NULL ORDER BY RAND() LIMIT 1";
        else
            wdbString += "' AND `class`='ANY' AND `category` IS NULL ORDER BY RAND() LIMIT 1";
    else if (classQueryString == "ARMOR")
        if (rand_chance() < 5.0f)
            wdbString += "' AND `class`='" + classQueryString + "' AND `category` IS NULL ORDER BY RAND() LIMIT 1";
        else
            wdbString += "' AND `class`='ANY' AND `category` IS NULL ORDER BY RAND() LIMIT 1";
    else
        wdbString += "' AND `class`='ANY' AND `category` IS NULL ORDER BY RAND() LIMIT 1";

    // Check if a random enchant matching criteria was found
    QueryResult result = WorldDatabase.Query(wdbString);

    // If result failed to find enchant, try a random class 'ANY' enchant
    if (!result)
        wdbString = "SELECT `enchantID` FROM `item_enchantment_random_tiers` WHERE `tier`='" + std::to_string(tier) + "' AND `class`='ANY' AND `category` IS NULL ORDER BY RAND() LIMIT 1";
    else
        return result->Fetch()[0].Get<uint32>();

    // Check if the backup class 'ANY' enchant was found 
    result = WorldDatabase.Query(wdbString);        

    // If result failed again, abort 
    if (!result)
        return 0;

    return result->Fetch()[0].Get<uint32>();

}

void RandomEnchantsPlayer::OnLogin(Player* player)
{
    if (sConfigMgr->GetOption<bool>("RandomEnchants.AnnounceOnLogin", true) && (sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true)))
        ChatHandler(player->GetSession()).SendSysMessage(sConfigMgr->GetOption<std::string>("RandomEnchants.OnLoginMessage", "This server is running a RandomEnchants Module.").c_str());
}

void RandomEnchantsPlayer::OnLootItem(Player* player, Item* item, uint32 /*count*/, ObjectGuid /*lootguid*/)
{
    if (sConfigMgr->GetOption<bool>("RandomEnchants.OnLoot", true) && sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true))
        rollPossibleEnchant(player, item);
}

void RandomEnchantsPlayer::OnCreateItem(Player* player, Item* item, uint32 /*count*/)
{
    if (sConfigMgr->GetOption<bool>("RandomEnchants.OnCreate", true) && (sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true)))
        rollPossibleEnchant(player, item);
}

void RandomEnchantsPlayer::OnQuestRewardItem(Player* player, Item* item, uint32 /*count*/)
{
    if (sConfigMgr->GetOption<bool>("RandomEnchants.OnQuestReward", true) && (sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true)))
        rollPossibleEnchant(player, item);
}

void RandomEnchantsPlayer::OnGroupRollRewardItem(Player* player, Item* item, uint32 /*count*/, RollVote /*voteType*/, Roll* /*roll*/)
{
    if (sConfigMgr->GetOption<bool>("RandomEnchants.OnGroupRoll", true) && (sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true)))
        rollPossibleEnchant(player, item);
}

void RandomEnchantsPlayer::OnAfterStoreOrEquipNewItem(Player* player, uint32 /*vendorslot*/, Item* item, uint8 /*count*/, uint8 /*bag*/, uint8 /*slot*/, ItemTemplate const* /*pProto*/, Creature* /*pVendor*/, VendorItem const* /*crItem*/, bool /*bStore*/)
{
    if (sConfigMgr->GetOption<bool>("RandomEnchants.OnAfterStoreOrEquipNewItem", true) && (sConfigMgr->GetOption<bool>("RandomEnchants.Enable", true)))
        rollPossibleEnchant(player, item);
}