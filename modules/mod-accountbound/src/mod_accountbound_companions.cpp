#include "mod_accountbound.h"

void AccountBound::LoadCompanions()
{
    QueryResult result = LoginDatabase.Query("SELECT `spell_id`, `allowable_race` FROM `account_bound_companion_template`");

    if (!result)
    {
        LOG_INFO("server.loading", ">> Loaded 0 companion templates");
        return;
    }

    companions.clear();

    int i = 0;
    do
    {
        companions.push_back(Companions());
        Field* fields = result->Fetch();
        companions[i].SpellId = fields[0].Get<int32>();
        companions[i].AllowableRace = fields[1].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} companion templates", i);
}

void AccountBound::LoadFactionSpecificCompanions()
{
    QueryResult result = LoginDatabase.Query("SELECT `alliance_id`, `horde_id` FROM `account_bound_factionchange_spells` pfs LEFT OUTER JOIN "
        "`account_bound_companion_template` abt ON pfs.`alliance_id` = abt.`spell_id` WHERE abt.`allowable_race` = 1101");

    if (!result)
    {
        LOG_INFO("server.loading", ">> Loaded 0 faction-specific companion templates");
        return;
    }

    factionSpecificCompanions.clear();

    int i = 0;
    do
    {
        factionSpecificCompanions.push_back(FactionSpecificCompanions());
        Field* fields = result->Fetch();
        factionSpecificCompanions[i].AllianceId = fields[0].Get<int32>();
        factionSpecificCompanions[i].HordeId = fields[1].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} faction-specific companion templates", i);
}

void AccountBound::SaveCompanions(Player* player)
{
    std::string data = "";

    for (auto& companion : companions)
    {
        if (!player->HasSpell(companion.SpellId))
        {
            continue;
        }

        data.append(Acore::StringFormat("(%i, %i, %i),", player->GetSession()->GetAccountId(), companion.SpellId, companion.AllowableRace));

        uint32 factionSpecificSpellId = GetFactionSpecificCompanionId(companion.SpellId);
        if (factionSpecificSpellId > 0)
        {
            data.append(Acore::StringFormat("(%i, %i, %i),", player->GetSession()->GetAccountId(), factionSpecificSpellId, companion.AllowableRace == 690 ? 1101 : 690));
        }
    }

    if (data.length() > 0)
    {
        data.pop_back();
        data.append(";");

        LoginDatabase.Execute("REPLACE INTO `account_bound_companions` (`account_id`, `spell_id`, `allowable_race`) VALUES {}", data);
    }
}

void AccountBound::LearnCompanions(Player* player)
{
    QueryResult result = LoginDatabase.Query("SELECT `spell_id` FROM `account_bound_companions` WHERE `account_id`={} AND `allowable_race` & {}",
        player->GetSession()->GetAccountId(),
        player->getRaceMask());

    if (!result)
    {
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint32 spell_id = fields[0].Get<uint32>();

        if (!player->HasSpell(spell_id))
        {
            player->learnSpell(spell_id);
        }
    } while (result->NextRow());
}

uint32 AccountBound::GetFactionSpecificCompanionId(uint32 spell_id)
{
    for (auto& companion : factionSpecificCompanions)
    {
        if (companion.AllianceId != spell_id && companion.HordeId != spell_id)
        {
            continue;
        }

        if (companion.AllianceId == spell_id)
        {
            return companion.HordeId;
        }
        else if (companion.HordeId == spell_id)
        {
            return companion.AllianceId;
        }
    }

    return 0;
}
