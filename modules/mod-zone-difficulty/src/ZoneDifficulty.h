#ifndef DEF_ZONEDIFFICULTY_H
#define DEF_ZONEDIFFICULTY_H

#include "Player.h"
#include "Config.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"

struct ZoneDifficultyNerfData
{
    float HealingNerfPct;
    float AbsorbNerfPct;
    float SpellDamageBuffPct;
    float MeleeDamageBuffPct;
    int8 Enabled;
    float HealingNerfPctHard;
    float AbsorbNerfPctHard;
    float SpellDamageBuffPctHard;
    float MeleeDamageBuffPctHard;
};

struct ZoneDifficulySpellOverrideData
{
    float NerfPct;
    uint32 ModeMask;    // 1=normal, 64=mythic (bitmask)
};

struct ZoneDifficultyMythicmodeMapData
{
    uint32 EncounterEntry;
    uint32 Override;
    uint32 RewardType;
};

struct ZoneDifficultyRewardData
{
    uint32 Entry;
    uint32 Price;
    uint32 Enchant;
    uint8 EnchantSlot;
    int32 Achievement;
};

struct ZoneDifficultyHAI
{
    uint8 Chance;
    uint32 Spell;
    int32 Spellbp0;
    int32 Spellbp1;
    int32 Spellbp2;
    uint8 Target;
    int8 TargetArg;
    uint8 TargetArg2;
    Milliseconds Delay;
    Milliseconds Cooldown;
    uint8 Repetitions;
    bool TriggeredCast;
};

int32 const DUEL_INDEX = 0x7FFFFFFF;
int32 const DUEL_AREA = 2402;       // Forbidding Sea (Wetlands)

uint32 const NPC_TEXT_LEADER_NORMAL = 91301;
uint32 const NPC_TEXT_OTHER = 91302;
uint32 const NPC_TEXT_LEADER_HARD = 91303;
uint32 const NPC_TEXT_LEADER_FINAL = 91304;
uint32 const NPC_TEXT_OFFER = 91305;
uint32 const NPC_TEXT_CATEGORY = 91306;
uint32 const NPC_TEXT_ITEM = 91307;
uint32 const NPC_TEXT_CONFIRM = 91308;
uint32 const NPC_TEXT_GRANT = 91309;
uint32 const NPC_TEXT_DENIED = 91310;
uint32 const NPC_TEXT_SCORE = 91311;

int32 const MODE_NORMAL = 1;
int32 const MODE_HARD = 64;

// EVENT_GROUP is used for unit->m_Events.AddEventAtOffset
uint8 const EVENT_GROUP = 64;

int32 const TYPE_VANILLA = 1;
int32 const TYPE_RAID_MC = 2;
int32 const TYPE_RAID_ONY = 3;
int32 const TYPE_RAID_BWL = 4;
int32 const TYPE_RAID_ZG = 5;
int32 const TYPE_RAID_AQ20 = 6;
int32 const TYPE_RAID_AQ40 = 7;
int32 const TYPE_HEROIC_TBC = 8;
int32 const TYPE_RAID_T4 = 9;
int32 const TYPE_RAID_T5 = 10;
int32 const TYPE_RAID_T6 = 11;
int32 const TYPE_HEROIC_WOTLK = 12;
int32 const TYPE_RAID_T7 = 13;
int32 const TYPE_RAID_T8 = 14;
int32 const TYPE_RAID_T9 = 15;
int32 const TYPE_RAID_T10 = 16;

uint32 const ITEMTYPE_MISC = 1;
uint32 const ITEMTYPE_CLOTH = 2;
uint32 const ITEMTYPE_LEATHER = 3;
uint32 const ITEMTYPE_MAIL = 4;
uint32 const ITEMTYPE_PLATE = 5;
uint32 const ITEMTYPE_WEAPONS = 6;

uint32 const TARGET_NONE = 0;
uint32 const TARGET_SELF = 1;
uint32 const TARGET_VICTIM = 2;                      // current target
uint32 const TARGET_HOSTILE_AGGRO_FROM_TOP = 3;      // count TargetArg highest aggro from top within TargetArg2 distance
uint32 const TARGET_HOSTILE_AGGRO_FROM_BOTTOM = 4;   // count TargetArg lowest aggro from bottom within TargetArg2 distance
uint32 const TARGET_HOSTILE_RANDOM = 5;              // any random player from the threat list
uint32 const TARGET_HOSTILE_RANDOM_NOT_TOP = 6;      // any random player from the threat list except the current target
uint32 const TARGET_PLAYER_DISTANCE = 18;            // a random player within TargetArg range

const std::string REWARD_MAIL_SUBJECT = "Chromie's Reward for you";
const std::string REWARD_MAIL_BODY = "Enjoy your new item!";

class ZoneDifficulty
{
public:
    static ZoneDifficulty* instance();

    void LoadMapDifficultySettings();
    void SaveMythicmodeInstanceData(uint32 instanceId);
    void LoadMythicmodeInstanceData();
    void LoadMythicmodeScoreData();
    void SendWhisperToRaid(std::string message, Creature* creature, Player* player);
    std::string GetItemTypeString(uint32 type);
    std::string GetContentTypeString(uint32 type);
    void AddMythicmodeScore(Map* map, uint32 type, uint32 score);
    void DeductMythicmodeScore(Player* player, uint32 type, uint32 score);
    void SendItem(Player* player, uint32 category, uint32 itemType, uint32 id);
    std::list<Unit*> GetTargetList(Unit* unit, uint32 entry, uint32 key);
    void MythicmodeEvent(Unit* unit, uint32 entry, uint32 key);
    bool HasNormalMode(int8 mode) { return (mode & MODE_NORMAL) == MODE_NORMAL; }
    bool HasMythicmode(int8 mode) { return (mode & MODE_HARD) == MODE_HARD; }
    bool HasCompletedFullTier(uint32 category, uint32 playerGUID);
    bool OverrideModeMatches(uint32 instanceId, uint32 spellId, uint32 mapId);
    [[nodiscard]] bool IsValidNerfTarget(Unit* target);
    [[nodiscard]] bool VectorContainsUint32(std::vector<uint32> vec, uint32 element);
    [[nodiscard]] bool IsMythicmodeMap(uint32 mapid);
    [[nodiscard]] bool ShouldNerfInDuels(Unit* target);
    [[nodiscard]] int32 GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask);

    bool IsEnabled{ false };
    bool IsDebugInfoEnabled{ false };
    float MythicmodeHpModifier{ 2.0 };
    bool MythicmodeEnable{ false };
    bool MythicmodeInNormalDungeons{ false };
    std::vector<uint32> DailyHeroicQuests;
    std::map<uint32, uint32> HeroicTBCQuestMapList;
    std::map<uint32, uint8> EncounterCounter;
    std::map<uint32, uint8> Expansion;
    std::map<uint32, float> CreatureOverrides;
    std::map<uint32, uint32> EncountersInProgress;
    std::map<uint32, std::string> ItemIcons;
    std::map<uint8, ZoneDifficultyRewardData> TierRewards;

    typedef std::map<uint32, std::map<uint32, ZoneDifficultyNerfData> > ZoneDifficultyNerfDataMap;
    ZoneDifficultyNerfDataMap NerfInfo;
    typedef std::map<uint32, std::map<uint32, ZoneDifficulySpellOverrideData> > ZoneDifficultySpellNerfMap;
    ZoneDifficultySpellNerfMap SpellNerfOverrides;
    typedef std::map<uint32, std::vector<uint32> > ZoneDifficultyDisablesMap;
    ZoneDifficultyDisablesMap DisallowedBuffs;
    typedef std::map<uint32, bool> ZoneDifficultyMythicmodeInstDataMap;
    ZoneDifficultyMythicmodeInstDataMap MythicmodeInstanceData;
    typedef std::map<uint32, std::vector<ZoneDifficultyMythicmodeMapData> > ZoneDifficultyMythicmodeLootMap;
    ZoneDifficultyMythicmodeLootMap MythicmodeLoot;
    typedef std::map<uint32, std::map<uint32, uint32> > ZoneDifficultyDualUintMap;
    ZoneDifficultyDualUintMap MythicmodeScore;
    typedef std::map<uint32, std::map<uint32, std::vector<ZoneDifficultyRewardData> > > ZoneDifficultyRewardMap;
    ZoneDifficultyRewardMap Rewards;
    typedef std::map<uint32, std::vector<ZoneDifficultyHAI> > ZoneDifficultyHAIMap;
    ZoneDifficultyHAIMap MythicmodeAI;
    typedef std::map<uint32, std::map<uint32, std::map<uint32, bool> > > ZoneDifficultyEncounterLogMap;
    ZoneDifficultyEncounterLogMap Logs;
};

#define sZoneDifficulty ZoneDifficulty::instance()

#endif
