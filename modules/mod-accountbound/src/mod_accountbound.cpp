#include "mod_accountbound.h"

AccountBound::AccountBound() : CreatureScript("npc_heirloom_hoarder"), PlayerScript("AccountBoundPlayerScript"), WorldScript("AccountBoundWorldScript") {}

void AccountBound::OnStartup()
{
    LOG_INFO("server.loading", "Loading account bound templates...");

    LoadCompanions();
    LoadFactionSpecificCompanions();
    LoadMounts();
    LoadFactionSpecificMounts();
}

void Addmod_accountboundScripts()
{
    new AccountBound();
}
