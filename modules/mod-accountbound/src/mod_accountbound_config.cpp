#include "mod_accountbound.h"

void AccountBound::OnAfterConfigLoad(bool /*reload*/)
{
    EnableGamemasters = sConfigMgr->GetOption<bool>("AccountBound.Gamemasters", 0);
    EnableCompanions = sConfigMgr->GetOption<bool>("AccountBound.Companions", 1);
    EnableHeirlooms = sConfigMgr->GetOption<bool>("AccountBound.Heirlooms", 0);
    EnableMounts = sConfigMgr->GetOption<bool>("AccountBound.Mounts", 1);
}
