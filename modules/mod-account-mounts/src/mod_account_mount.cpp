#include "Config.h"
#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include <set>         // Required for std::set             -- might be redundant
#include <sstream>     // Required for std::istringstream   -- might be redundant
#include <string>      // Required for std::string          -- might be redundant

class AccountMounts : public PlayerScript
{
    static const bool limitrace = true; // This set to true will only learn mounts from chars on the same team, do what you want.
    std::set<uint32> excludedSpellIds; // Set to hold the Spell IDs to be excluded

public:
    AccountMounts() : PlayerScript("AccountMounts")
    {
        // Retrieve the string of excluded Spell IDs from the config file
        std::string excludedSpellsStr = sConfigMgr->GetOption<std::string>("Account.Mounts.ExcludedSpellIDs", "");
        // Proceed only if the configuration is not "0" or empty, indicating exclusions are specified
        if (excludedSpellsStr != "0" && !excludedSpellsStr.empty())
        {
            std::istringstream spellStream(excludedSpellsStr);
            std::string spellIdStr;
            while (std::getline(spellStream, spellIdStr, ',')) {
                uint32 spellId = static_cast<uint32>(std::stoul(spellIdStr));
                if (spellId != 0) { // Ensure the spell ID is not 0, as 0 is used to indicate no exclusions
                    excludedSpellIds.insert(spellId); // Add the Spell ID to the set of exclusions
                }
            }
        }
    }
    
    void OnLogin(Player* pPlayer)
    {
        if (sConfigMgr->GetOption<bool>("Account.Mounts.Enable", true))
        {
            if (sConfigMgr->GetOption<bool>("Account.Mounts.Announce", false))
            {
                ChatHandler(pPlayer->GetSession()).SendSysMessage("This server is running the |cff4CFF00AccountMounts |rmodule.");
            }

            std::vector<uint32> Guids;
            uint32 playerAccountID = pPlayer->GetSession()->GetAccountId();
            QueryResult result1 = CharacterDatabase.Query("SELECT `guid`, `race` FROM `characters` WHERE `account`={};", playerAccountID);

            if (!result1)
                return;

            do
            {
                Field* fields = result1->Fetch();
                uint32 race = fields[1].Get<uint8>();

                if ((Player::TeamIdForRace(race) == Player::TeamIdForRace(pPlayer->getRace())) || !limitrace)
                    Guids.push_back(fields[0].Get<uint32>());

            } while (result1->NextRow());

            std::vector<uint32> Spells;

            for (auto& i : Guids)
            {
                QueryResult result2 = CharacterDatabase.Query("SELECT `spell` FROM `character_spell` WHERE `guid`={};", i);
                if (!result2)
                    continue;

                do
                {
                    Spells.push_back(result2->Fetch()[0].Get<uint32>());
                } while (result2->NextRow());
            }

            for (auto& i : Spells)
            {
                // Check if the spell is in the excluded list before learning it
                if (excludedSpellIds.find(i) == excludedSpellIds.end())
                {
                    auto sSpell = sSpellStore.LookupEntry(i);
                    if (sSpell->Effect[0] == SPELL_EFFECT_APPLY_AURA && sSpell->EffectApplyAuraName[0] == SPELL_AURA_MOUNTED)
                        pPlayer->learnSpell(sSpell->Id);
                }
            }
        }
    }
};

void AddAccountMountsScripts()
{
    new AccountMounts();
}
