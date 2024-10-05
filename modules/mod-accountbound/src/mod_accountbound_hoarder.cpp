#include "mod_accountbound.h"

bool AccountBound::OnGossipHello(Player* player, Creature* creature)
{
    if (!EnableHeirlooms)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("This feature is currently disabled.");
        CloseGossipMenuFor(player);
        return true;
    }

    QueryResult result = LoginDatabase.Query("SELECT `item_id` FROM `account_bound_heirlooms` WHERE `account_id` = {}",
            player->GetSession()->GetAccountId());

    if (!result)
    {
        ChatHandler(player->GetSession()).PSendSysMessage("You don't have any stored heirlooms.");
        CloseGossipMenuFor(player);
        return true;
    }

    uint32 vendorId = 7500000 + player->GetSession()->GetAccountId();

    do
    {
        Field* fields = result->Fetch();
        uint32 itemId = fields[0].Get<uint32>();

        sObjectMgr->RemoveVendorItem(vendorId, itemId);
        sObjectMgr->AddVendorItem(vendorId, itemId, 0, 0, 0, false);
    } while (result->NextRow());

    player->GetSession()->SendListInventory(creature->GetGUID(), vendorId);

    return true;
}
