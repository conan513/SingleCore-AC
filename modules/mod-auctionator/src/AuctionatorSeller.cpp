#include "Auctionator.h"
#include "AuctionHouseMgr.h"
#include "AuctionatorSeller.h"
#include "Item.h"
#include "DatabaseEnv.h"
#include "PreparedStatement.h"
#include <random>


AuctionatorSeller::AuctionatorSeller(Auctionator* natorParam, uint32 auctionHouseIdParam)
{
    SetLogPrefix("[AuctionatorSeller] ");
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

    // Set the maximum number of items to query for. Changing this <might>
    // affect how random our auctoin listing are at the cost of memory/cpu
    // but it is something i need to test.
    uint32 queryLimit = nator->config->sellerConfig.queryLimit;

    // Get the name of the character database so we can do our join below.
    std::string characterDbName = CharacterDatabase.GetConnectionInfo()->database;

    // soooo, let's talk query.
    // I like doing this with a query because it's easy to me but this isn't
    // really how acore works AND this makes this module completely incompatible
    // with Trinitycore and other cores that don't have a template table and instead
    // rely on the DBC files. I really should use the memory store of templates
    // for this but it would mean a TON of extra knarly c++ code that i just don't
    // want to write. If you want to run this on trinity or want to write that nasty
    // c++ in a trinity specific iffy block thing and submit a PR i would be happy
    // to consider it, but left on my own I am going to stick with this sql query.
    std::string itemQuery = R"(
        SELECT
            it.entry
            , it.name
            , it.BuyPrice
            , it.stackable
            , it.quality
            , mp.average_price
        FROM
            mod_auctionator_itemclass_config aicconf
            LEFT JOIN item_template it ON
                aicconf.class = it.class
                AND aicconf.subclass = it.subclass
                -- skip BoP
                AND it.bonding != 1
                AND (
                    it.bonding >= aicconf.bonding
                    OR it.bonding = 0
                )
            LEFT JOIN mod_auctionator_disabled_items dis on it.entry = dis.item
            LEFT JOIN (
                -- this sub query lets us get the current count of each item already in the AH
                -- so that we can filter out any items where itemCount >= max_count and not add
                -- anymore of them.
                SELECT
                    count(ii.itemEntry) as itemCount
                    , ii.itemEntry AS itemEntry
                FROM
                    {}.item_instance ii
                    INNER JOIN {}.auctionhouse ah ON ii.guid = ah.itemguid
                    LEFT JOIN item_template it ON ii.itemEntry = it.entry
                WHERE ah.houseId = {}
                GROUP BY ii.itemEntry, it.name
            ) ic ON ic.itemEntry = it.entry
            LEFT JOIN
                -- get the newest prices for our items that we have in our
                -- market price table.
                (
                    SELECT
                        DISTINCT(mpp.entry),
                        mpa.average_price
                    FROM {}.mod_auctionator_market_price mpp
                    INNER JOIN (
                        SELECT
                            max(scan_datetime) AS scan,
                            entry
                        FROM {}.mod_auctionator_market_price
                        GROUP BY entry
                    ) mps ON mpp.entry = mps.entry
                    INNER JOIN
                        {}.mod_auctionator_market_price mpa
                        ON mpa.entry = mpp.entry
                        AND mpa.scan_datetime = mps.scan
                ) mp ON it.entry = mp.entry
        WHERE
            -- filter out items from the disabled table
            dis.item IS NULL
            -- filter out items with 'depreacted' anywhere in the name
            -- AND it.name NOT LIKE '%deprecated%'
            -- filter out items that start with 'Test'
            -- AND it.name NOT LIKE 'Test%'
            -- AND it.name NOT LIKE 'NPC %'
            -- filter out items where we are already at or above max_count
            -- for uniques in this class to limit dups
            AND (ic.itemCount IS NULL OR ic.itemCount < aicconf.max_count)
            AND VerifiedBuild != 1
        ORDER BY RAND()
        LIMIT {}
        ;
    )";

    QueryResult result = WorldDatabase.Query(
        itemQuery,
        characterDbName,
        characterDbName,
        houseId,
        characterDbName,
        characterDbName,
        characterDbName,
        queryLimit
    );

    if (!result)
    {
        logDebug("No results for LetsGo item query");
        return;
    }

    AuctionatorPriceMultiplierConfig multiplierConfig = nator->config->sellerMultipliers;
    uint32 count = 0;
    do
    {
        count++;
        Field* fields = result->Fetch();

        // TODO: refactor listing an item into a testable method
        std::string itemName = fields[1].Get<std::string>();

        uint32 stackSize = fields[3].Get<uint32>();
        if (stackSize > 20) {
            stackSize = 20;
        }

        // create a random number between 1 and stackSize
        if (stackSize > 1 && nator->config->sellerConfig.randomizeStackSize) {
            stackSize = GetRandomNumber(1, stackSize);
            logDebug("Stack size: " + std::to_string(stackSize));
        }

        uint32 quality = fields[4].Get<uint32>();
        float qualityMultiplier = Auctionator::GetQualityMultiplier(multiplierConfig, quality);

        uint32 price = fields[2].Get<uint32>();
        uint32 marketPrice = fields[5].Get<uint32>();
        if (marketPrice > 0) {
            logDebug("Using Market over Template [" + itemName + "] " +
                std::to_string(marketPrice) + " <--> " + std::to_string(price));
            price = marketPrice;
        }

        if (price == 0) {
            price = 10000000 * qualityMultiplier;
        }

        // calculate our starting bid price
        uint32 bidPrice = price;
        float bidStartModifier = nator->config->sellerConfig.bidStartModifier;
        logDebug("Bid start modifier: " + std::to_string(bidStartModifier));
        bidPrice = GetRandomNumber(bidPrice - (bidPrice * bidStartModifier), bidPrice);
        logDebug("Bid price " + std::to_string(bidPrice) + " from price " + std::to_string(price));

        AuctionatorItem newItem = AuctionatorItem();
        newItem.itemId = fields[0].Get<uint32>();
        newItem.quantity = 1;
        newItem.houseId = houseId;
        newItem.buyout = uint32(price * stackSize * qualityMultiplier);
        newItem.bid = uint32(bidPrice * stackSize * qualityMultiplier);
        newItem.time = 60 * 60 * 12;
        newItem.stackSize = stackSize;

        logDebug("Adding item: " + itemName
            + " with quantity of " + std::to_string(newItem.quantity)
            + " at price of " +  std::to_string(newItem.buyout)
            + " to house " + std::to_string(houseId)
        );


        nator->CreateAuction(newItem);
        if (count == maxCount) {
            break;
        }
    } while (result->NextRow());

    logInfo("Items added houseId("
        + std::to_string(houseId)
        + ") this run: " + std::to_string(count));

};

uint32 AuctionatorSeller::GetRandomNumber(uint32 min, uint32 max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}