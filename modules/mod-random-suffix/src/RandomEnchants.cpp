/*
* Converted from the original LUA script to a module for Azerothcore(Sunwell) :D
*/
#include "ScriptMgr.h"
#include "Player.h"
#include "Configuration/Config.h"
#include "Chat.h"
#include "Item.h"

// DEFAULT VALUES

// Our suffix tiers can go up to 4.
#define MAX_RAND_ENCHANT_TIERS 4

double default_enchant_pcts[MAX_RAND_ENCHANT_TIERS] = {30.0, 35.0, 40.0, 45.0};

bool default_announce_on_log = true;
bool default_debug = false;
bool default_on_loot = true;
bool default_on_create = true;
bool default_on_quest_reward = true;
bool default_on_group_roll_reward_item = true;
bool default_on_vendor_purchase = true;
bool default_on_all_items_created = true;
bool default_use_new_random_enchant_system = true;
bool default_roll_player_class_preference = false;
std::string default_login_message ="This server is running a RandomEnchants Module.";

// CONFIGURATION

double config_enchant_pcts[MAX_RAND_ENCHANT_TIERS] = {
    default_enchant_pcts[0],
    default_enchant_pcts[1],
    default_enchant_pcts[2],
    default_enchant_pcts[3],
};
bool config_announce_on_log = default_announce_on_log;
bool config_debug = default_debug;
bool config_on_loot = default_on_loot;
bool config_on_create = default_on_create;
bool config_on_quest_reward = default_on_quest_reward;
bool config_on_group_roll_reward_item = default_on_group_roll_reward_item;
bool config_on_vendor_purchase = default_on_vendor_purchase;
// bool config_on_all_items_created = default_on_all_items_created;
bool config_use_new_random_enchant_system = default_use_new_random_enchant_system;
bool config_roll_player_class_preference = default_roll_player_class_preference;
std::string config_login_message = default_login_message;

enum Attributes
{
    ATTRIBUTE_STRENGTH      = 0,  
    ATTRIBUTE_AGILITY       = 1,  
    ATTRIBUTE_INTELLECT     = 2,  
    ATTRIBUTE_SPIRIT        = 3,  
    ATTRIBUTE_STAMINA       = 4,  
    ATTRIBUTE_ATTACKPOWER   = 5,  
    ATTRIBUTE_SPELLPOWER    = 6,  
    ATTRIBUTE_HASTE         = 7,  
    ATTRIBUTE_HIT           = 8,  
    ATTRIBUTE_CRIT          = 9,  
    ATTRIBUTE_EXPERTISE     = 10,  
    ATTRIBUTE_DEFENSERATING = 11,  
    ATTRIBUTE_DODGE         = 12,  
    ATTRIBUTE_PARRY         = 13,  
};

// UTILS
enum EnchantCategory
{
    ENCH_CAT_MELEE_STR_DPS  = 0,
    ENCH_CAT_MELEE_STR_TANK = 1,
    ENCH_CAT_MELEE_AGI_DPS  = 2,
    ENCH_CAT_MELEE_AGI_TANK = 3,
    ENCH_CAT_RANGED_AGI     = 4,
    ENCH_CAT_CASTER         = 5,
};

uint32 getEnchantCategoryMask(std::vector<EnchantCategory> enchCategories)
{
    uint32 r = 0;
    for (auto enchCat : enchCategories)
    {
        r |= 1 << enchCat;
    }
    return r;
}

uint32 getAttributeMask(std::vector<Attributes> attributes)
{
    uint32 r = 0;
    for (auto enchCat : attributes)
    {
        r |= 1 << enchCat;
    }
    return r;
}

// getItemPlayerLevel retrieves an item's player required level
// It uses the item's required level if its not zero, otherwise it will rely on
// the average required level calculated from the item template table.
uint32 getItemPlayerLevel(Item* item)
{
    if (uint32 reqLevel = item->GetTemplate()->RequiredLevel)
    {
        return reqLevel;
    }
    QueryResult qr = WorldDatabase.Query(R"(select ceil(avg(RequiredLevel)) from item_template where
ItemLevel = {}
and class in ({},{})
and RequiredLevel != {}
and not (name like '%qa%' or name like '%test%' or name like '%debug%' or name like '%internal%' or name like '%demo%')
group by ItemLevel LIMIT 1)",
        item->GetTemplate()->ItemLevel,
        ITEM_CLASS_WEAPON, ITEM_CLASS_ARMOR,
        0);
    if (!qr)
    {
        // If unable to query, fallback to maxlevel (NOTE: maybe would be better to default to 1 instead of max)
        return sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    }
    uint32 avgReqLevel = qr->Fetch()[0].Get<uint32>();
    return avgReqLevel;
}

int getLevelOffset(Item* item, Player* player = nullptr)
{
    int level = 1;
    if (player)
    {
        level = player->getLevel();
    }
    else
    {
        level = getItemPlayerLevel(item);
    }
    // level offset calculation below
    // current_level - 5 + item_quality
    level = level - ITEM_QUALITY_LEGENDARY + item->GetTemplate()->Quality;
    if (level <= 0)
    {
        level = 1;
    }
    return level;
}

auto getEnchantCategoryMaskByClassAndSpec(uint8 plrClass, uint32 plrSpec)
{
    std::vector<EnchantCategory> plrEnchCats;
    std::vector<Attributes> plrAttrs;
    switch (plrClass)
    {
        case CLASS_WARRIOR:
            switch (plrSpec)
            {
            case TALENT_TREE_WARRIOR_ARMS:
            case TALENT_TREE_WARRIOR_FURY:
                plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STRENGTH,ATTRIBUTE_STAMINA,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE});
                plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_STR_DPS});
                break;
            case TALENT_TREE_WARRIOR_PROTECTION:
                plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STRENGTH,ATTRIBUTE_STAMINA,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE,ATTRIBUTE_DEFENSERATING,ATTRIBUTE_DODGE,ATTRIBUTE_PARRY});
                plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_STR_TANK});
                break;
            }
            break;
        case CLASS_PALADIN:
            switch (plrSpec)
            {
                case TALENT_TREE_PALADIN_HOLY:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
                    break;
                case TALENT_TREE_PALADIN_PROTECTION:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STRENGTH,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_EXPERTISE,ATTRIBUTE_DEFENSERATING,ATTRIBUTE_DODGE,ATTRIBUTE_PARRY});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_STR_TANK});
                    break;
                case TALENT_TREE_PALADIN_RETRIBUTION:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STRENGTH,ATTRIBUTE_STAMINA,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_STR_DPS});
                    break;
            }
            break;
        case CLASS_HUNTER:
            plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_RANGED_AGI});
            plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_AGILITY,ATTRIBUTE_STAMINA,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT});
            switch (plrSpec)
            {
                case TALENT_TREE_HUNTER_BEAST_MASTERY:
                case TALENT_TREE_HUNTER_MARKSMANSHIP:
                case TALENT_TREE_HUNTER_SURVIVAL:
                    break;
            }
            break;
        case CLASS_ROGUE:
            plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_AGILITY,ATTRIBUTE_STAMINA,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE});
            switch (plrSpec)
            {
                case TALENT_TREE_ROGUE_ASSASSINATION:
                case TALENT_TREE_ROGUE_COMBAT:
                case TALENT_TREE_ROGUE_SUBTLETY:
                    break;
            }
            break;
        case CLASS_PRIEST:
            plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
            plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_SPIRIT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_CRIT});
            switch (plrSpec)
            {
                case TALENT_TREE_PRIEST_DISCIPLINE:
                case TALENT_TREE_PRIEST_HOLY:
                    break;
                case TALENT_TREE_PRIEST_SHADOW:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_HIT});
                    break;
            }
            break;
        case CLASS_DEATH_KNIGHT:
            switch (plrSpec)
            {
                case TALENT_TREE_DEATH_KNIGHT_BLOOD:
                case TALENT_TREE_DEATH_KNIGHT_FROST:
                case TALENT_TREE_DEATH_KNIGHT_UNHOLY:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STRENGTH,ATTRIBUTE_STAMINA,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE,ATTRIBUTE_DEFENSERATING,ATTRIBUTE_DODGE,ATTRIBUTE_PARRY});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_STR_DPS, ENCH_CAT_MELEE_STR_TANK});
                    break;
            }
            break;
        case CLASS_SHAMAN:
            switch (plrSpec)
            {
                case TALENT_TREE_SHAMAN_ELEMENTAL:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
                    break;
                case TALENT_TREE_SHAMAN_ENHANCEMENT:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STAMINA,ATTRIBUTE_STRENGTH,ATTRIBUTE_AGILITY,ATTRIBUTE_INTELLECT,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_AGI_DPS});
                    break;
                case TALENT_TREE_SHAMAN_RESTORATION:
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_CRIT});
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
                    break;
            }
            break;
        case CLASS_MAGE:
            plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
            plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_SPIRIT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT});
            switch (plrSpec)
            {
                case TALENT_TREE_MAGE_ARCANE:
                case TALENT_TREE_MAGE_FIRE:
                case TALENT_TREE_MAGE_FROST:
                    break;
            }
            break;
        case CLASS_WARLOCK:
            plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
            plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_SPIRIT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT});
            switch (plrSpec)
            {
                case TALENT_TREE_WARLOCK_AFFLICTION:
                case TALENT_TREE_WARLOCK_DEMONOLOGY:
                case TALENT_TREE_WARLOCK_DESTRUCTION:
                    break;
            }
            break;
        case CLASS_DRUID:
            switch (plrSpec)
            {
                case TALENT_TREE_DRUID_BALANCE:
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_SPIRIT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT});
                    break;
                case TALENT_TREE_DRUID_FERAL_COMBAT:
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_MELEE_AGI_DPS, ENCH_CAT_MELEE_AGI_TANK});
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_STRENGTH,ATTRIBUTE_AGILITY,ATTRIBUTE_STAMINA,ATTRIBUTE_ATTACKPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_HIT,ATTRIBUTE_CRIT,ATTRIBUTE_EXPERTISE,ATTRIBUTE_DEFENSERATING,ATTRIBUTE_DODGE});
                    break;
                case TALENT_TREE_DRUID_RESTORATION:
                    plrEnchCats.insert(plrEnchCats.end(), {ENCH_CAT_CASTER});
                    plrAttrs.insert(plrAttrs.end(), {ATTRIBUTE_INTELLECT,ATTRIBUTE_SPIRIT,ATTRIBUTE_STAMINA,ATTRIBUTE_SPELLPOWER,ATTRIBUTE_HASTE,ATTRIBUTE_CRIT});
                    break;
            }
            break;
    }
    struct retVals {
        uint32 enchCatMask, attrMask;
    };
    return retVals{getEnchantCategoryMask(plrEnchCats), getAttributeMask(plrAttrs)};
}

auto getPlayerEnchantCategoryMask(Player* player)
{
    return getEnchantCategoryMaskByClassAndSpec(player->getClass(), player->GetSpec(player->GetActiveSpec()));
}

std::unordered_map<uint32, uint8> specToClass = {
    {TALENT_TREE_WARRIOR_ARMS,          CLASS_WARRIOR},
    {TALENT_TREE_WARRIOR_FURY,          CLASS_WARRIOR},
    {TALENT_TREE_WARRIOR_PROTECTION,    CLASS_WARRIOR},
    {TALENT_TREE_PALADIN_HOLY,          CLASS_PALADIN},
    {TALENT_TREE_PALADIN_PROTECTION,    CLASS_PALADIN},
    {TALENT_TREE_PALADIN_RETRIBUTION,   CLASS_PALADIN},
    {TALENT_TREE_HUNTER_BEAST_MASTERY,  CLASS_HUNTER},
    {TALENT_TREE_HUNTER_MARKSMANSHIP,   CLASS_HUNTER},
    {TALENT_TREE_HUNTER_SURVIVAL,       CLASS_HUNTER},
    {TALENT_TREE_ROGUE_ASSASSINATION,   CLASS_ROGUE},
    {TALENT_TREE_ROGUE_COMBAT,          CLASS_ROGUE},
    {TALENT_TREE_ROGUE_SUBTLETY,        CLASS_ROGUE},
    {TALENT_TREE_PRIEST_DISCIPLINE,     CLASS_PRIEST},
    {TALENT_TREE_PRIEST_HOLY,           CLASS_PRIEST},
    {TALENT_TREE_PRIEST_SHADOW,         CLASS_PRIEST},
    {TALENT_TREE_DEATH_KNIGHT_BLOOD,    CLASS_DEATH_KNIGHT},
    {TALENT_TREE_DEATH_KNIGHT_FROST,    CLASS_DEATH_KNIGHT},
    {TALENT_TREE_DEATH_KNIGHT_UNHOLY,   CLASS_DEATH_KNIGHT},
    {TALENT_TREE_SHAMAN_ELEMENTAL,      CLASS_SHAMAN},
    {TALENT_TREE_SHAMAN_ENHANCEMENT,    CLASS_SHAMAN},
    {TALENT_TREE_SHAMAN_RESTORATION,    CLASS_SHAMAN},
    {TALENT_TREE_MAGE_ARCANE,           CLASS_MAGE},
    {TALENT_TREE_MAGE_FIRE,             CLASS_MAGE},
    {TALENT_TREE_MAGE_FROST,            CLASS_MAGE},
    {TALENT_TREE_WARLOCK_AFFLICTION,    CLASS_WARLOCK},
    {TALENT_TREE_WARLOCK_DEMONOLOGY,    CLASS_WARLOCK},
    {TALENT_TREE_WARLOCK_DESTRUCTION,   CLASS_WARLOCK},
    {TALENT_TREE_DRUID_BALANCE,         CLASS_DRUID},
    {TALENT_TREE_DRUID_FERAL_COMBAT,    CLASS_DRUID},
    {TALENT_TREE_DRUID_RESTORATION,     CLASS_DRUID},
};

std::unordered_map<uint32, std::string> specToSpecNames = {
    {TALENT_TREE_WARRIOR_ARMS,          "WARRIOR_ARMS"},
    {TALENT_TREE_WARRIOR_FURY,          "WARRIOR_FURY"},
    {TALENT_TREE_WARRIOR_PROTECTION,    "WARRIOR_PROTECTION"},
    {TALENT_TREE_PALADIN_HOLY,          "PALADIN_HOLY"},
    {TALENT_TREE_PALADIN_PROTECTION,    "PALADIN_PROTECTION"},
    {TALENT_TREE_PALADIN_RETRIBUTION,   "PALADIN_RETRIBUTION"},
    {TALENT_TREE_HUNTER_BEAST_MASTERY,  "HUNTER_BEAST_MASTERY"},
    {TALENT_TREE_HUNTER_MARKSMANSHIP,   "HUNTER_MARKSMANSHIP"},
    {TALENT_TREE_HUNTER_SURVIVAL,       "HUNTER_SURVIVAL"},
    {TALENT_TREE_ROGUE_ASSASSINATION,   "ROGUE_ASSASSINATION"},
    {TALENT_TREE_ROGUE_COMBAT,          "ROGUE_COMBAT"},
    {TALENT_TREE_ROGUE_SUBTLETY,        "ROGUE_SUBTLETY"},
    {TALENT_TREE_PRIEST_DISCIPLINE,     "PRIEST_DISCIPLINE"},
    {TALENT_TREE_PRIEST_HOLY,           "PRIEST_HOLY"},
    {TALENT_TREE_PRIEST_SHADOW,         "PRIEST_SHADOW"},
    {TALENT_TREE_DEATH_KNIGHT_BLOOD,    "DEATH_KNIGHT_BLOOD"},
    {TALENT_TREE_DEATH_KNIGHT_FROST,    "DEATH_KNIGHT_FROST"},
    {TALENT_TREE_DEATH_KNIGHT_UNHOLY,   "DEATH_KNIGHT_UNHOLY"},
    {TALENT_TREE_SHAMAN_ELEMENTAL,      "SHAMAN_ELEMENTAL"},
    {TALENT_TREE_SHAMAN_ENHANCEMENT,    "SHAMAN_ENHANCEMENT"},
    {TALENT_TREE_SHAMAN_RESTORATION,    "SHAMAN_RESTORATION"},
    {TALENT_TREE_MAGE_ARCANE,           "MAGE_ARCANE"},
    {TALENT_TREE_MAGE_FIRE,             "MAGE_FIRE"},
    {TALENT_TREE_MAGE_FROST,            "MAGE_FROST"},
    {TALENT_TREE_WARLOCK_AFFLICTION,    "WARLOCK_AFFLICTION"},
    {TALENT_TREE_WARLOCK_DEMONOLOGY,    "WARLOCK_DEMONOLOGY"},
    {TALENT_TREE_WARLOCK_DESTRUCTION,   "WARLOCK_DESTRUCTION"},
    {TALENT_TREE_DRUID_BALANCE,         "DRUID_BALANCE"},
    {TALENT_TREE_DRUID_FERAL_COMBAT,    "DRUID_FERAL_COMBAT"},
    {TALENT_TREE_DRUID_RESTORATION,     "DRUID_RESTORATION"},
};

typedef struct itemPotentialRoleCheck {
    bool isRanged;
    bool isMelee;
    bool isPhysDPS;
    bool isStr;
    bool isAgi;
    bool isTank;
    bool isCaster;

    itemPotentialRoleCheck(): isRanged(false), isMelee(false), isPhysDPS(false), isStr(false), isAgi(false), isTank(false), isCaster(false) {}

} itemPotentialRoleCheck;

auto getItemPotentialRoles(Item* item)
{
    itemPotentialRoleCheck r;
    // role checks
    for (uint8 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
    {
        if (i >= item->GetTemplate()->StatsCount)
        {
            break;
        }
        switch (item->GetTemplate()->ItemStat[i].ItemStatType)
        {
            case ITEM_MOD_AGILITY:
                r.isAgi = true;
                continue;
            case ITEM_MOD_STRENGTH:
                r.isStr = true;
                continue;
            case ITEM_MOD_INTELLECT:
            case ITEM_MOD_SPIRIT:
                r.isCaster = true;
                continue;
            case ITEM_MOD_HIT_MELEE_RATING:
            case ITEM_MOD_CRIT_MELEE_RATING:
            case ITEM_MOD_HASTE_MELEE_RATING:
            case ITEM_MOD_EXPERTISE_RATING:
                r.isMelee = true;
                continue;
            case ITEM_MOD_HIT_RANGED_RATING:
            case ITEM_MOD_CRIT_RANGED_RATING:
            case ITEM_MOD_HASTE_RANGED_RATING:
            case ITEM_MOD_RANGED_ATTACK_POWER:
                r.isRanged = true;
                continue;
            case ITEM_MOD_HIT_SPELL_RATING:
            case ITEM_MOD_SPELL_DAMAGE_DONE:
            case ITEM_MOD_SPELL_PENETRATION:
                r.isCaster = true;
                continue;
            case ITEM_MOD_CRIT_SPELL_RATING:
            case ITEM_MOD_HASTE_SPELL_RATING:
            case ITEM_MOD_SPELL_POWER:
            // TODO: Check proto->HasSpellPowerStat() for spellpower
            // TODO: Check SPELL_AURA_MOD_POWER_COST_SCHOOL for specific spell schools (e.g. shadow, holy etc)
                r.isCaster = true;
                continue;
            case ITEM_MOD_SPELL_HEALING_DONE:
            case ITEM_MOD_MANA_REGENERATION:
                r.isCaster = true;
                continue;
            case ITEM_MOD_ATTACK_POWER:
            case ITEM_MOD_ARMOR_PENETRATION_RATING:
                r.isPhysDPS = true;
                continue;
            case ITEM_MOD_DEFENSE_SKILL_RATING:
            case ITEM_MOD_DODGE_RATING:
            case ITEM_MOD_PARRY_RATING:
                r.isTank = true;
                continue;
            case ITEM_MOD_BLOCK_RATING:
            case ITEM_MOD_BLOCK_VALUE:
                r.isTank = true;
                continue;
            case ITEM_MOD_STAMINA:
            case ITEM_MOD_HEALTH:
            case ITEM_MOD_MANA:
            case ITEM_MOD_HEALTH_REGEN:
            case ITEM_MOD_HIT_RATING:
            case ITEM_MOD_CRIT_RATING:
            case ITEM_MOD_HASTE_RATING:
            case ITEM_MOD_HIT_TAKEN_RATING:         // Miss Related
            case ITEM_MOD_HIT_TAKEN_MELEE_RATING:   // Miss Related
            case ITEM_MOD_HIT_TAKEN_RANGED_RATING:  // Miss Related
            case ITEM_MOD_HIT_TAKEN_SPELL_RATING:   // Miss Related
            case ITEM_MOD_CRIT_TAKEN_RATING:        // Resilience related
            case ITEM_MOD_RESILIENCE_RATING:        // Resilience related
            case ITEM_MOD_CRIT_TAKEN_MELEE_RATING:  // Resilience related
            case ITEM_MOD_CRIT_TAKEN_RANGED_RATING: // Resilience related
            case ITEM_MOD_CRIT_TAKEN_SPELL_RATING:  // Resilience related
                continue;
        }
    }
    // TODO: Get DefenseRating equip check
    return r;
}

auto itemRoleRoleCheckToClassSpecs_Warrior(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (itemClass == ITEM_CLASS_ARMOR && itemSubClass == ITEM_SUBCLASS_ARMOR_SHIELD) {
        if (rc.isMelee || rc.isPhysDPS || rc.isStr || rc.isTank || forceAddAll) {
            specPool.insert({TALENT_TREE_WARRIOR_PROTECTION});
        }
        return specPool;
    }
    if (rc.isTank) {
        specPool.insert({TALENT_TREE_WARRIOR_PROTECTION});
    } else if (!rc.isAgi && (rc.isMelee || rc.isPhysDPS)) {
        specPool.insert({TALENT_TREE_WARRIOR_ARMS, TALENT_TREE_WARRIOR_FURY});
    } else if (rc.isStr || forceAddAll) {
        specPool.insert({TALENT_TREE_WARRIOR_PROTECTION, TALENT_TREE_WARRIOR_ARMS, TALENT_TREE_WARRIOR_FURY});
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Paladin(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (itemClass == ITEM_CLASS_ARMOR && itemSubClass == ITEM_SUBCLASS_ARMOR_SHIELD) {
        if (rc.isMelee || rc.isPhysDPS || rc.isStr || rc.isTank) {
            specPool.insert({TALENT_TREE_PALADIN_PROTECTION});
        } else if (rc.isCaster) {
            specPool.insert({TALENT_TREE_PALADIN_HOLY});
        } else if (forceAddAll) {
            specPool.insert({TALENT_TREE_PALADIN_HOLY, TALENT_TREE_PALADIN_PROTECTION});
        }
        return specPool;
    }
    if (itemInvType == INVTYPE_HOLDABLE) {
        if (rc.isCaster || forceAddAll) {
            specPool.insert({TALENT_TREE_PALADIN_HOLY});
        }
        return specPool;
    }
    if (rc.isTank) {
        specPool.insert({TALENT_TREE_PALADIN_PROTECTION});
    } else if (rc.isCaster) {
        specPool.insert({TALENT_TREE_PALADIN_HOLY});
    } else if (!rc.isAgi && (rc.isMelee || rc.isPhysDPS)) {
        specPool.insert({TALENT_TREE_PALADIN_RETRIBUTION});
    } else if (rc.isStr || forceAddAll) {
        specPool.insert({TALENT_TREE_PALADIN_HOLY, TALENT_TREE_PALADIN_PROTECTION, TALENT_TREE_PALADIN_RETRIBUTION});
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Hunter(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (rc.isRanged || rc.isAgi || (!rc.isStr && rc.isPhysDPS) || forceAddAll) {
        specPool.insert({
            TALENT_TREE_HUNTER_BEAST_MASTERY,
            TALENT_TREE_HUNTER_MARKSMANSHIP,
            TALENT_TREE_HUNTER_SURVIVAL,
        });
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Rogue(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (rc.isAgi || (!rc.isStr && (rc.isMelee || rc.isPhysDPS)) || forceAddAll) {
        specPool.insert({
            TALENT_TREE_ROGUE_ASSASSINATION,
            TALENT_TREE_ROGUE_COMBAT,
            TALENT_TREE_ROGUE_SUBTLETY,
        });
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Priest(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (rc.isCaster || forceAddAll) {
        specPool.insert({
            TALENT_TREE_PRIEST_DISCIPLINE,
            TALENT_TREE_PRIEST_HOLY,
            TALENT_TREE_PRIEST_SHADOW,
        });
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_DeathKnight(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (rc.isTank) {
        specPool.insert({TALENT_TREE_DEATH_KNIGHT_BLOOD});
    } else if (rc.isMelee || rc.isPhysDPS || rc.isStr || rc.isAgi || forceAddAll) {
        specPool.insert({TALENT_TREE_DEATH_KNIGHT_BLOOD,TALENT_TREE_DEATH_KNIGHT_FROST,TALENT_TREE_DEATH_KNIGHT_UNHOLY});
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Shaman(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (itemClass == ITEM_CLASS_ARMOR && itemSubClass == ITEM_SUBCLASS_ARMOR_SHIELD) {
        if (rc.isCaster || forceAddAll) {
            specPool.insert({TALENT_TREE_SHAMAN_ELEMENTAL,TALENT_TREE_SHAMAN_RESTORATION});
        }
        return specPool;
    }
    if (itemInvType == INVTYPE_HOLDABLE) {
        if (rc.isCaster || forceAddAll) {
            specPool.insert({TALENT_TREE_SHAMAN_ELEMENTAL,TALENT_TREE_SHAMAN_RESTORATION});
        }
        return specPool;
    }
    if (rc.isAgi || (!rc.isStr && (rc.isMelee || rc.isPhysDPS))) {
        specPool.insert({TALENT_TREE_SHAMAN_ENHANCEMENT});
    } else if (rc.isCaster) {
        specPool.insert({TALENT_TREE_SHAMAN_ELEMENTAL,TALENT_TREE_SHAMAN_RESTORATION});
    } else if (forceAddAll) {
        specPool.insert({TALENT_TREE_SHAMAN_ENHANCEMENT,TALENT_TREE_SHAMAN_ELEMENTAL,TALENT_TREE_SHAMAN_RESTORATION});
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Mage(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (rc.isCaster || forceAddAll) {
        specPool.insert({
            TALENT_TREE_MAGE_ARCANE,
            TALENT_TREE_MAGE_FIRE,
            TALENT_TREE_MAGE_FROST,
        });
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Warlock(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (rc.isCaster || forceAddAll) {
        specPool.insert({
            TALENT_TREE_WARLOCK_AFFLICTION,
            TALENT_TREE_WARLOCK_DEMONOLOGY,
            TALENT_TREE_WARLOCK_DESTRUCTION,
        });
    }
    return specPool;
}

auto itemRoleRoleCheckToClassSpecs_Druid(itemPotentialRoleCheck rc, bool forceAddAll=false, uint32 itemClass=0, uint32 itemSubClass=0, uint32 itemInvType=0) {
    std::set<uint32> specPool;
    if (itemInvType == INVTYPE_HOLDABLE) {
        if (rc.isCaster || forceAddAll) {
            specPool.insert({TALENT_TREE_DRUID_BALANCE,TALENT_TREE_DRUID_RESTORATION});
        }
        return specPool;
    }
    if (rc.isTank || rc.isAgi || (!rc.isStr && (rc.isMelee || rc.isPhysDPS))) {
        specPool.insert({TALENT_TREE_DRUID_FERAL_COMBAT});
    } else if (rc.isCaster) {
        specPool.insert({TALENT_TREE_DRUID_BALANCE,TALENT_TREE_DRUID_RESTORATION});
    } else if (forceAddAll) {
        specPool.insert({TALENT_TREE_DRUID_BALANCE,TALENT_TREE_DRUID_FERAL_COMBAT,TALENT_TREE_DRUID_RESTORATION});
    }
    return specPool;
}


// gets the item enchant category mask for a given item
auto getItemEnchantCategoryMask(Item* item)
{
    auto r = getItemPotentialRoles(item);
    auto itemPlayerLevel = getItemPlayerLevel(item);
    std::set<uint32> specPool; 
    auto ic = item->GetTemplate()->Class;
    auto isc = item->GetTemplate()->SubClass;
    auto ivt = item->GetTemplate()->InventoryType;
    // ITEM CLASS + SUB CLASS - ARMOUR
    bool isCloth = false;
    bool isLeather = false;
    bool isMail = false;
    bool isPlate = false;
    bool isIdol = false;
    bool isLibram = false;
    bool isTotem = false;
    bool isSigil = false;
    bool isShield = false;
    // ITEM CLASS + SUB CLASS - WEAPONS
    bool isBow = false;
    bool isCrossbow = false;
    bool isGun = false;
    bool isThrown = false;
    bool isWand = false;
    bool isDagger = false;
    bool isFist = false;
    bool isAxe = false;
    bool isMace = false;
    bool isSword = false;
    bool isPolearm = false;
    bool isStaff = false;
    bool isAxe2 = false;
    bool isMace2 = false;
    bool isSword2 = false;
    // INV TYPE FOR MISC SLOTS
    bool isNeck = false; 
    bool isCloak = false; 
    bool isHoldable = false; 
    bool isFinger = false; 
    bool isTrinket = false; 
    switch (ic)
    {
        case ITEM_CLASS_ARMOR:
            switch (isc)
            {
                case ITEM_SUBCLASS_ARMOR_CLOTH:
                    isCloth = ivt != INVTYPE_CLOAK;
                    if (isCloth) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_ARMOR_LEATHER:
                    isLeather = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (itemPlayerLevel > 40) {
                        if (specPool.empty()) {
                            // Nothing set, we include all potential leather wearing specs.
                            specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                            specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                        }
                        // If leather item above level 40, we can safely break away
                        break;
                    }
                    // NOTE: FALLTHROUGH TO MAIL, there is a potential that isSet is not set at all
                    //       but we try to bank on the chance that the conditionals in mail are set.
                case ITEM_SUBCLASS_ARMOR_MAIL:
                    // isMail gear check, but this case check can be fallenthrough from the LEATHER
                    // in that case if isLeather is true, isMail will still be set false.
                    isMail = !isLeather;
                    // isEventualMailUserItem check. This checks for actual mail item is above lvl 40 or is leather below level 40.
                    if (bool isEventualMailUserItem = !isMail || itemPlayerLevel > 40)
                    {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                        if (specPool.empty())
                        {
                            if (!isMail)
                            {
                                // received fallthru from leather item + nothing set, we include all
                                // potential leather wearing specs.
                                specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                                specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                            }
                            specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                            specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        }
                        break;
                    }
                    // NOTE: fallthrough to PLATE item. there is a potential that isSet is not set at all
                    //       but we try to bank on the chance that the conditionals in the next case section are set.
                case ITEM_SUBCLASS_ARMOR_PLATE:
                    // isMail gear check, but this case check can be fallenthrough from the LEATHER
                    // in that case if isLeather is true, isMail will still be set false.
                    isPlate = !isMail;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_ARMOR_SHIELD:
                    isShield = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_ARMOR_IDOL:
                    isIdol = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_ARMOR_LIBRAM:
                    isLibram = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_ARMOR_TOTEM:
                    isTotem = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_ARMOR_SIGIL:
                    isSigil = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                    }
                    break;
            }
            break;
        case ITEM_CLASS_WEAPON:
            switch (isc)
            {
                case ITEM_SUBCLASS_WEAPON_BOW:
                    isBow = true;
                    // NOTE: fallthrough to Crossbow item. there is a potential that isSet is not set at all
                    //       all ranged item types are evaluated together
                case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    isCrossbow = !isBow;
                    // NOTE: fallthrough to Gun item. there is a potential that isSet is not set at all
                    //       all ranged item types are evaluated together
                case ITEM_SUBCLASS_WEAPON_GUN:
                    isGun = !isBow && !isCrossbow;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    // NOTE: fallthrough to Thrown item. there is a potential that isSet is not set at all
                    //       all ranged item types are evaluated together
                case ITEM_SUBCLASS_WEAPON_THROWN:
                    isThrown = !isBow && !isCrossbow && !isGun;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_WAND:
                    isWand = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
                    break;
                case ITEM_SUBCLASS_WEAPON_DAGGER:
                    isDagger = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_FIST:
                    isFist = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_AXE:
                    isAxe = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_MACE:
                    isMace = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_SWORD:
                    isSword = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_POLEARM:
                    isPolearm = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_STAFF:
                    isStaff = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_AXE2:
                    isAxe2 = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_MACE2:
                    isMace2 = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_SWORD2:
                    isSword2 = true;
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
                    specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
                    if (specPool.empty()) {
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                        specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                    }
                    break;
                case ITEM_SUBCLASS_WEAPON_SPEAR:
                case ITEM_SUBCLASS_WEAPON_obsolete:
                case ITEM_SUBCLASS_WEAPON_EXOTIC:
                case ITEM_SUBCLASS_WEAPON_EXOTIC2:
                case ITEM_SUBCLASS_WEAPON_MISC:
                case ITEM_SUBCLASS_WEAPON_FISHING_POLE:
                    break;
            }
            break;
    }

    switch (ivt)
    {
        case INVTYPE_HOLDABLE:
            isHoldable = true;
            specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
            break;
        case INVTYPE_NECK:
            isNeck = true;
            // fallthru
        case INVTYPE_CLOAK:
            isCloak = !isNeck;
            // fallthru
        case INVTYPE_FINGER:
            isFinger = !isNeck && !isCloak;
            // fallthru
        case INVTYPE_TRINKET:
            isTrinket = !isFinger && !isNeck && !isCloak;
            specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, false, ic, isc, ivt));
            specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, false, ic, isc, ivt));
            if (specPool.empty()) {
                specPool.merge(itemRoleRoleCheckToClassSpecs_Warrior(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Paladin(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Hunter(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Rogue(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Priest(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_DeathKnight(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Shaman(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Mage(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Warlock(r, true, ic, isc, ivt));
                specPool.merge(itemRoleRoleCheckToClassSpecs_Druid(r, true, ic, isc, ivt));
            }
            break;
    }
    if (config_debug)
    {
        LOG_INFO("module", ">>>>> RANDOM_ENCHANT DEBUG PRINT START <<<<<");
        LOG_INFO("module", "RANDOM_ENCHANT: Getting item enchant mask, checks below:");
        LOG_INFO("module", "       For item {}, Item ID is: {}", item->GetTemplate()->Name1, item->GetTemplate()->ItemId);
        LOG_INFO("module", "       >>> Printing detected item roles/stats profile");
        LOG_INFO("module", "                isRanged = {}", r.isRanged);
        LOG_INFO("module", "                isMelee = {}", r.isMelee);
        LOG_INFO("module", "                isPhysDPS = {}", r.isPhysDPS);
        LOG_INFO("module", "                isStr = {}", r.isStr);
        LOG_INFO("module", "                isAgi = {}", r.isAgi);
        LOG_INFO("module", "                isTank = {}", r.isTank);
        LOG_INFO("module", "                isCaster = {}", r.isCaster);
        LOG_INFO("module", "       >>> Printing detected item slots");
        LOG_INFO("module", "                isCloth = {}", isCloth);
        LOG_INFO("module", "                isLeather = {}", isLeather);
        LOG_INFO("module", "                isMail = {}", isMail);
        LOG_INFO("module", "                isPlate = {}", isPlate);
        LOG_INFO("module", "                isIdol = {}", isIdol);
        LOG_INFO("module", "                isLibram = {}", isLibram);
        LOG_INFO("module", "                isTotem = {}", isTotem);
        LOG_INFO("module", "                isSigil = {}", isSigil);
        LOG_INFO("module", "                isShield = {}", isShield);
        LOG_INFO("module", "                isBow = {}", isBow);
        LOG_INFO("module", "                isCrossbow = {}", isCrossbow);
        LOG_INFO("module", "                isGun = {}", isGun);
        LOG_INFO("module", "                isThrown = {}", isThrown);
        LOG_INFO("module", "                isWand = {}", isWand);
        LOG_INFO("module", "                isDagger = {}", isDagger);
        LOG_INFO("module", "                isFist = {}", isFist);
        LOG_INFO("module", "                isAxe = {}", isAxe);
        LOG_INFO("module", "                isMace = {}", isMace);
        LOG_INFO("module", "                isSword = {}", isSword);
        LOG_INFO("module", "                isPolearm = {}", isPolearm);
        LOG_INFO("module", "                isStaff = {}", isStaff);
        LOG_INFO("module", "                isAxe2 = {}", isAxe2);
        LOG_INFO("module", "                isMace2 = {}", isMace2);
        LOG_INFO("module", "                isSword2 = {}", isSword2);
        LOG_INFO("module", "                isNeck = {}", isNeck);
        LOG_INFO("module", "                isCloak = {}", isCloak);
        LOG_INFO("module", "                isHoldable = {}", isHoldable);
        LOG_INFO("module", "                isFinger = {}", isFinger);
        LOG_INFO("module", "                isTrinket = {}", isTrinket);
        LOG_INFO("module", "       >>> Printing candidate specs");
        std::ostringstream stream;
        for (auto s : specPool) {
            auto specName = std::to_string(s);
            if (auto found = specToSpecNames.find(s); found != specToSpecNames.end()) {
                specName = found->second;
            }
            stream << specName << ",";
        }
        std::string result = stream.str();
        LOG_INFO("module", "                candidate_specs = [{}]", result);
        LOG_INFO("module", ">>>>> RANDOM_ENCHANT DEBUG PRINT END <<<<<");
    }
    struct retVals {
        uint32 enchCatMask, attrMask;
        bool hasEnch;
    };
    if (specPool.empty()) {
        LOG_ERROR("module", "RANDOM_ENCHANT: ERROR Spec pool is empty somehow");
        return retVals{0, 0, false};
    }
    auto chosenSpec = Acore::Containers::SelectRandomContainerElement(specPool);
    uint8 plrClass = 0;
    uint32 plrSpec = 0;
    bool isFound = false;
    if (auto found = specToClass.find(chosenSpec); found != specToClass.end()) {
        isFound = true;
        plrClass = found->second;
        plrSpec = found->first;
    }
    if (!isFound) {
        // should not be the case
        LOG_ERROR("module", "RANDOM_ENCHANT: ERROR the chosen spec is not found: chosen spec was: {}", chosenSpec);
        return retVals{0, 0, false};
    }
    auto [enchCatMask, attrMask] = getEnchantCategoryMaskByClassAndSpec(plrClass, plrSpec);
    if (config_debug)
    {
        LOG_INFO("module", ">>>>> RANDOM_ENCHANT DEBUG PRINT CHOSEN ITEM SPEC START <<<<<");
        LOG_INFO("module", "RANDOM_ENCHANT: CHOSEN SPEC: {}; PLAYER CLASS: {}", plrSpec, plrClass);
        LOG_INFO("module", "                enchMask: {}", enchCatMask);
        LOG_INFO("module", "                attrMask: {}", attrMask);
        LOG_INFO("module", ">>>>> RANDOM_ENCHANT DEBUG PRINT CHOSEN ITEM SPEC END <<<<<");
    }
    return retVals{enchCatMask, attrMask, true};
}

auto getPlayerItemEnchantCategoryMask(Item* item, Player* player = nullptr)
{
    struct retVals {
        uint32 enchCatMask, attrMask;
        bool found;
    };
    if (config_roll_player_class_preference && player->CanUseItem(item, false) == EQUIP_ERR_OK)
    {
        if (config_debug)
        {
            LOG_INFO("module", "RANDOM_ENCHANT: Getting player class preference for enchant category");
        }
        auto [enchMask, attrMask] = getPlayerEnchantCategoryMask(player);
        return retVals{enchMask, attrMask, true};
    }
    if (config_debug)
    {
        LOG_INFO("module", "RANDOM_ENCHANT: Getting item enchant category");
    }
    auto [enchMask, attrMask, found] = getItemEnchantCategoryMask(item);
    return retVals{enchMask, attrMask, found};
}

// END UTILS

// MAIN GET ROLL ENCHANT FUNCTIONS

int32 getCustomRandomSuffix(int enchantQuality, Item* item, Player* player = nullptr)
{
    uint32 Class = item->GetTemplate()->Class;
    uint32 subclassMask = 1 << item->GetTemplate()->SubClass;
    // int level = getLevelOffset(item, player);
    int level = getItemPlayerLevel(item);
    auto [enchantCategoryMask, attrMask, found] = getPlayerItemEnchantCategoryMask(item, player);
    if (!found) {
        return -1;
    }

    int maxCount = 50;
    while (maxCount > 0)
    {
    QueryResult qr = WorldDatabase.Query(R"(SELECT ID FROM item_enchantment_random_suffixes
INNER JOIN itemrandomsuffix_dbc ON
item_enchantment_random_suffixes.SuffixID = itemrandomsuffix_dbc.ID
WHERE
((MinLevel <= {} AND {} <= MaxLevel) OR (MinLevel = 0 AND MaxLevel = 0))
AND (
(ItemClass = 0) OR
(ItemClass = {} AND ItemSubClassMask = 0) OR
(ItemClass = {} AND ItemSubClassMask & {} > 0)
)
AND EnchantQuality = {}
AND (
  (AttributeMask = 0) OR
  ((AttributeMask & {} > 0) AND (AttributeMask & ~{} = 0))
)
AND (
  (EnchantCategoryMask = 0) OR
  (EnchantCategoryMask & {} > 0)
) ORDER BY RAND() LIMIT 1)", level, level, Class, Class, subclassMask, enchantQuality, attrMask, attrMask, enchantCategoryMask);
        if (qr)
        {
            int suffixID = qr->Fetch()[0].Get<uint32>();
            ItemRandomSuffixEntry const* item_rand = sItemRandomSuffixStore.LookupEntry(suffixID);
            if (!item_rand)
            {
                LOG_INFO("module", "Suffix ID does not exist to be enchanted, getting a new one: {}", suffixID);
                // get suffixID failed for some reason, should not happen, we still just continue and try to get
                // another one.
                maxCount--;
                continue;
            }
            uint32 minAllocPct = 3567587328;
            for (uint8 k = 0; k != MAX_ITEM_ENCHANTMENT_EFFECTS; ++k)
            {
                if (item_rand->AllocationPct[k] > 100)
                {
                    // NOTE: Any value set below 100, is either a 0, or a 1, which is used to denote either not set,
                    // or set but not really a stat ench, this is *Hardcoded*.
                    if (minAllocPct > item_rand->AllocationPct[k])
                    {
                        minAllocPct = item_rand->AllocationPct[k];
                    }
                }
            }
            auto suffFactor = GenerateEnchSuffixFactor(item->GetTemplate()->ItemId);
            if (config_debug)
            {
                LOG_INFO("module", "RANDOM_ENCHANT: Suffix factor for item {}, Item ID is: {} is {}", item->GetTemplate()->Name1, item->GetTemplate()->ItemId, suffFactor);
            }
            int32 basepoints = int32(minAllocPct * suffFactor / 10000);
            if (basepoints < 1)
            {
                // Suffix points should ideally be above 1 after suffix factor calculations
                // This is so that when presented on the client we dont get some weird looking values
                LOG_INFO("module", "Suffix min alloc pct calculation is below one, getting a new one: suffID: {}, suffFactor: {}, minAllocPct: {}", suffixID, suffFactor, minAllocPct);
                maxCount--;
                continue;
            }
            if (config_debug)
            {
                LOG_INFO("module", "RANDOM_ENCHANT: Query with the following params:");
                LOG_INFO("module", "                level {}, enchantQuality {}, item_class {}, subclassmask {}, enchCatMask {}, attrMask {}", level, enchantQuality, Class, subclassMask, enchantCategoryMask, attrMask);
                LOG_INFO("module", "                Return was: {}", suffixID);
            }
            return suffixID;
        }
        LOG_INFO("module", "RANDOM_ENCHANT: No suffixes found for this combi");
        LOG_INFO("module", "                level {}, enchantQuality {}, item_class {}, subclassmask {}, enchCatMask {}, attrMask {}", level, enchantQuality, Class, subclassMask, enchantCategoryMask, attrMask);
        // get suffixID failed for some reason here too. probably no entries.
        maxCount--;
    }
    LOG_INFO("module", "rerolled rolls a max number of times already times, but no candidate enchants, returning without a suffix");
    return -1;
}

int GetRolledEnchantLevel()
{
    int currentTier = -1;
    for (auto rollpct: config_enchant_pcts)
    {
        double roll = (float)rand_chance();
        if (roll + rollpct < 100.0)
        {
            // If roll was not successful, we break, no more attempted rolls beyond this;
            break;
        }
        currentTier++;
    }
    return currentTier;
}

void RollPossibleEnchant(Player* player, Item* item)
{
    uint32 Quality = item->GetTemplate()->Quality;
    uint32 Class = item->GetTemplate()->Class;

    switch (item->GetTemplate()->InventoryType)
    {
        // Dont roll if its of these types (Taken from GenerateEnchSuffixFactor)
        // Items of that type don`t have points
        case INVTYPE_NON_EQUIP:
        case INVTYPE_BAG:
        case INVTYPE_TABARD:
        case INVTYPE_AMMO:
        case INVTYPE_QUIVER:
        // case INVTYPE_RELIC: // core changes will allow enchants of relics as well
            return;
    }
    if (
        (Quality > ITEM_QUALITY_LEGENDARY || Quality < ITEM_QUALITY_UNCOMMON) /* eliminates enchanting anything that isn't a recognized quality */ ||
        (Class != ITEM_CLASS_WEAPON && Class != ITEM_CLASS_ARMOR) /* eliminates enchanting anything but weapons/armor */)
    {
        return;
    }

    if (item->GetItemRandomPropertyId() != 0)
    {
        // If already have enchant, we shouldnt apply a new one
        return;
    }

    auto rolledEnchantLevel = GetRolledEnchantLevel();
    if (rolledEnchantLevel < 0)
    {
        // Failed roll
        return;
    }
    auto suffixID = getCustomRandomSuffix(rolledEnchantLevel, item, player);
    if (suffixID < 0)
    {
        return;
    }
    // Apply the suffix to the item
    ItemRandomSuffixEntry const* item_rand = sItemRandomSuffixStore.LookupEntry(suffixID);
    if (!item_rand)
    {
        return;
    }
    item->SetItemRandomProperties(-suffixID);
    ChatHandler chathandle = ChatHandler(player->GetSession());
    uint32 loc = player->GetSession()->GetSessionDbLocaleIndex();
    std::string suffixName = item_rand->Name[loc];
    chathandle.PSendSysMessage("|cffFF0000 %s |rhas rolled the suffix|cffFF0000 %s |r!", item->GetTemplate()->Name1.c_str(), suffixName);
}

// END MAIN GET ROLL ENCHANTS FUNCTIONS

class RandomEnchantsWorldScript : public WorldScript
{
public:
    RandomEnchantsWorldScript() : WorldScript("RandomEnchantsWorldScript") { }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        config_announce_on_log = sConfigMgr->GetOption<bool>("RandomEnchants.AnnounceOnLogin", default_announce_on_log);
        config_debug = sConfigMgr->GetOption<bool>("RandomEnchants.Debug", default_debug);
        config_on_loot = sConfigMgr->GetOption<bool>("RandomEnchants.OnLoot", default_on_loot);
        config_on_create = sConfigMgr->GetOption<bool>("RandomEnchants.OnCreate", default_on_create);
        config_on_quest_reward = sConfigMgr->GetOption<bool>("RandomEnchants.OnQuestReward", default_on_quest_reward);
        config_on_group_roll_reward_item = sConfigMgr->GetOption<bool>("RandomEnchants.OnGroupRollRewardItem", default_on_group_roll_reward_item);
        config_on_vendor_purchase = sConfigMgr->GetOption<bool>("RandomEnchants.OnVendorPurchase", default_on_vendor_purchase);
        // config_on_all_items_created = sConfigMgr->GetOption<bool>("RandomEnchants.OnAllItemsCreated", default_on_all_items_created);
        config_roll_player_class_preference =  sConfigMgr->GetOption<bool>("RandomEnchants.RollPlayerClassPreference", default_roll_player_class_preference);
        config_login_message = sConfigMgr->GetOption<std::string>("RandomEnchants.OnLoginMessage", default_login_message);
        config_enchant_pcts[0] = sConfigMgr->GetOption<float>("RandomEnchants.RollPercentage.1", default_enchant_pcts[0]);
        config_enchant_pcts[1] = sConfigMgr->GetOption<float>("RandomEnchants.RollPercentage.2", default_enchant_pcts[1]);
        config_enchant_pcts[2] = sConfigMgr->GetOption<float>("RandomEnchants.RollPercentage.3", default_enchant_pcts[2]);
        config_enchant_pcts[3] = sConfigMgr->GetOption<float>("RandomEnchants.RollPercentage.4", default_enchant_pcts[3]);
    }
};

class RandomEnchantsPlayer : public PlayerScript{
public:

    RandomEnchantsPlayer() : PlayerScript("RandomEnchantsPlayer") { }

    void OnLogin(Player* player) override {
        if (config_announce_on_log)
        {
            ChatHandler(player->GetSession()).SendSysMessage(config_login_message);
        }
    }
    void OnStoreNewItem(Player* player, Item* item, uint32 /*count*/) override
    {
        if (/*!HasBeenTouchedByRandomEnchantMod(item) && */config_on_loot)

            RollPossibleEnchant(player, item);
    }
    void OnCreateItem(Player* player, Item* item, uint32 /*count*/) override
    {
        if (/*!HasBeenTouchedByRandomEnchantMod(item) && */config_on_create)
            RollPossibleEnchant(player, item);
    }
    void OnQuestRewardItem(Player* player, Item* item, uint32 /*count*/) override
    {
        if(/*!HasBeenTouchedByRandomEnchantMod(item) && */config_on_quest_reward)
            RollPossibleEnchant(player, item);
    }
    void OnGroupRollRewardItem(Player* player, Item* item, uint32 /*count*/, RollVote /*voteType*/, Roll* /*roll*/) override
    {
        if (/*!HasBeenTouchedByRandomEnchantMod(item) && */config_on_group_roll_reward_item)
        {
            RollPossibleEnchant(player, item);
        }
    }
    void OnAfterStoreOrEquipNewItem(Player* player, uint32 /*vendorslot*/, Item* item, uint8 /*count*/, uint8 /*bag*/, uint8 /*slot*/, ItemTemplate const* /*pProto*/, Creature* /*pVendor*/, VendorItem const* /*crItem*/, bool /*bStore*/) override
    {
        if (/*!HasBeenTouchedByRandomEnchantMod(item) && */config_on_vendor_purchase)
        {
            RollPossibleEnchant(player, item);
        }
    }
};

// class RandomEnchantsMisc : public MiscScript{
// public:

//     RandomEnchantsMisc() : MiscScript("RandomEnchantsMisc") { }

//     void OnItemCreate(Item* item, ItemTemplate const* /*itemProto*/, Player const* owner) override
//     {
//         if (config_on_all_items_created)
//         {
//             if (!owner) {
//                 return;
//             }
//             Player* player = const_cast<Player*>(owner);
//             if (!player || !player->FindMap())
//             {
//                 return;
//             }
//             RollPossibleEnchant(player, item);
//         }
//     }
// };


using namespace Acore::ChatCommands;

class RandomEnchantCommands : public CommandScript
{
public:
    RandomEnchantCommands() : CommandScript("RandomEnchantCommands") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
        {
            { "additemwsuffix",           HandleAddItemCommand,           SEC_GAMEMASTER,         Console::No  },
        };
        return commandTable;
    }
    static bool HandleAddItemCommand(ChatHandler* handler, ItemTemplate const* itemTemplate, Optional<int32> _count, Optional<int32> _suffID)
    {
        if (!sObjectMgr->GetItemTemplate(itemTemplate->ItemId))
        {
            handler->PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemTemplate->ItemId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 itemId = itemTemplate->ItemId;
        int32 count = 1;

        if (_count)
        {
            count = *_count;
        }

        if (!count)
        {
            count = 1;
        }
        int32 suffID = 0;
        if (_suffID)
        {
            suffID = *_suffID;
        }

        Player* player = handler->GetSession()->GetPlayer();
        Player* playerTarget = handler->getSelectedPlayer();

        if (!playerTarget)
        {
            playerTarget = player;
        }

        // Subtract
        if (count < 0)
        {
            // Only have scam check on player accounts
            if (playerTarget->GetSession()->GetSecurity() == SEC_PLAYER)
            {
                if (!playerTarget->HasItemCount(itemId, 0))
                {
                    // output that player don't have any items to destroy
                    handler->PSendSysMessage(LANG_REMOVEITEM_FAILURE, handler->GetNameLink(playerTarget).c_str(), itemId);
                    handler->SetSentErrorMessage(true);
                    return false;
                }

                if (!playerTarget->HasItemCount(itemId, -count))
                {
                    // output that player don't have as many items that you want to destroy
                    handler->PSendSysMessage(LANG_REMOVEITEM_ERROR, handler->GetNameLink(playerTarget).c_str(), itemId);
                    handler->SetSentErrorMessage(true);
                    return false;
                }
            }

            // output successful amount of destroyed items
            playerTarget->DestroyItemCount(itemId, -count, true, false);
            handler->PSendSysMessage(LANG_REMOVEITEM, itemId, -count, handler->GetNameLink(playerTarget).c_str());
            return true;
        }

        // Adding items
        uint32 noSpaceForCount = 0;

        // check space and find places
        ItemPosCountVec dest;
        InventoryResult msg = playerTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);

        if (msg != EQUIP_ERR_OK) // convert to possible store amount
        {
            count -= noSpaceForCount;
        }

        if (!count || dest.empty()) // can't add any
        {
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Item* item = playerTarget->StoreNewItem(dest, itemId, true);

        // remove binding (let GM give it to another player later)
        if (player == playerTarget)
        {
            for (auto const& itemPos : dest)
            {
                if (Item* item1 = player->GetItemByPos(itemPos.pos))
                {
                    item1->SetBinding(false);
                }
            }
        }

        if (count && item)
        {
            item->SetItemRandomProperties(suffID);
            player->SendNewItem(item, count, false, true);

            if (player != playerTarget)
            {
                playerTarget->SendNewItem(item, count, true, false);
            }
        }

        if (noSpaceForCount)
        {
            handler->PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
        }

        return true;
    }
};

void AddRandomEnchantsScripts() {
    new RandomEnchantsWorldScript();
    new RandomEnchantsPlayer();
    new RandomEnchantCommands();
    // new RandomEnchantsMisc();
}
