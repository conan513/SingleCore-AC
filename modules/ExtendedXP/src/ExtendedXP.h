#ifndef MODULE_EXTENDED_XP
#define MODULE_EXTENDED_XP

#include "ScriptMgr.h"
#include "Config.h"
#include "Player.h"

class ExtendedXPPlayer : public PlayerScript
{
public:
    ExtendedXPPlayer() : PlayerScript("ExtendedXPPlayer") { }

private:
    void OnGiveXP(Player* /*player*/, uint32& /*amount*/, Unit* /*victim*/, uint8 /*xpSource*/) override;
    void OnAchiComplete(Player* /*player*/, AchievementEntry const* /*achievement*/) override;
};

#endif // MODULE_EXTENDED_XP
