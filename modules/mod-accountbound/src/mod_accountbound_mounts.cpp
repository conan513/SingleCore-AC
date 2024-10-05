#include "mod_accountbound.h"

void AccountBound::LoadMounts()
{
    QueryResult result = LoginDatabase.Query("SELECT `spell_id`, `allowable_race`, `allowable_class`, `required_level`, "
        "`required_skill`, `required_skill_rank` FROM `account_bound_mount_template`");

    if (!result)
    {
        LOG_INFO("server.loading", ">> Loaded 0 mount spells");
        return;
    }

    mounts.clear();

    int i = 0;
    do
    {
        mounts.push_back(Mounts());
        Field* fields = result->Fetch();
        mounts[i].SpellId = fields[0].Get<int32>();
        mounts[i].AllowableRace = fields[1].Get<int32>();
        mounts[i].AllowableClass = fields[2].Get<int32>();
        mounts[i].RequiredLevel = fields[3].Get<int32>();
        mounts[i].RequiredSkill = fields[4].Get<int32>();
        mounts[i].RequiredSkillRank = fields[5].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} mount templates", i);
}

void AccountBound::LoadFactionSpecificMounts()
{
    QueryResult result = LoginDatabase.Query("SELECT `alliance_id`, `horde_id` FROM `account_bound_factionchange_spells` pfs LEFT OUTER JOIN "
        "`account_bound_mount_template` abt ON pfs.`alliance_id` = abt.`spell_id` WHERE abt.`allowable_race` = 1101");

    if (!result)
    {
        LOG_INFO("server.loading", ">> Loaded 0 faction-specific mount templates");
        return;
    }

    factionSpecificMounts.clear();

    int i = 0;
    do
    {
        factionSpecificMounts.push_back(FactionSpecificMounts());
        Field* fields = result->Fetch();
        factionSpecificMounts[i].AllianceId = fields[0].Get<int32>();
        factionSpecificMounts[i].HordeId = fields[1].Get<int32>();

        i++;
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} faction-specific mount templates", i);
}

void AccountBound::SaveMounts(Player* player)
{
    std::string data = "";

    for (auto& mount : mounts)
    {
        if (!player->HasSpell(mount.SpellId))
        {
            continue;
        }

        data.append(Acore::StringFormat("(%i, %i, %i, %i, %i, %i, %i),", player->GetSession()->GetAccountId(), mount.SpellId, mount.AllowableRace, mount.AllowableClass, mount.RequiredLevel, mount.RequiredSkill, mount.RequiredSkillRank));

        uint32 factionSpecificSpellId = GetFactionSpecificMountId(mount.SpellId);
        if (factionSpecificSpellId > 0)
        {
            data.append(Acore::StringFormat("(%i, %i, %i, %i, %i, %i, %i),", player->GetSession()->GetAccountId(), factionSpecificSpellId, mount.AllowableRace == 690 ? 1101 : 690, mount.AllowableClass, mount.RequiredLevel, mount.RequiredSkill, mount.RequiredSkillRank));
        }
    }

    if (data.length() > 0)
    {
        data.pop_back();
        data.append(";");

        LoginDatabase.Execute("REPLACE INTO `account_bound_mounts` (`account_id`, `spell_id`, `allowable_race`, "
            "`allowable_class`, `required_level`, `required_skill`, `required_skill_rank`) VALUES {}", data);
    }
}

void AccountBound::LearnMounts(Player* player)
{
    QueryResult result = LoginDatabase.Query("SELECT `spell_id` FROM `account_bound_mounts` WHERE `account_id` = {} AND `allowable_race` & {} "
        "AND `allowable_class` & {} AND `required_level` <= {} AND (`required_skill` = 0 OR `required_skill_rank` <= {})",
        player->GetSession()->GetAccountId(),
        player->getRaceMask(),
        player->getClassMask(),
        player->GetLevel(),
        player->GetSkillValue(SKILL_RIDING));

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

uint32 AccountBound::GetFactionSpecificMountId(uint32 spell_id)
{
    for (auto& mount : factionSpecificMounts)
    {
        if (mount.AllianceId != spell_id && mount.HordeId != spell_id)
        {
            continue;
        }

        if (mount.AllianceId == spell_id)
        {
            return mount.HordeId;
        }
        else if (mount.HordeId == spell_id)
        {
            return mount.AllianceId;
        }
    }

    return 0;
}
