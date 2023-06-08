/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "Config.h"
#include "Common.h"
#include "World.h"
#include "StringFormat.h"

class PvPstatsAnnouncer : public WorldScript
{
public:
    PvPstatsAnnouncer() : WorldScript("PvPstatsAnnouncer") { }

    void OnUpdate(uint32 diff) override
    {
        if (!sConfigMgr->GetOption<bool>("PvPstatsAnnouncer.Enabled", false))
        {
            return;
        }

        auto repeatTime = sConfigMgr->GetOption<uint32>("PvPstatsAnnouncer.RepeatMinutes", 40) * MINUTE * IN_MILLISECONDS;
        _time += diff;

        if (_time > repeatTime)
        {
            auto limit = sConfigMgr->GetOption<uint8>("PvPstatsAnnouncer.Limit", 5);
            auto bracketId = sConfigMgr->GetOption<uint8>("PvPstatsAnnouncer.BracketId", 1);

            auto stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PVPSTATS_BRACKET_MONTH);
            stmt->SetData(0, bracketId);
            stmt->SetData(1, limit);

            auto result = CharacterDatabase.Query(stmt);

            if (result)
            {
                OutputGlobal(sConfigMgr->GetOption<std::string>("PvPstatsAnnouncer.StartText", ""));

                uint8 i = 1;
                do
                {
                    Field* fields = result->Fetch();
                    if (fields)
                    {
                        OutputGlobal(Acore::StringFormatFmt("{}. {} - {}", i, fields[2].Get<std::string_view>(), fields[1].Get<uint32>()));
                        i++;
                    }
                } while (result->NextRow());

                OutputGlobal(sConfigMgr->GetOption<std::string>("PvPstatsAnnouncer.EndText", ""));
            }

            _time = 0;
        }
    }
private:
    uint32 _time = 0;

    void OutputGlobal(std::string const& message)
    {
        sWorld->SendGlobalText(message.c_str(), nullptr);
    }
};

void AddPvPstatsAnnouncerScripts()
{
    new PvPstatsAnnouncer();
}

