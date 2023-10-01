#include "Auctionator.h"
#include "AuctionHouseMgr.h"
#include "AuctionatorSeller.h"
#include "Item.h"
#include "DatabaseEnv.h"
#include "PreparedStatement.h"


AuctionatorSeller::AuctionatorSeller(Auctionator* natorParam, uint32 auctionHouseIdParam)
{
    nator = natorParam;
    auctionHouseId = auctionHouseIdParam;

    ahMgr = nator->GetAuctionMgr(auctionHouseId);
};

AuctionatorSeller::~AuctionatorSeller()
{
    // TODO: clean up
};

void AuctionatorSeller::LetsGetToIt(uint32 maxCount, uint32 houseId)
{
    std::string itemQuery = R"(
        SELECT
            it.entry
            , it.name
            , it.BuyPrice
            , it.stackable
            , aicconf.stack_count
        FROM
            acore_world.mod_auctionator_itemclass_config aicconf
            LEFT JOIN acore_world.item_template it ON 
                aicconf.class = it.class 
                AND aicconf.subclass = it.subclass 
                -- skip BoP
                AND it.bonding != 1
                AND (
                    it.bonding >= aicconf.bonding
                    OR it.bonding = 0
                )
            LEFT JOIN acore_world.mod_auctionator_disabled_items dis on it.entry = dis.item
            LEFT JOIN (
                -- this sub query lets us get the current count of each item already in the AH
                -- so that we can filter out any items where itemCount >= max_count and not add
                -- anymore of them.
                SELECT
                    count(ii.itemEntry) as itemCount
                    , ii.itemEntry AS itemEntry
                FROM
                    acore_characters.item_instance ii 
                    INNER JOIN acore_characters.auctionhouse ah ON ii.guid = ah.itemguid 
                    LEFT JOIN acore_world.item_template it ON ii.itemEntry = it.entry 
                WHERE ah.houseId = {}
                GROUP BY ii.itemEntry, it.name
            ) ic ON ic.itemEntry = it.entry
        WHERE
            -- filter out items from the disabled table
            dis.item IS NULL
            -- filter out items with 'depreacted' anywhere in the name
            AND it.name NOT LIKE '%deprecated%'
            -- filter out items that start with 'Test'
            AND it.name NOT LIKE 'Test%'
            AND it.name NOT LIKE 'NPC %'
            -- filter out items where we are already at or above max_count for uniques in this class to limit dups
            AND (ic.itemCount IS NULL OR ic.itemCount < aicconf.max_count)
        ORDER BY RAND()
        LIMIT {}
        ;
    )";

    QueryResult result = WorldDatabase.Query(
        itemQuery,
        houseId,
        maxCount
    );

    if (!result)
    {
        nator->logDebug("No results for LetsGo item query");
        return;
    }

    uint32 count = 0;
    uint32 stackSize = 1;
    uint32 price = 0;
    do
    {
        count++;
        Field* fields = result->Fetch();
        std::string itemName = fields[1].Get<std::string>();

        stackSize = fields[4].Get<uint32>();

        price = fields[2].Get<uint32>();
        if (price == 0) {
            price = 10000000;
        }

        AuctionatorItem newItem = AuctionatorItem();
        newItem.itemId = fields[0].Get<uint32>();
        newItem.quantity = 1;
        newItem.buyout = price * stackSize;
        newItem.time = 60 * 60 * 12;
        newItem.stackSize = stackSize;

        nator->logDebug("Adding item: " + itemName
            + " with quantity of " + std::to_string(newItem.quantity)
            + " at price of " +  std::to_string(newItem.buyout)
            + " to house " + std::to_string(houseId)
        );


        nator->CreateAuction(newItem, houseId);
    } while (result -> NextRow());

    nator->logInfo("Items added houseId("
        + std::to_string(houseId)
        + ") this run: " + std::to_string(count));

};
