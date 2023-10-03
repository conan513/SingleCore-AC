#include "class_trial_loader.h"
#include "Chat.h"
#include "Config.h"
#include "MapMgr.h"
#include "Player.h"
#include "ScriptMgr.h"

class TrialOperation : public PlayerScript {
public:
    TrialOperation() : PlayerScript("TrialOperation") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("AnnounceEnable", true))
            ChatHandler(player->GetSession()).SendSysMessage(ALERT_MODULE_PRESENCE);

        if (!isTrialCharacter(player))
            return; // Not a class trial, don't do anything.
        if (sConfigMgr->GetOption<bool>("AllowCustomize", true))
            player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
        if (sConfigMgr->GetOption<bool>("AllowRaceChange", true))
            player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
        if ((player->m_Played_time[PLAYED_TIME_TOTAL] >= sConfigMgr->GetOption<uint32>("TrialPlaytimeLimit", 0)) && sConfigMgr->GetOption<uint32>("TrialPlaytimeLimit", 0) != 0)
            trialDelete(player);
        return;
    };

    bool CanInitTrade(Player* player, Player* target) override
    {
        if ((isTrialCharacter(player) || isTrialCharacter(target)) && sConfigMgr->GetOption<bool>("RestrictTrade", true))
            return false;
        else
            return true;
    }

    bool CanSendMail(Player* player, ObjectGuid /*receiverGuid*/, ObjectGuid /*mailbox*/, std::string& /*subject*/, std::string& /*body*/, uint32 /*money*/, uint32 /*COD*/, Item* /*item*/) override
    {
        if (!isTrialCharacter(player))
            return true;
        if (sConfigMgr->GetOption<bool>("RestrictMail", true))
            return false;
        return true;
    }

    bool CanEnterMap(Player* player, MapEntry const* entry, InstanceTemplate const* /*instance*/, MapDifficulty const* /*mapDiff*/, bool /*loginCheck*/) override
    {
        if (!isTrialCharacter(player))
            return true;

        AchievementCriteriaEntry const* dungeonCount = sAchievementCriteriaStore.LookupEntry(932);
        AchievementCriteriaEntry const* raidCountOne = sAchievementCriteriaStore.LookupEntry(933); // 10-man
        AchievementCriteriaEntry const* raidCountTwo = sAchievementCriteriaStore.LookupEntry(934); // 25-man
        uint32 combinedRaidCount = (player->GetAchievementMgr()->GetCriteriaProgress(raidCountOne)->counter + player->GetAchievementMgr()->GetCriteriaProgress(raidCountTwo)->counter);

        if ((player->GetAchievementMgr()->GetCriteriaProgress(dungeonCount)->counter > sConfigMgr->GetOption<uint32>("TrialDungeonLimit", 0)) && sMapStore.LookupEntry(entry->MapID)->IsDungeon())
            return false;
        if ((combinedRaidCount > sConfigMgr->GetOption<uint32>("TrialRaidLimit", 0)) && sMapStore.LookupEntry(entry->MapID)->IsRaid())
            return false;

        return true;
    }

    bool CanJoinInBattlegroundQueue(Player* player, ObjectGuid /*BattlemasterGuid*/, BattlegroundTypeId /*BGTypeID*/, uint8 /*joinAsGroup*/, GroupJoinBattlegroundResult& /*err*/) override
    {
        if (!isTrialCharacter(player))
            return true;

        AchievementCriteriaEntry const* bgCount = sAchievementCriteriaStore.LookupEntry(839);

        if (player->GetAchievementMgr()->GetCriteriaProgress(bgCount)->counter > sConfigMgr->GetOption<uint32>("TrialBattlegroundLimit", 0))
            return false;
        return true;
    }

    bool CanJoinInArenaQueue(Player* player, ObjectGuid /*BattlemasterGuid*/, uint8 /*arenaslot*/, BattlegroundTypeId /*BGTypeID*/, uint8 /*joinAsGroup*/, uint8 IsRated, GroupJoinBattlegroundResult& /*err*/) override
    {
        if (!isTrialCharacter(player))
            return true;

        AchievementCriteriaEntry const* arenaCount = sAchievementCriteriaStore.LookupEntry(838);

        if ((player->GetAchievementMgr()->GetCriteriaProgress(arenaCount)->counter > sConfigMgr->GetOption<uint32>("TrialArenaLimit", 0)) && IsRated)
            return false;
        return true;
    }

    static void imbueTrial(Player* player)
    {
        enlistTrial(player);
    }

    static void enlistTrial(Player* player)
    {
        WorldDatabase.Execute("INSERT INTO mod_classtrial_characters (GUID) VALUES ({})", player->GetGUID().GetCounter());
        LOG_DEBUG("module", "Enlisted character {} with a class trial.", player->GetGUID().GetCounter());
    }

    static bool isTrialCharacter(Player* player)
    {
        LOG_DEBUG("module", "Checking for trial status for character {}.", player->GetGUID().GetCounter());
        QueryResult result = WorldDatabase.Query("SELECT GUID FROM mod_classtrial_characters WHERE GUID = {}", player->GetGUID().GetCounter());
        if (result)
            return true;
        else
            return false;
    }

    static void trialDelete(Player* player)
    {
        uint32 account = player->GetSession()->GetAccountId();
        ObjectGuid guid = player->GetGUID();
        player->GetSession()->KickPlayer();
        player->DeleteFromDB(guid.GetCounter(), account, true, true);
        WorldDatabase.Execute("DELETE FROM mod_classtrial_characters WHERE GUID = {}", player->GetGUID().GetCounter());
    }

private:
    enum moduleStrings
    {
        ALERT_MODULE_PRESENCE = 40050
    };
};

using namespace Acore::ChatCommands;

class TrialCommands : public CommandScript {
public:
    TrialCommands() : CommandScript("TrialCommands") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable commandTable =
        {
            { "classtrial", trialHelper, SEC_PLAYER, Console::No },
        };
        return commandTable;
    }

    static bool trialHelper(ChatHandler* handler)
    {
        if (!sConfigMgr->GetOption<bool>("ClassTrialEnable", true))
        {
            handler->SendSysMessage(ALERT_MODULE_DISABLED);
            return true;
        }
        TrialOperation::imbueTrial(handler->GetPlayer());
        return true;
    }

private:
    enum moduleStrings
    {
        ALERT_MODULE_DISABLED = 40051
    };
};

void Add_class_trial()
{
    new TrialOperation();
}

void AddSC_class_trial_commandscript()
{
    new TrialCommands();
}
