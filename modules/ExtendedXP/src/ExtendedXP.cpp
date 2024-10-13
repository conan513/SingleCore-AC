#include "ExtendedXP.h"

void ExtendedXPPlayer::OnGiveXP(Player* player, uint32& amount, Unit* victim, uint8 /*xpSource*/)
{
    if (!sConfigMgr->GetOption<bool>("ExtendedXP.Enable", false))
    {
        return;
    }

    if (!player || !victim ||
        victim->IsPlayer() || !victim->ToCreature())
    {
        return;
    }

    if (sConfigMgr->GetOption<bool>("ExtendedXP.RareXP.Enable", false))
    {
        auto creature = victim->ToCreature();
        auto creatureProto = creature->GetCreatureTemplate();

        if (creatureProto->rank != CREATURE_ELITE_RARE && creatureProto->rank != CREATURE_ELITE_RAREELITE)
        {
            return;
        }

        float rareExpMulti = sConfigMgr->GetOption<float>("ExtendedXP.RareXP.Multiplier", 3);
        amount = amount * rareExpMulti;
    }

    if (sConfigMgr->GetOption<bool>("ExtendedXP.GroupXP.Enable", false))
    {
        auto group = player->GetGroup();
        if (!group)
        {
            return;
        }

        uint32 playersInRange = 0;
        for (auto it = group->GetFirstMember(); it != nullptr; it = it->next())
        {
            auto member = it->GetSource();
            if (!member)
            {
                continue;
            }

            // Don't count self
            if (member->GetGUID() == player->GetGUID())
            {
                continue;
            }

            if (!member->IsInMap(player) ||
                !member->IsWithinDist(player, member->GetSightRange(player), false))
            {
                continue;
            }

            playersInRange++;
        }

        float groupExpMulti = sConfigMgr->GetOption<float>("ExtendedXP.GroupXP.Multiplier", 0.33);
        uint32 groupExpCap = sConfigMgr->GetOption<uint32>("ExtendedXP.GroupXP.Cap", 5);

        if (playersInRange)
        {
            if (playersInRange > groupExpCap)
            {
                playersInRange = groupExpCap;
            }

            auto bonusXP = (amount * groupExpMulti) * playersInRange;
            player->GiveXP(bonusXP, victim);
        }
    }
}

void ExtendedXPPlayer::OnAchiComplete(Player* player, AchievementEntry const* achievement)
{
    if (!sConfigMgr->GetOption<bool>("ExtendedXP.Enable", false))
    {
        return;
    }

    if (!sConfigMgr->GetOption<bool>("ExtendedXP.AchievementXP.Enable", false))
    {
        return;
    }

    if (!player)
    {
        return;
    }

    if (!achievement)
    {
        return;
    }

    auto pLevel = player->GetLevel();
    if (pLevel >= sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
    {
        return;
    }

    float expPercent = sConfigMgr->GetOption<float>("ExtendedXP.AchievementXP.Percent", 1.5f);
    float expMultiplier = (expPercent * achievement->points) / 100;

    if (sConfigMgr->GetOption<bool>("ExtendedXP.AchievementXP.ScaleLevel", false))
    {
        expMultiplier = ((expMultiplier * 100.0f) * (1.0f - (pLevel / 100.0f))) / 100.0f;
    }

    float xpMax = player->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
    float xpReward = xpMax * expMultiplier;

    player->GiveXP(xpReward, nullptr);
}

void SC_AddExtendedXPScripts()
{
    new ExtendedXPPlayer();
}
