/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Config.h"
#include "Chat.h"
#include "GameTime.h"
#include "ItemTemplate.h"
#include "MapMgr.h"
#include "Pet.h"
#include "Player.h"
#include "PoolMgr.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "StringConvert.h"
#include "TaskScheduler.h"
#include "Tokenize.h"
#include "Unit.h"
#include "ZoneDifficulty.h"

class mod_zone_difficulty_unitscript : public UnitScript
{
public:
    mod_zone_difficulty_unitscript() : UnitScript("mod_zone_difficulty_unitscript") { }

    void OnAuraApply(Unit* target, Aura* aura) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }
        if (!sZoneDifficulty->MythicmodeInNormalDungeons && !target->GetMap()->IsRaidOrHeroicDungeon())
        {
            return;
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            bool nerfInDuel = sZoneDifficulty->ShouldNerfInDuels(target);

            //Check if the map of the target is subject of a nerf at all OR if the target is subject of a nerf in a duel
            if (sZoneDifficulty->NerfInfo.find(mapId) != sZoneDifficulty->NerfInfo.end() || nerfInDuel)
            {
                if (SpellInfo const* spellInfo = aura->GetSpellInfo())
                {
                    // Skip spells not affected by vulnerability (potions)
                    if (spellInfo->HasAttribute(SPELL_ATTR0_NO_IMMUNITIES))
                    {
                        return;
                    }

                    if (spellInfo->HasAura(SPELL_AURA_SCHOOL_ABSORB))
                    {
                        std::list<AuraEffect*> AuraEffectList = target->GetAuraEffectsByType(SPELL_AURA_SCHOOL_ABSORB);

                        for (AuraEffect* eff : AuraEffectList)
                        {
                            if ((eff->GetAuraType() != SPELL_AURA_SCHOOL_ABSORB) || (eff->GetSpellInfo()->Id != spellInfo->Id))
                            {
                                continue;
                            }

                            if (sZoneDifficulty->IsDebugInfoEnabled && target)
                            {
                                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                                {
                                    if (player->GetSession())
                                    {
                                        ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Base Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, eff->GetAmount());
                                    }
                                }
                            }

                            int32 absorb = eff->GetAmount();
                            uint32 phaseMask = target->GetPhaseMask();
                            int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
                            int8 mode = sZoneDifficulty->NerfInfo[mapId][matchingPhase].Enabled;
                            if (matchingPhase != -1)
                            {
                                Map* map = target->GetMap();
                                if (sZoneDifficulty->HasNormalMode(mode))
                                {
                                    absorb = eff->GetAmount() * sZoneDifficulty->NerfInfo[mapId][matchingPhase].AbsorbNerfPct;
                                }
                                if (sZoneDifficulty->HasMythicmode(mode) && sZoneDifficulty->MythicmodeInstanceData[target->GetMap()->GetInstanceId()])
                                {
                                    if (map->IsRaid() ||
                                        (map->IsHeroic() && map->IsDungeon()))
                                    {
                                        absorb = eff->GetAmount() * sZoneDifficulty->NerfInfo[mapId][matchingPhase].AbsorbNerfPctHard;
                                    }
                                }
                            }
                            else if (sZoneDifficulty->NerfInfo[DUEL_INDEX][0].Enabled > 0 && nerfInDuel)
                            {
                                absorb = eff->GetAmount() * sZoneDifficulty->NerfInfo[DUEL_INDEX][0].AbsorbNerfPct;
                            }

                            //This check must be last and override duel and map adjustments
                            if (sZoneDifficulty->SpellNerfOverrides.find(spellInfo->Id) != sZoneDifficulty->SpellNerfOverrides.end())
                            {
                                if (sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].find(mapId) != sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].end())
                                {
                                    // Check if the mode of instance and SpellNerfOverride match 
                                    if (sZoneDifficulty->OverrideModeMatches(target->GetMap()->GetInstanceId(), spellInfo->Id, mapId))
                                    {
                                        absorb = eff->GetAmount() * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id][mapId].NerfPct;
                                    }
                                }
                                else if (sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].find(0) != sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].end())
                                {
                                    if (sZoneDifficulty->OverrideModeMatches(target->GetMap()->GetInstanceId(), spellInfo->Id, mapId))
                                    {
                                        absorb = eff->GetAmount() * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id][0].NerfPct;
                                    }
                                }
                            }

                            eff->SetAmount(absorb);

                            if (sZoneDifficulty->IsDebugInfoEnabled && target)
                            {
                                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                                {
                                    if (player->GetSession())
                                    {
                                        ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Post Nerf Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, eff->GetAmount());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void ModifyHealReceived(Unit* target, Unit* /*healer*/, uint32& heal, SpellInfo const* spellInfo) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }
        if (!sZoneDifficulty->MythicmodeInNormalDungeons && !target->GetMap()->IsRaidOrHeroicDungeon())
        {
            return;
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            if (spellInfo)
            {
                if (spellInfo->HasEffect(SPELL_EFFECT_HEALTH_LEECH))
                {
                    return;
                }
                for (auto const& eff : spellInfo->GetEffects())
                {
                    if (eff.ApplyAuraName == SPELL_AURA_PERIODIC_LEECH)
                    {
                        return;
                    }
                }
                // Skip spells not affected by vulnerability (potions) and bandages
                if (spellInfo->HasAttribute(SPELL_ATTR0_NO_IMMUNITIES) || spellInfo->Mechanic == MECHANIC_BANDAGE)
                {
                    return;
                }
            }

            uint32 mapId = target->GetMapId();
            bool nerfInDuel = sZoneDifficulty->ShouldNerfInDuels(target);
            //Check if the map of the target is subject of a nerf at all OR if the target is subject of a nerf in a duel
            if (sZoneDifficulty->NerfInfo.find(mapId) != sZoneDifficulty->NerfInfo.end() || sZoneDifficulty->ShouldNerfInDuels(target))
            {
                //This check must be first and skip the rest to override everything else.
                if (spellInfo)
                {
                    if (sZoneDifficulty->SpellNerfOverrides.find(spellInfo->Id) != sZoneDifficulty->SpellNerfOverrides.end())
                    {
                        if (sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].find(mapId) != sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].end())
                        {
                            if (sZoneDifficulty->OverrideModeMatches(target->GetMap()->GetInstanceId(), spellInfo->Id, mapId))
                            {
                                heal = heal * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id][mapId].NerfPct;
                                return;
                            }
                        }
                        if (sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].find(0) != sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].end())
                        {
                            if (sZoneDifficulty->OverrideModeMatches(target->GetMap()->GetInstanceId(), spellInfo->Id, mapId))
                            {
                                heal = heal * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id][0].NerfPct;
                                return;
                            }
                        }
                    }
                }

                uint32 phaseMask = target->GetPhaseMask();
                int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
                int8 mode = sZoneDifficulty->NerfInfo[mapId][matchingPhase].Enabled;
                if (matchingPhase != -1)
                {
                    Map* map = target->GetMap();
                    if (sZoneDifficulty->HasNormalMode(mode))
                    {
                        heal = heal * sZoneDifficulty->NerfInfo[mapId][matchingPhase].HealingNerfPct;
                    }
                    if (sZoneDifficulty->HasMythicmode(mode) && sZoneDifficulty->MythicmodeInstanceData[map->GetInstanceId()])
                    {
                        if (map->IsRaid() ||
                            (map->IsHeroic() && map->IsDungeon()))
                        {
                            heal = heal * sZoneDifficulty->NerfInfo[mapId][matchingPhase].HealingNerfPctHard;
                        }
                    }
                }
                else if (sZoneDifficulty->NerfInfo[DUEL_INDEX][0].Enabled > 0 && nerfInDuel)
                {
                    heal = heal * sZoneDifficulty->NerfInfo[DUEL_INDEX][0].HealingNerfPct;
                }
            }
        }
    }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* spellInfo) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }
        if (!sZoneDifficulty->MythicmodeInNormalDungeons && !target->GetMap()->IsRaidOrHeroicDungeon())
        {
            return;
        }

        bool isDot = false;

        if (spellInfo)
        {
            for (auto const& eff : spellInfo->GetEffects())
            {
                if (eff.ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE || eff.ApplyAuraName == SPELL_AURA_PERIODIC_DAMAGE_PERCENT)
                {
                    isDot = true;
                }
            }
        }

        if (!isDot)
        {
            return;
        }

        // Disclaimer: also affects disables boss adds buff.
        if (sConfigMgr->GetOption<bool>("ModZoneDifficulty.SpellBuff.OnlyBosses", false))
        {
            if (attacker->ToCreature() && !attacker->ToCreature()->IsDungeonBoss())
            {
                return;
            }
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            uint32 phaseMask = target->GetPhaseMask();
            int32 matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);

            if (sZoneDifficulty->IsDebugInfoEnabled && attacker)
            {
                if (Player* player = attacker->ToPlayer())
                {
                    if (player->GetSession())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("A dot tick will be altered. Pre Nerf Value: %i", damage);
                    }
                }
            }

            if (sZoneDifficulty->NerfInfo.find(mapId) != sZoneDifficulty->NerfInfo.end() && matchingPhase != -1)
            {
                int8 mode = sZoneDifficulty->NerfInfo[mapId][matchingPhase].Enabled;
                Map* map = target->GetMap();
                if (sZoneDifficulty->HasNormalMode(mode))
                {
                    damage = damage * sZoneDifficulty->NerfInfo[mapId][matchingPhase].SpellDamageBuffPct;
                }
                if (sZoneDifficulty->HasMythicmode(mode) && sZoneDifficulty->MythicmodeInstanceData[map->GetInstanceId()])
                {
                    if (map->IsRaid() ||
                        (map->IsHeroic() && map->IsDungeon()))
                    {
                        damage = damage * sZoneDifficulty->NerfInfo[mapId][matchingPhase].SpellDamageBuffPctHard;
                    }
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                if (sZoneDifficulty->NerfInfo[DUEL_INDEX][0].Enabled > 0)
                {
                    damage = damage * sZoneDifficulty->NerfInfo[DUEL_INDEX][0].SpellDamageBuffPct;
                }
            }

            if (sZoneDifficulty->IsDebugInfoEnabled && attacker)
            {
                if (Player* player = attacker->ToPlayer())
                {
                    if (player->GetSession())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("A dot tick was altered. Post Nerf Value: %i", damage);
                    }
                }
            }
        }
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* spellInfo) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }
        if (!sZoneDifficulty->MythicmodeInNormalDungeons && !target->GetMap()->IsRaidOrHeroicDungeon())
        {
            return;
        }

        // Disclaimer: also affects disables boss adds buff.
        if (sConfigMgr->GetOption<bool>("ModZoneDifficulty.SpellBuff.OnlyBosses", false))
        {
            if (attacker->ToCreature() && !attacker->ToCreature()->IsDungeonBoss())
            {
                return;
            }
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            uint32 phaseMask = target->GetPhaseMask();
            int32 matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
            if (spellInfo)
            {
                //This check must be first and skip the rest to override everything else.
                if (sZoneDifficulty->SpellNerfOverrides.find(spellInfo->Id) != sZoneDifficulty->SpellNerfOverrides.end())
                {
                    if (sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].find(mapId) != sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].end())
                    {
                        if (sZoneDifficulty->OverrideModeMatches(target->GetMap()->GetInstanceId(), spellInfo->Id, mapId))
                        {
                            damage = damage * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id][mapId].NerfPct;
                            return;
                        }
                    }
                    else if (sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].find(0) != sZoneDifficulty->SpellNerfOverrides[spellInfo->Id].end())
                    {
                        if (sZoneDifficulty->OverrideModeMatches(target->GetMap()->GetInstanceId(), spellInfo->Id, mapId))
                        {
                            damage = damage * sZoneDifficulty->SpellNerfOverrides[spellInfo->Id][0].NerfPct;
                            return;
                        }
                    }
                }

                if (sZoneDifficulty->IsDebugInfoEnabled && target)
                {
                    if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                    {
                        if (player->GetSession())
                        {
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f Normal Mode)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->NerfInfo[mapId][matchingPhase].SpellDamageBuffPct);
                            ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Before Nerf Value: %i (%f Mythic Mode)", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage, sZoneDifficulty->NerfInfo[mapId][matchingPhase].SpellDamageBuffPctHard);
                        }
                    }
                }
            }

            if (sZoneDifficulty->NerfInfo.find(mapId) != sZoneDifficulty->NerfInfo.end() && matchingPhase != -1)
            {
                int8 mode = sZoneDifficulty->NerfInfo[mapId][matchingPhase].Enabled;
                Map* map = target->GetMap();
                if (sZoneDifficulty->HasNormalMode(mode))
                {
                    damage = damage * sZoneDifficulty->NerfInfo[mapId][matchingPhase].SpellDamageBuffPct;
                }
                if (sZoneDifficulty->HasMythicmode(mode) && sZoneDifficulty->MythicmodeInstanceData[map->GetInstanceId()])
                {
                    if (map->IsRaid() ||
                        (map->IsHeroic() && map->IsDungeon()))
                    {
                        damage = damage * sZoneDifficulty->NerfInfo[mapId][matchingPhase].SpellDamageBuffPctHard;
                    }
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                if (sZoneDifficulty->NerfInfo[DUEL_INDEX][0].Enabled > 0)
                {
                    damage = damage * sZoneDifficulty->NerfInfo[DUEL_INDEX][0].SpellDamageBuffPct;
                }
            }

            if (sZoneDifficulty->IsDebugInfoEnabled && target)
            {
                if (Player* player = target->ToPlayer()) // Pointless check? Perhaps.
                {
                    if (player->GetSession())
                    {
                        ChatHandler(player->GetSession()).PSendSysMessage("Spell: %s (%u) Post Nerf Value: %i", spellInfo->SpellName[player->GetSession()->GetSessionDbcLocale()], spellInfo->Id, damage);
                    }
                }
            }
        }
    }

    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override
    {
        if (!sZoneDifficulty->IsEnabled)
        {
            return;
        }
        if (!sZoneDifficulty->MythicmodeInNormalDungeons && !target->GetMap()->IsRaidOrHeroicDungeon())
        {
            return;
        }

        // Disclaimer: also affects disables boss adds buff.
        if (sConfigMgr->GetOption<bool>("ModZoneDifficulty.MeleeBuff.OnlyBosses", false))
        {
            if (attacker->ToCreature() && !attacker->ToCreature()->IsDungeonBoss())
            {
                return;
            }
        }

        if (sZoneDifficulty->IsValidNerfTarget(target))
        {
            uint32 mapId = target->GetMapId();
            uint32 phaseMask = target->GetPhaseMask();
            int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(mapId, phaseMask);
            if (sZoneDifficulty->NerfInfo.find(mapId) != sZoneDifficulty->NerfInfo.end() && matchingPhase != -1)
            {
                int8 mode = sZoneDifficulty->NerfInfo[mapId][matchingPhase].Enabled;
                Map* map = target->GetMap();
                if (sZoneDifficulty->HasNormalMode(mode))
                {
                    damage = damage * sZoneDifficulty->NerfInfo[mapId][matchingPhase].MeleeDamageBuffPct;
                }
                if (sZoneDifficulty->HasMythicmode(mode) && sZoneDifficulty->MythicmodeInstanceData[target->GetMap()->GetInstanceId()])
                {
                    if (map->IsRaid() ||
                        (map->IsHeroic() && map->IsDungeon()))
                    {
                        damage = damage * sZoneDifficulty->NerfInfo[mapId][matchingPhase].MeleeDamageBuffPctHard;
                    }
                }
            }
            else if (sZoneDifficulty->ShouldNerfInDuels(target))
            {
                if (sZoneDifficulty->NerfInfo[DUEL_INDEX][0].Enabled > 0)
                {
                    damage = damage * sZoneDifficulty->NerfInfo[DUEL_INDEX][0].MeleeDamageBuffPct;
                }
            }
        }
    }

    /**
     *  @brief Check if the Mythicmode is activated for the instance and if the creature has any Mythicmode AI assigned. Schedule the events, if so.
     */
    void OnUnitEnterCombat(Unit* unit, Unit* /*victim*/) override
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnUnitEnterCombat for unit {}", unit->GetEntry());
        if (sZoneDifficulty->MythicmodeInstanceData.find(unit->GetInstanceId()) == sZoneDifficulty->MythicmodeInstanceData.end())
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Instance is not in mythic mode.");
            return;
        }
        if (!sZoneDifficulty->MythicmodeInstanceData[unit->GetInstanceId()])
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: InstanceId not found in mythic mode list.");
            return;
        }

        if (Creature* creature = unit->ToCreature())
        {
            if (creature->IsTrigger())
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Creature is a trigger.");
                return;
            }
        }

        uint32 entry = unit->GetEntry();
        if (sZoneDifficulty->MythicmodeAI.find(entry) == sZoneDifficulty->MythicmodeAI.end())
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: No HarmodeAI found for creature with entry {}", entry);
            return;
        }

        unit->m_Events.CancelEventGroup(EVENT_GROUP);
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnUnitEnterCombat checks passed for unit {}", unit->GetEntry());

        uint32 i = 0;
        for (ZoneDifficultyHAI& data : sZoneDifficulty->MythicmodeAI[entry])
        {
            if (data.Chance == 100 || data.Chance >= urand(1, 100))
            {
                unit->m_Events.AddEventAtOffset([unit, entry, i]()
                    {
                        sZoneDifficulty->MythicmodeEvent(unit, entry, i);
                    }, data.Delay, EVENT_GROUP);
            }
            ++i;
        }
    }
};

class mod_zone_difficulty_playerscript : public PlayerScript
{
public:
    mod_zone_difficulty_playerscript() : PlayerScript("mod_zone_difficulty_playerscript") { }

    void OnMapChanged(Player* player) override
    {
        uint32 mapId = player->GetMapId();
        if (sZoneDifficulty->DisallowedBuffs.find(mapId) != sZoneDifficulty->DisallowedBuffs.end())
        {
            for (auto aura : sZoneDifficulty->DisallowedBuffs[mapId])
            {
                player->RemoveAura(aura);
            }
        }
    }
};

class mod_zone_difficulty_petscript : public PetScript
{
public:
    mod_zone_difficulty_petscript() : PetScript("mod_zone_difficulty_petscript") { }

    void OnPetAddToWorld(Pet* pet) override
    {
        uint32 mapId = pet->GetMapId();
        if (sZoneDifficulty->DisallowedBuffs.find(mapId) != sZoneDifficulty->DisallowedBuffs.end())
        {
            pet->m_Events.AddEventAtOffset([mapId, pet]()
                {
                    for (uint32 aura : sZoneDifficulty->DisallowedBuffs[mapId])
                    {
                        pet->RemoveAurasDueToSpell(aura);
                    }
                }, 2s);
        }
    }
};

class mod_zone_difficulty_worldscript : public WorldScript
{
public:
    mod_zone_difficulty_worldscript() : WorldScript("mod_zone_difficulty_worldscript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        sZoneDifficulty->IsEnabled = sConfigMgr->GetOption<bool>("ModZoneDifficulty.Enable", false);
        sZoneDifficulty->IsDebugInfoEnabled = sConfigMgr->GetOption<bool>("ModZoneDifficulty.DebugInfo", false);
        sZoneDifficulty->MythicmodeHpModifier = sConfigMgr->GetOption<float>("ModZoneDifficulty.Mythicmode.HpModifier", 2);
        sZoneDifficulty->MythicmodeEnable = sConfigMgr->GetOption<bool>("ModZoneDifficulty.Mythicmode.Enable", false);
        sZoneDifficulty->MythicmodeInNormalDungeons = sConfigMgr->GetOption<bool>("ModZoneDifficulty.Mythicmode.InNormalDungeons", false);
        sZoneDifficulty->LoadMapDifficultySettings();
    }

    void OnStartup() override
    {
        sZoneDifficulty->LoadMythicmodeInstanceData();
        sZoneDifficulty->LoadMythicmodeScoreData();
    }
};

class mod_zone_difficulty_globalscript : public GlobalScript
{
public:
    mod_zone_difficulty_globalscript() : GlobalScript("mod_zone_difficulty_globalscript") { }

    void OnBeforeSetBossState(uint32 id, EncounterState newState, EncounterState oldState, Map* instance) override
    {
        if (!sZoneDifficulty->MythicmodeEnable)
        {
            return;
        }
        if (sZoneDifficulty->IsDebugInfoEnabled)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnBeforeSetBossState: bossId = {}, newState = {}, oldState = {}, MapId = {}, InstanceId = {}", id, newState, oldState, instance->GetId(), instance->GetInstanceId());
        }
        uint32 instanceId = instance->GetInstanceId();
        if (!sZoneDifficulty->IsMythicmodeMap(instance->GetId()) ||
            (!sZoneDifficulty->MythicmodeInNormalDungeons && !instance->IsRaidOrHeroicDungeon()))
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnBeforeSetBossState: Instance not handled because there is no Mythicmode loot data for map id: {}", instance->GetId());
            return;
        }
        if (oldState != IN_PROGRESS && newState == IN_PROGRESS)
        {
            if (sZoneDifficulty->MythicmodeInstanceData[instanceId])
            {
                sZoneDifficulty->EncountersInProgress[instanceId] = GameTime::GetGameTime().count();
            }
        }
        else if (oldState == IN_PROGRESS && newState == DONE)
        {
            if (sZoneDifficulty->MythicmodeInstanceData[instanceId])
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Mythicmode is on.");
                if (sZoneDifficulty->EncountersInProgress.find(instanceId) != sZoneDifficulty->EncountersInProgress.end() && sZoneDifficulty->EncountersInProgress[instanceId] != 0)
                {
                    Map::PlayerList const& PlayerList = instance->GetPlayers();
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        Player* player = i->GetSource();
                        if (!player->IsGameMaster() && !player->IsDeveloper())
                        {
                            CharacterDatabase.Execute(
                                "REPLACE INTO `zone_difficulty_encounter_logs` VALUES({}, {}, {}, {}, {}, {}, {})",
                                instanceId, sZoneDifficulty->EncountersInProgress[instanceId], GameTime::GetGameTime().count(), instance->GetId(), id, player->GetGUID().GetCounter(), 64);
                        }
                    }
                }
            }
        }
    }

    void OnInstanceIdRemoved(uint32 instanceId) override
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnInstanceIdRemoved: instanceId = {}", instanceId);
        if (sZoneDifficulty->MythicmodeInstanceData.find(instanceId) != sZoneDifficulty->MythicmodeInstanceData.end())
        {
            sZoneDifficulty->MythicmodeInstanceData.erase(instanceId);
        }

        CharacterDatabase.Execute("DELETE FROM zone_difficulty_instance_saves WHERE InstanceID = {};", instanceId);
    }

    void OnAfterUpdateEncounterState(Map* map, EncounterCreditType /*type*/, uint32 /*creditEntry*/, Unit* source, Difficulty /*difficulty_fixed*/, DungeonEncounterList const* /*encounters*/, uint32 /*dungeonCompleted*/, bool /*updated*/) override
    {
        if (!sZoneDifficulty->MythicmodeEnable)
        {
            return;
        }
        if (!source)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: source is a nullptr in OnAfterUpdateEncounterState");
            return;
        }

        if (sZoneDifficulty->MythicmodeInstanceData.find(map->GetInstanceId()) != sZoneDifficulty->MythicmodeInstanceData.end())
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Encounter completed. Map relevant. Checking for source: {}", source->GetEntry());
            // Give additional loot, if the encounter was in Mythicmode.
            if (sZoneDifficulty->MythicmodeInstanceData[map->GetInstanceId()])
            {
                uint32 mapId = map->GetId();
                uint32 score = 0;
                if (!sZoneDifficulty->IsMythicmodeMap(mapId) ||
                    (!sZoneDifficulty->MythicmodeInNormalDungeons && !map->IsRaidOrHeroicDungeon()))
                {
                    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: No additional loot stored in map with id {}.", map->GetInstanceId());
                    return;
                }

                bool SourceAwardsMythicmodeLoot = false;
                //iterate over all listed creature entries for that map id and see, if the encounter should yield Mythicmode loot and if there is an override to the default behaviour
                for (auto value : sZoneDifficulty->MythicmodeLoot[mapId])
                {
                    if (value.EncounterEntry == source->GetEntry())
                    {
                        SourceAwardsMythicmodeLoot = true;
                        if (!(value.Override & 1))
                        {
                            score = 1;
                        }
                        break;
                    }
                }

                if (!SourceAwardsMythicmodeLoot)
                {
                    return;
                }

                if (map->IsHeroic() && map->IsNonRaidDungeon())
                {
                    sZoneDifficulty->AddMythicmodeScore(map, sZoneDifficulty->Expansion[mapId], score);
                }
                else if (map->IsRaid())
                {
                    sZoneDifficulty->AddMythicmodeScore(map, sZoneDifficulty->Expansion[mapId], score);
                }
                /* debug
                 * else
                 * {
                 *   LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Map with id {} is not a raid or a dungeon. Mythicmode loot not granted.", map->GetInstanceId());
                 * }
                 */
            }
        }
    }
};

class mod_zone_difficulty_rewardnpc : public CreatureScript
{
public:
    mod_zone_difficulty_rewardnpc() : CreatureScript("mod_zone_difficulty_rewardnpc") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        if (sZoneDifficulty->IsDebugInfoEnabled)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnGossipSelectRewardNpc action: {}", action);
        }
        ClearGossipMenuFor(player);
        uint32 npcText = 0;
        //Safety measure. There's a way for action 0 to happen even though it's not provided in the gossip menu.
        if (action == 0)
        {
            CloseGossipMenuFor(player);
            return true;
        }

        if (action == 999998)
        {
            CloseGossipMenuFor(player);
            return true;
        }

        if (action == 999999)
        {
            npcText = NPC_TEXT_SCORE;
            for (int i = 1; i <= 16; ++i)
            {
                std::string whisper;
                whisper.append("Your score is ");
                if (sZoneDifficulty->MythicmodeScore.find(player->GetGUID().GetCounter()) == sZoneDifficulty->MythicmodeScore.end())
                {
                    continue;
                }
                else if (sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].find(i) == sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].end())
                {
                    continue;
                }
                else
                {
                    whisper.append(std::to_string(sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()][i])).append(" ");
                }
                whisper.append(sZoneDifficulty->GetContentTypeString(i));
                creature->Whisper(whisper, LANG_UNIVERSAL, player);
            }
            return true;
        }

        // full tier clearance rewards: confirmation
        if (action > 99001000)
        {

            uint32 category = action - 99001000;

            // Check (again) if the player has enough score in the respective category.
            uint32 availableScore = 0;
            if (sZoneDifficulty->MythicmodeScore.find(player->GetGUID().GetCounter()) != sZoneDifficulty->MythicmodeScore.end())
            {
                if (sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].find(category) != sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].end())
                {
                    availableScore = sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()][category];
                }
            }
            if (availableScore < sZoneDifficulty->TierRewards[category].Price)
            {
                CloseGossipMenuFor(player);
                return true;
            }

            // Check (again) if the player has the neccesary achievement
            if (!sZoneDifficulty->HasCompletedFullTier(category, player->GetGUID().GetCounter()))
            {
                CloseGossipMenuFor(player);
                return true;
            }

            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Sending full tier clearance reward for category {}", category);
            sZoneDifficulty->DeductMythicmodeScore(player, category, sZoneDifficulty->TierRewards[category].Price);
            sZoneDifficulty->SendItem(player, category, 99, 0);

            return true;
        }

        // full tier clearance rewards: selection
        if (action > 99000000)
        {
            uint32 category = action - 99000000;
            if (sZoneDifficulty->HasCompletedFullTier(category, player->GetGUID().GetCounter()))
            {
                // Check if the player has enough score in the respective category.
                uint32 availableScore = 0;
                if (sZoneDifficulty->MythicmodeScore.find(player->GetGUID().GetCounter()) != sZoneDifficulty->MythicmodeScore.end())
                {
                    if (sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].find(category) != sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].end())
                    {
                        availableScore = sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()][category];
                    }
                }

                if (availableScore < sZoneDifficulty->TierRewards[category].Price)
                {
                    npcText = NPC_TEXT_DENIED;
                    SendGossipMenuFor(player, npcText, creature);
                    std::string whisper;
                    whisper.append("I am sorry, time-traveler. This reward costs ");
                    whisper.append(std::to_string(sZoneDifficulty->TierRewards[category].Price));
                    whisper.append(" score but you only have ");
                    whisper.append(std::to_string(sZoneDifficulty->MythicmodeScore[category][player->GetGUID().GetCounter()]));
                    whisper.append(" ");
                    whisper.append(sZoneDifficulty->GetContentTypeString(category));
                    creature->Whisper(whisper, LANG_UNIVERSAL, player);
                    return true;
                }
                npcText = NPC_TEXT_CONFIRM;
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(sZoneDifficulty->TierRewards[category].Entry);
                std::string gossip;
                std::string name = proto->Name1;
                if (ItemLocale const* leftIl = sObjectMgr->GetItemLocale(sZoneDifficulty->TierRewards[category].Entry))
                {
                    ObjectMgr::GetLocaleString(leftIl->Name, player->GetSession()->GetSessionDbcLocale(), name);
                }
                gossip.append("Yes, ").append(name).append(" is the item i want.");
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "No!", GOSSIP_SENDER_MAIN, 999998);
                AddGossipItemFor(player, GOSSIP_ICON_VENDOR, gossip, GOSSIP_SENDER_MAIN, 99001000 + category);
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: AddingGossipItem with action {}", 99001000 + category);
                SendGossipMenuFor(player, npcText, creature);
                return true;
            }
            return true;
        }

        // player has selected a content type
        else if (action < 100)
        {
            npcText = NPC_TEXT_CATEGORY;
            if (sZoneDifficulty->HasCompletedFullTier(action, player->GetGUID().GetCounter()))
            {
                std::string gossip = "I want to redeem the ultimate Mythicmode reward ";
                gossip.append(sZoneDifficulty->GetContentTypeString(action));
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, gossip, GOSSIP_SENDER_MAIN, 99000000 + action);
            }

            uint32 i = 1;
            for (auto& itemType : sZoneDifficulty->Rewards[action])
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: typedata.first is {}", itemType.first);
                std::string gossip;
                std::string typestring = sZoneDifficulty->GetItemTypeString(itemType.first);
                if (sZoneDifficulty->ItemIcons.find(i) != sZoneDifficulty->ItemIcons.end())
                {
                    gossip.append(sZoneDifficulty->ItemIcons[i]);
                }
                gossip.append("I am interested in items from the ").append(typestring).append(" category.");
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, gossip, GOSSIP_SENDER_MAIN, itemType.first + (action * 100));
                ++i;
            }
        }
        else if (action < 1000)
        {
            npcText = NPC_TEXT_ITEM;
            uint32 category = 0;
            uint32 counter = action;
            while (counter > 99)
            {
                ++category;
                counter = counter - 100;
            }
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Building gossip with category {} and counter {}", category, counter);

            for (size_t i = 0; i < sZoneDifficulty->Rewards[category][counter].size(); ++i)
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Adding gossip option for entry {}", sZoneDifficulty->Rewards[category][counter][i].Entry);
                ItemTemplate const* proto = sObjectMgr->GetItemTemplate(sZoneDifficulty->Rewards[category][counter][i].Entry);
                std::string name = proto->Name1;
                if (ItemLocale const* leftIl = sObjectMgr->GetItemLocale(sZoneDifficulty->Rewards[category][counter][i].Entry))
                {
                    ObjectMgr::GetLocaleString(leftIl->Name, player->GetSession()->GetSessionDbcLocale(), name);
                }

                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, name, GOSSIP_SENDER_MAIN, (1000 * category) + (100 * counter) + i);
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: AddingGossipItem with action {}", (1000 * category) + (100 * counter) + i);
            }
        }
        else if (action < 100000)
        {
            uint32 category = 0;
            uint32 itemType = 0;
            uint32 counter = action;
            while (counter > 999)
            {
                ++category;
                counter = counter - 1000;
            }
            while (counter > 99)
            {
                ++itemType;
                counter = counter - 100;
            }
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Handling item with category {}, itemType {}, counter {}", category, itemType, counter);

            // Check if the player has enough score in the respective category.
            uint32 availableScore = 0;
            if (sZoneDifficulty->MythicmodeScore.find(player->GetGUID().GetCounter()) != sZoneDifficulty->MythicmodeScore.end())
            {
                if (sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].find(category) != sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].end())
                {
                    availableScore = sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()][category];
                }
            }

            if (availableScore < sZoneDifficulty->Rewards[category][itemType][counter].Price)
            {
                npcText = NPC_TEXT_DENIED;
                SendGossipMenuFor(player, npcText, creature);
                std::string whisper;
                whisper.append("I am sorry, time-traveler. This item costs ");
                whisper.append(std::to_string(sZoneDifficulty->Rewards[category][itemType][counter].Price));
                whisper.append(" score but you only have ");
                whisper.append(std::to_string(sZoneDifficulty->MythicmodeScore[category][player->GetGUID().GetCounter()]));
                whisper.append(" ");
                whisper.append(sZoneDifficulty->GetContentTypeString(category));
                creature->Whisper(whisper, LANG_UNIVERSAL, player);
                return true;
            }

            npcText = NPC_TEXT_CONFIRM;
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(sZoneDifficulty->Rewards[category][itemType][counter].Entry);
            std::string gossip;
            std::string name = proto->Name1;
            if (ItemLocale const* leftIl = sObjectMgr->GetItemLocale(sZoneDifficulty->Rewards[category][itemType][counter].Entry))
            {
                ObjectMgr::GetLocaleString(leftIl->Name, player->GetSession()->GetSessionDbcLocale(), name);
            }
            gossip.append("Yes, ").append(name).append(" is the item i want.");
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "No!", GOSSIP_SENDER_MAIN, 999998);
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, gossip, GOSSIP_SENDER_MAIN, 100000 + (1000 * category) + (100 * itemType) + counter);
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: AddingGossipItem with action {}", 100000 + (1000 * category) + (100 * itemType) + counter);
        }
        else if (action > 100000)
        {
            npcText = NPC_TEXT_GRANT;
            uint32 category = 0;
            uint32 itemType = 0;
            uint32 counter = action;
            counter = counter - 100000;
            while (counter > 999)
            {
                ++category;
                counter = counter - 1000;
            }
            while (counter > 99)
            {
                ++itemType;
                counter = counter - 100;
            }

            // Check (again) if the player has enough score in the respective category.
            uint32 availableScore = 0;
            if (sZoneDifficulty->MythicmodeScore.find(player->GetGUID().GetCounter()) != sZoneDifficulty->MythicmodeScore.end())
            {
                if (sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].find(category) != sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()].end())
                {
                    availableScore = sZoneDifficulty->MythicmodeScore[player->GetGUID().GetCounter()][category];
                }
            }
            if (availableScore < sZoneDifficulty->Rewards[category][itemType][counter].Price)
            {
                return true;
            }

            // Check if the player has the neccesary achievement
            if (sZoneDifficulty->Rewards[category][itemType][counter].Achievement != 0)
            {
                if (!player->HasAchieved(sZoneDifficulty->Rewards[category][itemType][counter].Achievement))
                {
                    std::string gossip = "You do not have the required achievement with ID ";
                    gossip.append(std::to_string(sZoneDifficulty->Rewards[category][itemType][counter].Achievement));
                    gossip.append(" to receive this item. Before i can give it to you, you need to complete the whole dungeon where it can be obtained.");
                    creature->Whisper(gossip, LANG_UNIVERSAL, player);
                    /* debug
                     * LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Player missing achiement with ID {} to obtain item with category {}, itemType {}, counter {}",
                     *    sZoneDifficulty->Rewards[category][itemType][counter].Achievement, category, itemType, counter);
                     * end debug
                     */
                    CloseGossipMenuFor(player);
                    return true;
                }
            }

            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Sending item with category {}, itemType {}, counter {}", category, itemType, counter);
            sZoneDifficulty->DeductMythicmodeScore(player, category, sZoneDifficulty->Rewards[category][itemType][counter].Price);
            sZoneDifficulty->SendItem(player, category, itemType, counter);
        }

        SendGossipMenuFor(player, npcText, creature);
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnGossipHelloRewardNpc");
        uint32 npcText = NPC_TEXT_OFFER;
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface\\icons\\inv_misc_questionmark:15|t Can you please remind me of my score?", GOSSIP_SENDER_MAIN, 999999);

        for (auto& typedata : sZoneDifficulty->Rewards)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: typedata.first is {}", typedata.first);
            if (typedata.first != 0)
            {
                std::string gossip;
                std::string typestring = sZoneDifficulty->GetContentTypeString(typedata.first);
                gossip.append("I want to redeem rewards ").append(typestring);
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: typestring is: {} gossip is: {}", typestring, gossip);
                // typedata.first is the ContentType
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, gossip, GOSSIP_SENDER_MAIN, typedata.first);
            }
        }

        SendGossipMenuFor(player, npcText, creature);
        return true;
    }
};

class mod_zone_difficulty_dungeonmaster : public CreatureScript
{
public:
    mod_zone_difficulty_dungeonmaster() : CreatureScript("mod_zone_difficulty_dungeonmaster") { }

    struct mod_zone_difficulty_dungeonmasterAI : public ScriptedAI
    {
        mod_zone_difficulty_dungeonmasterAI(Creature* creature) : ScriptedAI(creature) { }

        void Reset() override
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: mod_zone_difficulty_dungeonmasterAI: Reset happens.");
            if (me->GetMap() && me->GetMap()->IsHeroic() && !me->GetMap()->IsRaid())
            {
                if (!sZoneDifficulty->MythicmodeEnable)
                {
                    return;
                }
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: We're inside a heroic 5man now.");
                //todo: add the list for the wotlk heroic dungeons quests
                for (auto& quest : sZoneDifficulty->DailyHeroicQuests)
                {
                    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Checking quest {} and MapId {}", quest, me->GetMapId());
                    if (sPoolMgr->IsSpawnedObject<Quest>(quest))
                    {
                        if (sZoneDifficulty->HeroicTBCQuestMapList[me->GetMapId()] == quest)
                        {
                            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: mod_zone_difficulty_dungeonmasterAI: Quest with id {} is active.", quest);
                            me->SetPhaseMask(1, true);

                            _scheduler.Schedule(15s, [this](TaskContext /*context*/)
                                {
                                    me->Yell("If you want a challenge, please talk to me soon adventurer!", LANG_UNIVERSAL);
                                });
                            _scheduler.Schedule(45s, [this](TaskContext /*context*/)
                                {
                                    me->Yell("I will take my leave then and offer my services to other adventurers. See you again!", LANG_UNIVERSAL);
                                });
                            _scheduler.Schedule(60s, [this](TaskContext /*context*/)
                                {
                                    me->DespawnOrUnsummon();
                                });
                            break;
                        }
                    }
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _scheduler.Update(diff);
        }

    private:
        TaskScheduler _scheduler;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new mod_zone_difficulty_dungeonmasterAI(creature);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        uint32 instanceId = player->GetMap()->GetInstanceId();
        if (action == 100)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Try turn on");
            bool canTurnOn = true;

            // Forbid turning Mythicmode on ...
            // ...if a single encounter was completed on normal mode
            if (sZoneDifficulty->MythicmodeInstanceData.find(instanceId) != sZoneDifficulty->MythicmodeInstanceData.end())
            {
                if (player->GetInstanceScript()->GetBossState(0) == DONE)
                {
                    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Mythicmode is not Possible for instanceId {}", instanceId);
                    canTurnOn = false;
                    creature->Whisper("I am sorry, time-traveler. You can not return to this version of the time-line anymore. You have already completed one of the lessons.", LANG_UNIVERSAL, player);
                    sZoneDifficulty->SaveMythicmodeInstanceData(instanceId);
                }
            }
            // ... if there is an encounter in progress
            if (player->GetInstanceScript()->IsEncounterInProgress())
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: IsEncounterInProgress");
                canTurnOn = false;
                creature->Whisper("I am sorry, time-traveler. You can not return to this version of the time-line currently. There is already a battle in progress.", LANG_UNIVERSAL, player);
            }

            if (player->IsGameMaster())
            {
                LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: GM {} has allowed Mythicmode for instance {}", player->GetName(), instanceId);
                canTurnOn = true;
            }

            if (canTurnOn)
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Turn on Mythicmode for id {}", instanceId);
                sZoneDifficulty->MythicmodeInstanceData[instanceId] = true;
                sZoneDifficulty->SaveMythicmodeInstanceData(instanceId);
                sZoneDifficulty->SendWhisperToRaid("We're switching to the challenging version of the history lesson now. (Mythic Mode)", creature, player);
            }

            CloseGossipMenuFor(player);
        }
        else if (action == 101)
        {
            if (player->GetInstanceScript()->IsEncounterInProgress())
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: IsEncounterInProgress");
                creature->Whisper("I am sorry, time-traveler. You can not change the version of the time-line currently. There is already a battle in progress.", LANG_UNIVERSAL, player);
                CloseGossipMenuFor(player);
            }
            if (player->GetInstanceScript()->GetBossState(0) != DONE)
            {
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Turn off Mythicmode for id {}", instanceId);
                sZoneDifficulty->MythicmodeInstanceData[instanceId] = false;
                sZoneDifficulty->SaveMythicmodeInstanceData(instanceId);
                sZoneDifficulty->SendWhisperToRaid("We're switching to the cinematic version of the history lesson now. (Normal mode)", creature, player);
                CloseGossipMenuFor(player);
            }
            else
            {
                ClearGossipMenuFor(player);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Yes, i am sure. I know we can not go back to the harder version anymore, but we still want to stick with the less challenging route.", GOSSIP_SENDER_MAIN, 102);
                SendGossipMenuFor(player, NPC_TEXT_LEADER_FINAL, creature);
            }
        }
        else if (action == 102)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Turn off Mythicmode for id {}", instanceId);
            sZoneDifficulty->MythicmodeInstanceData[instanceId] = false;
            sZoneDifficulty->SaveMythicmodeInstanceData(instanceId);
            sZoneDifficulty->SendWhisperToRaid("We're switching to the cinematic version of the history lesson now. (Normal mode)", creature, player);
            CloseGossipMenuFor(player);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnGossipHelloChromie");
        Group* group = player->GetGroup();
        if (group && group->IsLfgRandomInstance() && !player->GetMap()->IsRaid())
        {
            creature->Whisper("I am sorry, time-traveler. You can not accept challenges here. You need to choose a specific dungeon in order to play my history lessons.", LANG_UNIVERSAL, player);
            return true;
        }
        if (!group && !player->IsGameMaster())
        {
            creature->Whisper("I am sorry, time-traveler. You can not play my history lessons on your own. Bring some friends?", LANG_UNIVERSAL, player);
            return true;
        }
        uint32 npcText = NPC_TEXT_OTHER;
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnGossipHello Has Group");
        if (player->IsGameMaster() || (group && group->IsLeader(player->GetGUID())))
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: OnGossipHello Is Leader");
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Please Chromie, let us re-experience how all the things really happened back then. (Mythic Mode)", GOSSIP_SENDER_MAIN, 100);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I think we will be fine with the cinematic version from here. (Normal mode)", GOSSIP_SENDER_MAIN, 101);

            if (sZoneDifficulty->MythicmodeInstanceData[player->GetMap()->GetInstanceId()])
            {
                npcText = NPC_TEXT_LEADER_HARD;
            }
            else
            {
                npcText = NPC_TEXT_LEADER_NORMAL;
            }
        }
        else
        {
            creature->Whisper("I will let the leader of your group decide about this subject. You will receive a notification, when they make a new request to me.", LANG_UNIVERSAL, player);
        }
        SendGossipMenuFor(player, npcText, creature);
        return true;
    }
};

class mod_zone_difficulty_allcreaturescript : public AllCreatureScript
{
public:
    mod_zone_difficulty_allcreaturescript() : AllCreatureScript("mod_zone_difficulty_allcreaturescript") { }

    void OnAllCreatureUpdate(Creature* creature, uint32 /*diff*/) override
    {
        if (!sZoneDifficulty->MythicmodeEnable)
        {
            return;
        }
        // Heavily inspired by https://github.com/azerothcore/mod-autobalance/blob/1d82080237e62376b9a030502264c90b5b8f272b/src/AutoBalance.cpp
        Map* map = creature->GetMap();
        if (!creature || !map)
        {
            return;
        }

        if (!map->IsRaid() &&
            (!(map->IsHeroic() && map->IsDungeon())))
        {
            return;
        }

        uint32 mapId = creature->GetMapId();
        if (sZoneDifficulty->NerfInfo.find(mapId) == sZoneDifficulty->NerfInfo.end())
        {
            return;
        }

        if ((creature->IsHunterPet() || creature->IsPet() || creature->IsSummon()) && creature->IsControlledByPlayer())
        {
            return;
        }

        if (!creature->IsAlive())
        {
            return;
        }

        CreatureTemplate const* creatureTemplate = creature->GetCreatureTemplate();
        //skip critters and special creatures (spell summons etc.) in instances
        if (creatureTemplate->maxlevel <= 1)
        {
            return;
        }

        CreatureBaseStats const* origCreatureStats = sObjectMgr->GetCreatureBaseStats(creature->GetLevel(), creatureTemplate->unit_class);
        uint32 baseHealth = origCreatureStats->GenerateHealth(creatureTemplate);
        uint32 newHp;
        uint32 entry = creature->GetEntry();
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Checking hp for creature with entry {}.", entry);
        if (sZoneDifficulty->CreatureOverrides.find(entry) == sZoneDifficulty->CreatureOverrides.end())
        {
            if (creature->IsDungeonBoss())
            {
                return;
            }
            newHp = round(baseHealth * sZoneDifficulty->MythicmodeHpModifier);
        }
        else
        {
            newHp = round(baseHealth * sZoneDifficulty->CreatureOverrides[entry]);
        }

        uint32 phaseMask = creature->GetPhaseMask();
        int matchingPhase = sZoneDifficulty->GetLowestMatchingPhase(creature->GetMapId(), phaseMask);
        int8 mode = sZoneDifficulty->NerfInfo[mapId][matchingPhase].Enabled;
        if (matchingPhase != -1)
        {
            if (sZoneDifficulty->HasMythicmode(mode) && sZoneDifficulty->MythicmodeInstanceData[creature->GetMap()->GetInstanceId()])
            {
                if (creature->GetMaxHealth() == newHp)
                {
                    return;
                }
                //if (sZoneDifficulty->IsDebugInfoEnabled)
                //{
                    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Modify creature hp for Mythic Mode: {} to {}", baseHealth, newHp);
                //}
                bool hpIsFull = false;
                if (creature->GetHealthPct() >= 100)
                {
                    hpIsFull = true;
                }
                creature->SetMaxHealth(newHp);
                creature->SetCreateHealth(newHp);
                creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)newHp);
                if (hpIsFull)
                {
                    creature->SetHealth(newHp);
                }
                creature->UpdateAllStats();
                return;
            }

            if (sZoneDifficulty->MythicmodeInstanceData[creature->GetMap()->GetInstanceId()] == false)
            {
                if (creature->GetMaxHealth() == newHp)
                {
                    //if (sZoneDifficulty->IsDebugInfoEnabled)
                    //{
                    //    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Modify creature hp for normal mode: {} to {}", baseHealth, baseHealth);
                    //}
                    bool hpIsFull = false;
                    if (creature->GetHealthPct() >= 100)
                    {
                        hpIsFull = true;
                    }
                    creature->SetMaxHealth(baseHealth);
                    creature->SetCreateHealth(baseHealth);
                    creature->SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, (float)baseHealth);
                    if (hpIsFull)
                    {
                        creature->SetHealth(baseHealth);
                    }
                    creature->UpdateAllStats();
                    return;
                }
            }
        }
    }
};

// Add all scripts in one
void AddModZoneDifficultyScripts()
{
    new mod_zone_difficulty_unitscript();
    new mod_zone_difficulty_playerscript();
    new mod_zone_difficulty_petscript();
    new mod_zone_difficulty_worldscript();
    new mod_zone_difficulty_globalscript();
    new mod_zone_difficulty_rewardnpc();
    new mod_zone_difficulty_dungeonmaster();
    new mod_zone_difficulty_allcreaturescript();
}
