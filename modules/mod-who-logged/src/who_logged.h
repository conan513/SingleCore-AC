#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"

class WhoLoggedAnnounce : public PlayerScript
{
public:
    WhoLoggedAnnounce() : PlayerScript("WhoLoggedAnnounce") {}

    void OnLogin(Player* player) override;
};

void AddWhoLoggedScripts()
{
    new WhoLoggedAnnounce();
}
