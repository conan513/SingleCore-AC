#include "../pchdef.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "DatabaseEnv.h"
#include "PlayerbotAI.h"
#include "AiFactory.h"
#include "MapManager.h"
#include "PlayerbotCommandServer.h"
#include "GuildTaskMgr.h"
#include "Battleground.h"

RandomPlayerbotMgr::RandomPlayerbotMgr() : PlayerbotHolder(), processTicks(0)
{
    //sPlayerbotCommandServer.Start();
}

RandomPlayerbotMgr::~RandomPlayerbotMgr()
{
}

int RandomPlayerbotMgr::GetMaxAllowedBotCount()
{
	return GetEventValue(0, "bot_count");
}

void RandomPlayerbotMgr::UpdateAIInternal(uint32 elapsed)
{
    SetNextCheckDelay(sPlayerbotAIConfig.randomBotUpdateInterval * 1000);

    if (!sPlayerbotAIConfig.randomBotAutologin || !sPlayerbotAIConfig.enabled)
        return;

    int maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (!maxAllowedBotCount)
    {
        maxAllowedBotCount = urand(sPlayerbotAIConfig.minRandomBots, sPlayerbotAIConfig.maxRandomBots);
        SetEventValue(0, "bot_count", maxAllowedBotCount,
                urand(sPlayerbotAIConfig.randomBotCountChangeMinInterval, sPlayerbotAIConfig.randomBotCountChangeMaxInterval));
    }

    list<uint32> bots = GetBots();
    int botCount = bots.size();
    int randomBotsPerInterval = (int)urand(sPlayerbotAIConfig.minRandomBotsPerInterval, sPlayerbotAIConfig.maxRandomBotsPerInterval);
    if (!processTicks)
    {
        if (sPlayerbotAIConfig.randomBotLoginAtStartup)
            randomBotsPerInterval = bots.size();
    }

    while (botCount++ < maxAllowedBotCount)
    {
        bool alliance = botCount % 2;
        uint32 bot = AddRandomBot(alliance);
        if (bot) bots.push_back(bot);
        else break;
    }

    int botProcessed = 0;
    for (list<uint32>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        uint32 bot = *i;
        if (ProcessBot(bot))
            botProcessed++;

        if (botProcessed >= randomBotsPerInterval)
            break;
    }

    ostringstream out; out << "Random bots are now scheduled to be processed in the background. Next re-schedule in " << sPlayerbotAIConfig.randomBotUpdateInterval << " seconds";
    sLog->outBasic(out.str().c_str());
    //sWorld->SendWorldText(3, out.str().c_str());

    PrintStats();
}

uint32 RandomPlayerbotMgr::AddRandomBot(bool alliance)
{
    vector<uint32> bots = GetFreeBots(alliance);
    if (bots.size() == 0)
        return 0;

    int index = urand(0, bots.size() - 1);
    uint32 bot = bots[index];
    SetEventValue(bot, "add", 1, urand(sPlayerbotAIConfig.minRandomBotInWorldTime, sPlayerbotAIConfig.maxRandomBotInWorldTime));
    uint32 randomTime = 30 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3);
    ScheduleRandomize(bot, randomTime);
    sLog->outBasic("Random bot %d added", bot);
    return bot;
}

void RandomPlayerbotMgr::ScheduleRandomize(uint32 bot, uint32 time)
{
    SetEventValue(bot, "randomize", 1, time);
    SetEventValue(bot, "logout", 1, time + 30 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3));
}

void RandomPlayerbotMgr::ScheduleTeleport(uint32 bot)
{
    SetEventValue(bot, "teleport", 1, 60 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3));
}

bool RandomPlayerbotMgr::ProcessBot(uint32 bot)
{
	uint32 isValid = GetEventValue(bot, "add");
    if (!isValid)
    {
		Player* player = GetPlayerBot(bot);
		if (!player || !player->GetGroup())
		{
			sLog->outBasic("Bot %d expired", bot);
			SetEventValue(bot, "add", 0, 0);
		}
        return true;
    }

    if (!GetPlayerBot(bot))
    {
        AddPlayerBot(bot, 0);
        if (!GetEventValue(bot, "online"))
        {
            SetEventValue(bot, "online", 1, sPlayerbotAIConfig.minRandomBotInWorldTime);
        }
        return true;
    }

    Player* player = GetPlayerBot(bot);
    if (!player)
        return false;
	
	if (player->InBattleground() && player->isDead())
	{
		Battleground *bg = player->GetBattleground();		
		const WorldSafeLocsEntry *pos = sObjectMgr->GetClosestGraveyard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeamId());
		if (!player->IsWithinDist3d(pos->x,pos->y,pos->z,3.0))
		{
			// Special handle for battleground maps
			sLog->outBasic("bot %s died in a battleground. Try to resurrect.", player->GetName().c_str());
			SetEventValue(bot, "dead", 1, 5);
			//this is spirit release confirm?			
			player->RemovePet(NULL, PET_SAVE_NOT_IN_SLOT, true);
			player->BuildPlayerRepop();
			player->SpawnCorpseBones();
			player->RepopAtGraveyard();
		}
		else {
			player->ResurrectPlayer(1.0f);
		}
		return false;
	}

	if (player->InBattleground())
	{
		return false;
	}
	
    PlayerbotAI* ai = player->GetPlayerbotAI();
    if (!ai)
        return false;

    if (player->GetGroup())
    {
        sLog->outBasic("Skipping bot %d as it is in group", bot);
        return false;
    }

    ai->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);
    return true;
}

bool RandomPlayerbotMgr::ProcessBot(Player* player)
{
	player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(false);

	uint32 bot = player->GetGUID();
    if (player->isDead())
    {
        if (!GetEventValue(bot, "dead"))
        {
            sLog->outBasic("Setting dead flag for bot %d", bot);
            uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotReviveTime, sPlayerbotAIConfig.maxRandomBotReviveTime);
            SetEventValue(bot, "dead", 1, randomTime);
            SetEventValue(bot, "revive", 1, randomTime - 60);
            return false;
        }

        if (!GetEventValue(bot, "revive"))
        {
            Revive(player);
            return true;
        }

        return false;
    }

    if (player->GetGuild() && player->GetGuild()->GetLeaderGUID() == player->GetGUID())
    {
        for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
            sGuildTaskMgr.Update(*i, player);
    }	
		
    uint32 randomize = GetEventValue(bot, "randomize");
    if (!randomize)
    {
        sLog->outBasic("Randomizing bot %d", bot);
        Randomize(player);
        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotRandomizeTime, sPlayerbotAIConfig.maxRandomBotRandomizeTime);
        ScheduleRandomize(bot, randomTime);
        return true;
    }

    uint32 logout = GetEventValue(bot, "logout");
    if (!logout)
    {
        sLog->outBasic("Logging out bot %d", bot);
        LogoutPlayerBot(bot);
        SetEventValue(bot, "logout", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
        return true;
    }

    uint32 teleport = GetEventValue(bot, "teleport");
    if (!teleport)
    {
        sLog->outBasic("Random teleporting bot %d", bot);
		RandomTeleportForLevel(player);
        SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
        return true;
    }

    return false;
}

void RandomPlayerbotMgr::Revive(Player* player)
{
    uint32 bot = player->GetGUID();
    sLog->outBasic("Reviving dead bot %d", bot);
    SetEventValue(bot, "dead", 0, 0);
    SetEventValue(bot, "revive", 0, 0);
    RandomTeleport(player, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot, vector<WorldLocation> &locs)
{
	if (bot->InBattleground())
		return;
	
    if (bot->IsBeingTeleported())
        return;

    if (locs.empty())
    {
        sLog->outError("Cannot teleport bot %s - no locations available", bot->GetName().c_str());
        return;
    }

    for (int attemtps = 0; attemtps < 10; ++attemtps)
    {
        int index = urand(0, locs.size() - 1);
        WorldLocation loc = locs[index];
        float x = loc.m_positionX + urand(0, sPlayerbotAIConfig.grindDistance) - sPlayerbotAIConfig.grindDistance / 2;
        float y = loc.m_positionY + urand(0, sPlayerbotAIConfig.grindDistance) - sPlayerbotAIConfig.grindDistance / 2;
        float z = loc.m_positionZ;

        Map* map = sMapMgr->FindMap(loc.GetMapId(), 0);
        if (!map)
            continue;

        if (!map->IsOutdoors(x, y, z) ||
                map->IsInWater(x, y, z))
            continue;

        uint32 areaId = map->GetAreaId(x, y, z);
        if (!areaId)
            continue;

		AreaTableEntry const* area = sAreaStore.LookupEntry(areaId);
        if (!area)
            continue;

        float ground = map->GetHeight(x, y, z + 0.5f);
        if (ground <= INVALID_HEIGHT)
            continue;

        z = 0.05f + ground;
        sLog->outBasic("Random teleporting bot %s to %s %f,%f,%f (%u/%u locations)",
                bot->GetName().c_str(), area->area_name[0], x, y, z, attemtps, locs.size());

        bot->GetMotionMaster()->Clear();
        bot->TeleportTo(loc.GetMapId(), x, y, z, 0);
        return;
    }

    sLog->outError("Cannot teleport bot %s - no locations available", bot->GetName().c_str());
}

void RandomPlayerbotMgr::RandomTeleportForLevel(Player* bot)
{	
	if (bot->InBattleground())
		return;
	sLog->outBasic("Preparing location to random teleporting bot %s for level %u", bot->GetName().c_str(), bot->getLevel());

    if (locsPerLevelCache[bot->getLevel()].empty()) {
        QueryResult results = WorldDatabase.PQuery("select map, position_x, position_y, position_z "
            "from (select map, position_x, position_y, position_z, avg(t.maxlevel), avg(t.minlevel), "
            "%u - (avg(t.maxlevel) + avg(t.minlevel)) / 2 delta "
            "from creature c inner join creature_template t on c.id = t.entry group by t.entry) q "
            "where delta >= 0 and delta <= %u and map in (%s) and not exists ( "
            "select map, position_x, position_y, position_z from "
            "("
            "select map, c.position_x, c.position_y, c.position_z, avg(t.maxlevel), avg(t.minlevel), "
            "%u - (avg(t.maxlevel) + avg(t.minlevel)) / 2 delta "
            "from creature c "
            "inner join creature_template t on c.id = t.entry group by t.entry "
            ") q1 "
            "where delta > %u and q1.map = q.map "
            "and sqrt("
            "(q1.position_x - q.position_x)*(q1.position_x - q.position_x) +"
            "(q1.position_y - q.position_y)*(q1.position_y - q.position_y) +"
            "(q1.position_z - q.position_z)*(q1.position_z - q.position_z)"
            ") < %u) limit 50",
            bot->getLevel(),
            sPlayerbotAIConfig.randomBotTeleLevel,
            sPlayerbotAIConfig.randomBotMapsAsString.c_str(),
            bot->getLevel(),
            sPlayerbotAIConfig.randomBotTeleLevel,
            (uint32)sPlayerbotAIConfig.sightDistance
            );
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                uint16 mapId = fields[0].GetUInt16();
                float x = fields[1].GetFloat();
                float y = fields[2].GetFloat();
                float z = fields[3].GetFloat();
                WorldLocation loc(mapId, x, y, z, 0);
                locsPerLevelCache[bot->getLevel()].push_back(loc);
            } while (results->NextRow());
        }
    }

    RandomTeleport(bot, locsPerLevelCache[bot->getLevel()]);
	sLog->outBasic("Random teleport finished.");
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot, uint16 mapId, float teleX, float teleY, float teleZ)
{
	if (bot->InBattleground())
		return;
	sLog->outBasic("Preparing location to random teleporting bot %s", bot->GetName().c_str());

    vector<WorldLocation> locs;
    QueryResult results = WorldDatabase.PQuery("select position_x, position_y, position_z from creature where map = '%u' and abs(position_x - '%f') < '%u' and abs(position_y - '%f') < '%u' limit 50",
            mapId, teleX, sPlayerbotAIConfig.randomBotTeleportDistance / 2, teleY, sPlayerbotAIConfig.randomBotTeleportDistance / 2);
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            float x = fields[0].GetFloat();
            float y = fields[1].GetFloat();
            float z = fields[2].GetFloat();
            WorldLocation loc(mapId, x, y, z, 0);
            locs.push_back(loc);
        } while (results->NextRow());
    }

    RandomTeleport(bot, locs);
    Refresh(bot);
}

void RandomPlayerbotMgr::Randomize(Player* bot)
{
	if (bot->getLevel() == 1)
	{
		RandomizeFirst(bot);
	}
	else
	{
		// EJ bot wont increase level
		if (bot->isDead())
			bot->ResurrectPlayer(1.0f);
		bot->CombatStop(true);
		RandomTeleportForLevel(bot);
	}
}

void RandomPlayerbotMgr::IncreaseLevel(Player* bot)
{
    uint32 maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    uint32 level = min((uint32)(bot->getLevel() + 1), maxLevel);
    PlayerbotFactory factory(bot, level);
    if (bot->GetGuildId())
        factory.Refresh();
    else
        factory.Randomize();
    RandomTeleportForLevel(bot);
}

// EJ ignore take player level
void RandomPlayerbotMgr::RandomizeFirst(Player* bot)
{	
	uint32 maxLevel = sPlayerbotAIConfig.randomBotMaxLevel;
	uint32 minLevel = sPlayerbotAIConfig.randomBotMinLevel;	
	if (maxLevel > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
		maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
	if (minLevel < 1)
		minLevel = 1;

	uint32 level = urand(minLevel, maxLevel);

	PlayerbotFactory factory(bot, level);
	factory.CleanRandomize();
	RandomTeleportForLevel(bot);
}

uint32 RandomPlayerbotMgr::GetMasterLevel()
{
	if (masterLevel==0)
	{
		QueryResult results = CharacterDatabase.PQuery("select level from characters where cinematic = 1 order by online desc limit 1");
		if (results)
		{
			Field* fields = results->Fetch();
			masterLevel = fields[0].GetUInt8();
		}
	}
	return masterLevel;
}

uint32 RandomPlayerbotMgr::GetZoneLevel(uint16 mapId, float teleX, float teleY, float teleZ)
{
    uint32 maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);

	uint32 level;
    QueryResult results = WorldDatabase.PQuery("select avg(t.minlevel) minlevel, avg(t.maxlevel) maxlevel from creature c "
            "inner join creature_template t on c.id = t.entry "
            "where map = '%u' and minlevel > 1 and abs(position_x - '%f') < '%u' and abs(position_y - '%f') < '%u'",
            mapId, teleX, sPlayerbotAIConfig.randomBotTeleportDistance / 2, teleY, sPlayerbotAIConfig.randomBotTeleportDistance / 2);

    if (results)
    {
        Field* fields = results->Fetch();
        uint8 minLevel = fields[0].GetUInt8();
        uint8 maxLevel = fields[1].GetUInt8();
        level = urand(minLevel, maxLevel);
        if (level > maxLevel)
            level = maxLevel;
    }
    else
    {
        level = urand(1, maxLevel);
    }

    return level;
}

void RandomPlayerbotMgr::Refresh(Player* bot)
{
	if (bot->InBattleground())
		return;
	sLog->outBasic("Refreshing bot %s", bot->GetName().c_str());
	if (bot->isDead())
	{
		bot->ResurrectPlayer(1.0f);
		bot->SpawnCorpseBones();
		bot->SaveToDB(false, true);
		bot->GetPlayerbotAI()->ResetStrategies();
	}

    bot->GetPlayerbotAI()->Reset();

	HostileReference *ref = bot->getHostileRefManager().getFirst();
	while (ref)
	{
		ThreatManager *threatManager = ref->GetSource();
		Unit *unit = threatManager->GetOwner();
		float threat = ref->getThreat();

        unit->RemoveAllAttackers();
        unit->ClearInCombat();

        ref = ref->next();
    }

    bot->RemoveAllAttackers();
    bot->ClearInCombat();

    bot->DurabilityRepairAll(false, 1.0f, false);
    bot->SetFullHealth();
    bot->SetPvP(true);

    if (bot->GetMaxPower(POWER_MANA) > 0)
        bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    if (bot->GetMaxPower(POWER_ENERGY) > 0)
        bot->SetPower(POWER_ENERGY, bot->GetMaxPower(POWER_ENERGY));
}


bool RandomPlayerbotMgr::IsRandomBot(Player* bot)
{
    return IsRandomBot(bot->GetGUID());
}

bool RandomPlayerbotMgr::IsRandomBot(uint32 bot)
{
    return GetEventValue(bot, "add");
}

list<uint32> RandomPlayerbotMgr::GetBots()
{
    list<uint32> bots;

    QueryResult results = CharacterDatabase.Query(
            "select bot from ai_playerbot_random_bots where owner = 0 and event = 'add'");

    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 bot = fields[0].GetUInt32();
            bots.push_back(bot);
        } while (results->NextRow());
    }

    return bots;
}

vector<uint32> RandomPlayerbotMgr::GetFreeBots(bool alliance)
{
    set<uint32> bots;

    QueryResult results = CharacterDatabase.PQuery(
            "select `bot` from ai_playerbot_random_bots where event = 'add'");

    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 bot = fields[0].GetUInt32();
            bots.insert(bot);
        } while (results->NextRow());
    }

    vector<uint32> guids;
    for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotAccounts.begin(); i != sPlayerbotAIConfig.randomBotAccounts.end(); i++)
    {
        uint32 accountId = *i;
        if (!AccountMgr::GetCharactersCount(accountId))
            continue;

        QueryResult result = CharacterDatabase.PQuery("SELECT guid, race FROM characters WHERE account = '%u'", accountId);
        if (!result)
            continue;

        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint8 race = fields[1].GetUInt8();
            if (bots.find(guid) == bots.end() &&
                    ((alliance && IsAlliance(race)) || ((!alliance && !IsAlliance(race))
            )))
                guids.push_back(guid);
        } while (result->NextRow());
    }


    return guids;
}

uint32 RandomPlayerbotMgr::GetEventValue(uint32 bot, string event)
{
    uint32 value = 0;

    QueryResult results = CharacterDatabase.PQuery(
            "select `value`, `time`, validIn from ai_playerbot_random_bots where owner = 0 and bot = '%u' and event = '%s'",
            bot, event.c_str());

    if (results)
    {
        Field* fields = results->Fetch();
        value = fields[0].GetUInt32();
        uint32 lastChangeTime = fields[1].GetUInt32();
        uint32 validIn = fields[2].GetUInt32();
        if ((time(0) - lastChangeTime) >= validIn)
            value = 0;
    }

    return value;
}

uint32 RandomPlayerbotMgr::SetEventValue(uint32 bot, string event, uint32 value, uint32 validIn)
{
    CharacterDatabase.PExecute("delete from ai_playerbot_random_bots where owner = 0 and bot = '%u' and event = '%s'",
            bot, event.c_str());
    if (value)
    {
        CharacterDatabase.PExecute(
                "insert into ai_playerbot_random_bots (owner, bot, `time`, validIn, event, `value`) values ('%u', '%u', '%u', '%u', '%s', '%u')",
                0, bot, (uint32)time(0), validIn, event.c_str(), value);
    }

    return value;
}

bool RandomPlayerbotMgr::HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args)
{
    if (!sPlayerbotAIConfig.enabled)
    {
        sLog->outError("Playerbot system is currently disabled!");
        return false;
    }

    if (!args || !*args)
    {
        sLog->outError("Usage: rndbot stats/update/reset/init/refresh/add/remove");
        return false;
    }

    string cmd = args;

    if (cmd == "reset")
    {
        CharacterDatabase.PExecute("delete from ai_playerbot_random_bots");
        sLog->outBasic("Random bots were reset for all players. Please restart the Server.");
        return true;
    }
    else if (cmd == "stats")
    {
        sRandomPlayerbotMgr.PrintStats();
        return true;
    }
    else if (cmd == "update")
    {
        sRandomPlayerbotMgr.UpdateAIInternal(0);
        return true;
    }
    else if (cmd == "init" || cmd == "refresh" || cmd == "teleport" || cmd == "revive")
    {
		sLog->outBasic("Randomizing bots for %d accounts", sPlayerbotAIConfig.randomBotAccounts.size());
        list<uint32> botIds;
        for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotAccounts.begin(); i != sPlayerbotAIConfig.randomBotAccounts.end(); ++i)
        {
            uint32 account = *i;
            if (QueryResult results = CharacterDatabase.PQuery("SELECT guid FROM characters where account = '%u'", account))
            {
                do
                {
                    Field* fields = results->Fetch();

                    uint32 botId = fields[0].GetUInt32();
					uint64 guid = MAKE_NEW_GUID(botId, 0, HIGHGUID_PLAYER);
                    Player* bot = ObjectAccessor::FindPlayer(guid);
                    if (!bot)
                        continue;

                    botIds.push_back(botId);
                } while (results->NextRow());
            }
        }

        int processed = 0;
        for (list<uint32>::iterator i = botIds.begin(); i != botIds.end(); ++i)
        {
            uint64 guid = MAKE_NEW_GUID(*i, 0, HIGHGUID_PLAYER);
            Player* bot = ObjectAccessor::FindPlayer(guid);
            if (!bot)
                continue;

            sLog->outBasic("[%u/%u] Processing command '%s' for bot '%s'",
                    processed++, botIds.size(), cmd.c_str(), bot->GetName().c_str());

            if (cmd == "init")
            {
                sRandomPlayerbotMgr.RandomizeFirst(bot);
            }
            else if (cmd == "teleport")
            {
                sRandomPlayerbotMgr.RandomTeleportForLevel(bot);
            }
			else if (cmd == "revive")
			{
				sRandomPlayerbotMgr.Revive(bot);
			}
            else
            {
                bot->SetLevel(bot->getLevel() - 1);
                sRandomPlayerbotMgr.IncreaseLevel(bot);
            }
            uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotRandomizeTime, sPlayerbotAIConfig.maxRandomBotRandomizeTime);
            CharacterDatabase.PExecute("update ai_playerbot_random_bots set validIn = '%u' where event = 'randomize' and bot = '%u'",
                    randomTime, bot->GetGUID());
            CharacterDatabase.PExecute("update ai_playerbot_random_bots set validIn = '%u' where event = 'logout' and bot = '%u'",
                    sPlayerbotAIConfig.maxRandomBotInWorldTime, bot->GetGUID());
        }
        return true;
    }
    else
    {
        list<string> messages = sRandomPlayerbotMgr.HandlePlayerbotCommand(args, NULL);
        for (list<string>::iterator i = messages.begin(); i != messages.end(); ++i)
        {
            sLog->outBasic(i->c_str());
        }
        return true;
    }

    return false;
}

void RandomPlayerbotMgr::HandleCommand(uint32 type, const string& text, Player& fromPlayer)
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        bot->GetPlayerbotAI()->HandleCommand(type, text, fromPlayer);
    }
}

void RandomPlayerbotMgr::OnPlayerLogout(Player* player)
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* ai = bot->GetPlayerbotAI();
        if (player == ai->GetMaster())
        {
			ai->SetMaster(NULL);
			if (!bot->InBattleground())
				ai->ResetStrategies();
        }
    }

    if (!player->GetPlayerbotAI())
    {
        vector<Player*>::iterator i = find(players.begin(), players.end(), player);
        if (i != players.end())
            players.erase(i);
    }
}

void RandomPlayerbotMgr::OnPlayerLogin(Player* player)
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        if (player == bot || player->GetPlayerbotAI())
            continue;

        Group* group = bot->GetGroup();
        if (!group)
            continue;

        for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->GetSource();
            PlayerbotAI* ai = bot->GetPlayerbotAI();
            if (member == player && (!ai->GetMaster() || ai->GetMaster()->GetPlayerbotAI()))
            {
				if (!bot->InBattleground())
				{
					ai->SetMaster(player);
					ai->ResetStrategies();
					ai->TellMaster("Hello");
				}
                break;
            }
        }
    }

    if (player->GetPlayerbotAI())
        return;

    players.push_back(player);
}

Player* RandomPlayerbotMgr::GetRandomPlayer()
{
    if (players.empty())
        return NULL;

    uint32 index = urand(0, players.size() - 1);
    return players[index];
}

void RandomPlayerbotMgr::PrintStats()
{
    sLog->outBasic("%d Random Bots online", playerBots.size());

    map<uint32, int> alliance, horde;
    for (uint32 i = 0; i < 10; ++i)
    {
        alliance[i] = 0;
        horde[i] = 0;
    }

    map<uint8, int> perRace, perClass;
    for (uint8 race = RACE_HUMAN; race < MAX_RACES; ++race)
    {
        perRace[race] = 0;
    }
    for (uint8 cls = CLASS_WARRIOR; cls < MAX_CLASSES; ++cls)
    {
        perClass[cls] = 0;
    }

    int dps = 0, heal = 0, tank = 0;
    for (PlayerBotMap::iterator i = playerBots.begin(); i != playerBots.end(); ++i)
    {
        Player* bot = i->second;
        if (IsAlliance(bot->getRace()))
            alliance[bot->getLevel() / 10]++;
        else
            horde[bot->getLevel() / 10]++;

        perRace[bot->getRace()]++;
        perClass[bot->getClass()]++;

        int spec = AiFactory::GetPlayerSpecTab(bot);
        switch (bot->getClass())
        {
        case CLASS_DRUID:
            if (spec == 2)
                heal++;
            else
                dps++;
            break;
        case CLASS_PALADIN:
            if (spec == 1)
                tank++;
            else if (spec == 0)
                heal++;
            else
                dps++;
            break;
        case CLASS_PRIEST:
            if (spec != 2)
                heal++;
            else
                dps++;
            break;
        case CLASS_SHAMAN:
            if (spec == 2)
                heal++;
            else
                dps++;
            break;
		case CLASS_DEATH_KNIGHT :
			if (spec == 0)
				tank++;
			else
				dps++;
			break;
        case CLASS_WARRIOR:
            if (spec == 2)
                tank++;
            else
                dps++;
            break;
        default:
            dps++;
            break;
        }
    }

    sLog->outBasic("Per level:");
    uint32 maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    for (uint32 i = 0; i < 10; ++i)
    {
        if (!alliance[i] && !horde[i])
            continue;

        uint32 from = i*10;
        uint32 to = min(from + 9, maxLevel);
        if (!from) from = 1;
        sLog->outBasic("    %d..%d: %d alliance, %d horde", from, to, alliance[i], horde[i]);
    }
    sLog->outBasic("Per race:");
    for (uint8 race = RACE_HUMAN; race < MAX_RACES; ++race)
    {
        if (perRace[race])
            sLog->outBasic("    %s: %d", ChatHelper::formatRace(race).c_str(), perRace[race]);
    }
    sLog->outBasic("Per class:");
    for (uint8 cls = CLASS_WARRIOR; cls < MAX_CLASSES; ++cls)
    {
        if (perClass[cls])
            sLog->outBasic("    %s: %d", ChatHelper::formatClass(cls).c_str(), perClass[cls]);
    }
    sLog->outBasic("Per role:");
    sLog->outBasic("    tank: %d", tank);
    sLog->outBasic("    heal: %d", heal);
    sLog->outBasic("    dps: %d", dps);
}

double RandomPlayerbotMgr::GetBuyMultiplier(Player* bot)
{
    uint32 id = bot->GetGUID();
    uint32 value = GetEventValue(id, "buymultiplier");
    if (!value)
    {
        value = urand(1, 120);
        uint32 validIn = urand(sPlayerbotAIConfig.minRandomBotsPriceChangeInterval, sPlayerbotAIConfig.maxRandomBotsPriceChangeInterval);
        SetEventValue(id, "buymultiplier", value, validIn);
    }

    return (double)value / 100.0;
}

double RandomPlayerbotMgr::GetSellMultiplier(Player* bot)
{
    uint32 id = bot->GetGUID();
    uint32 value = GetEventValue(id, "sellmultiplier");
    if (!value)
    {
        value = urand(80, 250);
        uint32 validIn = urand(sPlayerbotAIConfig.minRandomBotsPriceChangeInterval, sPlayerbotAIConfig.maxRandomBotsPriceChangeInterval);
        SetEventValue(id, "sellmultiplier", value, validIn);
    }

    return (double)value / 100.0;
}

uint32 RandomPlayerbotMgr::GetLootAmount(Player* bot)
{
    uint32 id = bot->GetGUID();
    return GetEventValue(id, "lootamount");
}

void RandomPlayerbotMgr::SetLootAmount(Player* bot, uint32 value)
{
    uint32 id = bot->GetGUID();
    SetEventValue(id, "lootamount", value, 24 * 3600);
}

uint32 RandomPlayerbotMgr::GetTradeDiscount(Player* bot)
{
    Group* group = bot->GetGroup();
    return GetLootAmount(bot) / (group ? group->GetMembersCount() : 10);
}

string RandomPlayerbotMgr::HandleRemoteCommand(string request)
{
    string::iterator pos = find(request.begin(), request.end(), ',');
    if (pos == request.end())
    {
        ostringstream out; out << "invalid request: " << request;
        return out.str();
    }

    string command = string(request.begin(), pos);
    uint64 guid = atoi(string(pos + 1, request.end()).c_str());
    Player* bot = GetPlayerBot(guid);
    if (!bot)
        return "invalid guid";

    PlayerbotAI *ai = bot->GetPlayerbotAI();
    if (!ai)
        return "invalid guid";

    return ai->HandleRemoteCommand(command);
}