#include "mod_accountbound.h"

void AccountBound::SaveHeirloom(Player* player, Item* item)
{
    LoginDatabase.Execute("REPLACE INTO `account_bound_heirlooms` (`account_id`, `item_id`) "
        "VALUES ({}, {})",
        player->GetSession()->GetAccountId(),
        item->GetEntry());

    item->SetNotRefundable(player);
}
