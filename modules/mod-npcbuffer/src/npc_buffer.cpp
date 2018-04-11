/*

# Buffer NPC #

#### A module for AzerothCore by [StygianTheBest](https://github.com/StygianTheBest/AzerothCore-Content/tree/master/Modules)
------------------------------------------------------------------------------------------------------------------


### Description ###
------------------------------------------------------------------------------------------------------------------
Creates a one-click Buff NPC with emotes.


### Features ###
------------------------------------------------------------------------------------------------------------------
- Buffs the player with no dialogue interaction
- Cures ressurection sickness


### Data ###
------------------------------------------------------------------------------------------------------------------
- Type: NPC
- Script: buff_npc
- Config: Yes
    - Enable Module Announce
    - Enable Cure Ressurection Sickness
    - Set Spell ID(s) for Buffs
- SQL: Yes
    - NPC ID: 601016


### Version ###
------------------------------------------------------------------------------------------------------------------
- v2017.08.05 - Release
- v2017.08.06 - Removed dialogue options (Just buffs player on click)


### Credits ###
------------------------------------------------------------------------------------------------------------------
- [Blizzard Entertainment](http://blizzard.com)
- [TrinityCore](https://github.com/TrinityCore/TrinityCore/blob/3.3.5/THANKS)
- [SunwellCore](http://www.azerothcore.org/pages/sunwell.pl/)
- [AzerothCore](https://github.com/AzerothCore/azerothcore-wotlk/graphs/contributors)
- [AzerothCore Discord](https://discord.gg/gkt4y2x)
- [EMUDevs](https://youtube.com/user/EmuDevs)
- [AC-Web](http://ac-web.org/)
- [ModCraft.io](http://modcraft.io/)
- [OwnedCore](http://ownedcore.com/)
- [OregonCore](https://wiki.oregon-core.net/)
- [Wowhead.com](http://wowhead.com)
- [AoWoW](https://wotlk.evowow.com/)


### License ###
------------------------------------------------------------------------------------------------------------------
- This code and content is released under the [GNU AGPL v3](https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3).

*/

#include "Config.h"
#include "ScriptPCH.h"
#include "Configuration/Config.h"

class BufferAnnounce : public PlayerScript
{

public:

    BufferAnnounce() : PlayerScript("BufferAnnounce") {}

    void OnLogin(Player* player)
    {
        // Announce Module
        if (sConfigMgr->GetBoolDefault("BufferNPC.Announce", true))
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00BufferNPC |rmodule.");
        }
    }
};

class buff_npc : public CreatureScript
{

public:

    buff_npc() : CreatureScript("buff_npc") { }

    bool OnGossipSelect(Player * player, Creature * creature, uint32 /*uiSender*/, uint32 uiAction)
    {
        std::vector<uint32> vecBuffs;
        std::stringstream ss(sConfigMgr->GetStringDefault("Buff.Spells", ""));

        for (std::string buff; std::getline(ss, buff, ';');)
        {
            vecBuffs.push_back(stoul(buff));
        }

        // Remove Ressurection Sickness?
        if (sConfigMgr->GetBoolDefault("Buff.CureRes", true))
        {
            // Remove Debuffs
            player->RemoveAura(15007, true);	// Cure Ressurection Sickness
        }

        // Apply Buffs

        for (std::vector<uint32>::const_iterator itr = vecBuffs.begin(); itr != vecBuffs.end(); itr++)
            player->CastSpell(player, *itr, true);
        

        // NPC Emote
        creature->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
        creature->MonsterWhisper("You're buffed, you're the stuff, and the elven females can't get enuff!", player);
        player->CLOSE_GOSSIP_MENU();

        return true;
    }
};

class npc_buffer : public WorldScript
{
public:
    npc_buffer() : WorldScript("npc_buffer") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "settings/modules/npc_buffer.conf";

            #ifdef WIN32
                cfg_file = "settings/modules/npc_buffer.conf";
            #endif
            std::string cfg_def_file = cfg_file + ".dist";

            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
};

void AddNPCBufferScripts()
{
    new npc_buffer();
    new BufferAnnounce();
    new buff_npc();
}
