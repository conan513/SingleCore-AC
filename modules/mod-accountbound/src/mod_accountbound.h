#ifndef MOD_ACCOUNTBOUND
#define MOD_ACCOUNTBOUND

#include "Chat.h"
#include "Config.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

struct Companions
{
    uint32 SpellId;
    uint32 AllowableRace;
};

struct FactionSpecificCompanions
{
    uint32 AllianceId;
    uint32 HordeId;
};

struct Mounts
{
    uint32 SpellId;
    uint32 AllowableRace;
    uint32 AllowableClass;
    uint32 RequiredLevel;
    uint32 RequiredSkill;
    uint32 RequiredSkillRank;
};

struct FactionSpecificMounts
{
    uint32 AllianceId;
    uint32 HordeId;
};

enum Achievements
{
    ACHIEVEMENT_APPRENTICE = 891,
    ACHIEVEMENT_JOURNEYMAN = 889,
    ACHIEVEMENT_EXPERT = 890,
    ACHIEVEMENT_ARTISAN = 892
};

class AccountBound : public CreatureScript, PlayerScript, WorldScript
{
public:
    AccountBound();

    // CreatureScript
    bool OnGossipHello(Player* /*player*/, Creature* /*creature*/) override;

    // PlayerScript
    void OnAchiComplete(Player* /*player*/, AchievementEntry const* /*achievement*/) override;
    void OnAfterStoreOrEquipNewItem(Player* /*player*/, uint32 /*vendorslot*/, Item* /*item*/, uint8 /*count*/, uint8 /*bag*/, uint8 /*slot*/, ItemTemplate const* /*pProto*/, Creature* /*pVendor*/, VendorItem const* /*crItem*/, bool /*bStore*/) override;
    void OnLevelChanged(Player* /*player*/, uint8 /*oldlevel*/) override;
    void OnLogin(Player* /*player*/) override;
    void OnSave(Player* /*player*/) override;

    // WorldScript
    void OnAfterConfigLoad(bool /*reload*/) override;
    void OnStartup() override;

private:
    std::vector<Companions> companions;
    std::vector<FactionSpecificCompanions> factionSpecificCompanions;
    std::vector<Mounts> mounts;
    std::vector<FactionSpecificMounts> factionSpecificMounts;

    bool EnableGamemasters;
    bool EnableCompanions;
    bool EnableHeirlooms;
    bool EnableMounts;

    void LoadCompanions();
    void LoadFactionSpecificCompanions();
    void SaveCompanions(Player* /*player*/);
    void LearnCompanions(Player* /*player*/);
    uint32 GetFactionSpecificCompanionId(uint32 /*spell_id*/);
    void LoadMounts();
    void SaveMounts(Player* /*player*/);
    void LearnMounts(Player* /*player*/);
    void LoadFactionSpecificMounts();
    uint32 GetFactionSpecificMountId(uint32 /*spell_id*/);
    void SaveHeirloom(Player* /*player*/, Item* /*item*/);
};

#endif
