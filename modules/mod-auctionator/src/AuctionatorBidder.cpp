#include "AuctionatorBidder.h"
#include "Auctionator.h"
#include "ObjectMgr.h"
#include <random>


AuctionatorBidder::AuctionatorBidder(uint32 auctionHouseIdParam, ObjectGuid buyer, AuctionatorConfig* auctionatorConfig)
{
    SetLogPrefix("[AuctionatorBidder] ");
    auctionHouseId = auctionHouseIdParam;
    buyerGuid = buyer;
    ahMgr = sAuctionMgr->GetAuctionsMapByHouseId(auctionHouseId);
    config = auctionatorConfig;
    bidOnOwn = config->bidOnOwn;
}

AuctionatorBidder::~AuctionatorBidder()
{

}

void AuctionatorBidder::SpendSomeCash()
{
    uint32 auctionatorPlayerGuid = buyerGuid.GetRawValue();

    std::string query = R"(
        SELECT
            ah.id
        FROM auctionhouse ah
        WHERE itemowner <> {} AND houseid = {};
    )";

    // for testing we may want to bid on our own auctions.
    // if we do we set the ownerToSkip to 0 so we will pick
    // up all auctions including our own.
    uint32 ownerToSkip = auctionatorPlayerGuid;
    if (bidOnOwn) {
        ownerToSkip = 0;
    }

    QueryResult result = CharacterDatabase.Query(query, ownerToSkip, auctionHouseId);

    if (!result) {
        logInfo("Can't see player auctions at ["
            + std::to_string(auctionHouseId) + "] not from ["
            + std::to_string(auctionatorPlayerGuid) + "], moving on.");
        return;
    }

    if (result->GetRowCount() == 0) {
        logInfo("No player auctions, taking my money elsewhere.");
        return;
    }

    // move our query results into a vector for easier manipulation
    std::vector<uint32> biddableAuctionIds;
    do {
        biddableAuctionIds.push_back(result->Fetch()->Get<uint32>());
    } while(result->NextRow());

    // shuffle our vector to try to inject some randomness
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(
        biddableAuctionIds.begin(),
        biddableAuctionIds.end(),
        std::default_random_engine(seed)
    );

    logInfo("Found " + std::to_string(biddableAuctionIds.size()) + " biddable auctions");

    uint32 purchasePerCycle = GetAuctionsPerCycle();
    uint32 counter = 0;
    uint32 total = biddableAuctionIds.size();

    while(purchasePerCycle > 0 && biddableAuctionIds.size() > 0) {
        counter++;
        AuctionEntry* auction = GetAuctionForPurchase(biddableAuctionIds);

        if (auction == nullptr) {
            return;
        }

        // get our item template for this item, we need some of these details for pricing and such.
        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(auction->item_template);

        logInfo("Considering auction: "
            + itemTemplate->Name1
            + " [AuctionId: " + std::to_string(auction->Id) + "]"
            + " [ItemId: " + std::to_string(itemTemplate->ItemId) + "]"
            + " <> " + std::to_string(counter) + " of "
            + std::to_string(total)
        );

        bool success = false;

        // If this item has a buyout price, let's try to buy it. Otherwise we will
        // see if it's worth bidding on.
        if (auction->buyout > 0) {
            success = BuyoutAuction(auction, itemTemplate);
        } else {
            success = BidOnAuction(auction, itemTemplate);
        }

        if (success) {
            purchasePerCycle--;
            logInfo("Purchase made, remaining: " + std::to_string(purchasePerCycle));
        }
    }
}

AuctionEntry* AuctionatorBidder::GetAuctionForPurchase(std::vector<uint32>& auctionIds)
{
    if (auctionIds.size() == 0) {
        return nullptr;
    }

    uint32 auctionId = auctionIds[0];
    auctionIds.erase(auctionIds.begin());

    logTrace("Auction removed, remaining items: " + std::to_string(auctionIds.size()));

    AuctionEntry* auction = ahMgr->GetAuction(auctionId);
    return auction;
}

bool AuctionatorBidder::BidOnAuction(AuctionEntry* auction, ItemTemplate const* itemTemplate)
{
    uint32 currentPrice;

    // Check and see if someone has already bid on this auction. It is afterall
    // possible that a player has bid on it and we (currently) aren't in the market
    // of outbidding players. It's also possible we have bid on it and there is
    // no reason for us to bid against ourselves.
    if (auction->bid) {
        if (auction->bidder == buyerGuid) {
            logInfo("Skipping auction, I have already bid: "
                + std::to_string(auction->bid) + ".");
        } else {
            logInfo("Skipping auction, someone else has already bid "
                + std::to_string(auction->bid) + ".");
        }
        return false;
    } else {
        // nobody has bidded on this auction, so let's grab it's current bid value
        // for further scrutiny.
        currentPrice = auction->startbid;
    }

    // find out what we should really consider paying for the auction
    uint32 buyPrice = CalculateBuyPrice(auction, itemTemplate);

    // decide if our bid is less than the max amount we want to pay to avoid overpaying
    // for an item.
    if (currentPrice > buyPrice) {
        logInfo("Skipping auction ("
            + std::to_string(auction->Id) + "), price of "
            + std::to_string(currentPrice) + " is higher than template price of ("
            + std::to_string(buyPrice) + ")"
        );
        return false;
    }

    // Let's make a bid. We are going to add half the difference between the current
    // bid and the max price to the amount we bid just to try to help the seller out
    // a little bit but not overpay by TOO much.
    uint32 bidPrice = currentPrice + (buyPrice - currentPrice) / 2;

    auction->bidder = buyerGuid;
    auction->bid = bidPrice;

    // we probably shouldn't be updating the database directly here but this is what
    // i have seen the ahbot mod do so i am going with it for now.
    CharacterDatabase.Execute(R"(
            UPDATE
                auctionhouse
            SET
                buyguid = {},
                lastbid = {}.
            WHERE
                id = {}
        )",
        auction->bidder.GetCounter(),
        auction->bid,
        auction->Id
    );

    logInfo("Bid on auction of "
        + itemTemplate->Name1 + " ["
        + std::to_string(auction->Id) + "] of "
        + std::to_string(bidPrice) + " copper."
    );

    return true;
}

bool AuctionatorBidder::BuyoutAuction(AuctionEntry* auction, ItemTemplate const* itemTemplate)
{
    // let's just go ahead and find out what the max we will pay for this item is.
    uint32 buyPrice = CalculateBuyPrice(auction, itemTemplate);

    if (auction->buyout > buyPrice) {
        logInfo("Skipping buyout, price ("
            + std::to_string(auction->buyout) +") is higher than template buyprice ("
            + std::to_string(buyPrice) +")");
        return false;
    }

    // also based somewhat on what ahbot does, let's go ahead and buy this.
    auto trans = CharacterDatabase.BeginTransaction();
    // set our bidder and bid on the auction record.
    auction->bidder = buyerGuid;
    auction->bid = auction->buyout;

    // let the seller know we bought their junk.
    sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
    // delete the auction for the database so it doesn't come back on a server reload.
    auction->DeleteFromDB(trans);

    logInfo("Purchased auction of "
        + itemTemplate->Name1 + " ["
        + std::to_string(auction->Id) + "]"
        + "x" + std::to_string(auction->itemCount) + " for "
        + std::to_string(auction->buyout) + " copper."
    );

    // remove the aucton from memory. i don't fully understand why we need to do this
    // in 2 different places but i am going with it for now.
    sAuctionMgr->RemoveAItem(auction->item_guid);
    ahMgr->RemoveAuction(auction);

    // commit all these changes, however that works. thanks again ahbot.
    CharacterDatabase.CommitTransaction(trans);

    return true;
}

uint32 AuctionatorBidder::GetAuctionsPerCycle()
{
    switch(auctionHouseId) {
        case AUCTIONHOUSE_ALLIANCE:
            return config->allianceBidder.maxPerCycle;
            break;
        case AUCTIONHOUSE_HORDE:
            return config->hordeBidder.maxPerCycle;
            break;
        case AUCTIONHOUSE_NEUTRAL:
            return config->neutralBidder.maxPerCycle;
            break;
        default:
            return 0;
    }
}

uint32 AuctionatorBidder::CalculateBuyPrice(AuctionEntry* auction, ItemTemplate const* item)
{
    // get our market price for this item.
    uint32 marketPrice = 0;
    std::string query = R"(
        SELECT
            entry
            , average_price
            , scan_datetime
        FROM mod_auctionator_market_price
        WHERE entry = {}
        ORDER BY scan_datetime DESC
        LIMIT 1
    )";
    QueryResult result = CharacterDatabase.Query(query, item->ItemId);

    if (result) {
        marketPrice = result->Fetch()[1].Get<uint32>();
    }

    // get the stack size of the item.
    uint32 stackSize = 1;
    if (item->GetMaxStackSize() > 1 && auction->itemCount > 1) {
        stackSize = auction->itemCount;
    }

    // get our miltiplier configuration so we can get the right quality multiplier.
    AuctionatorPriceMultiplierConfig multiplierConfig = config->bidderMultipliers;
    uint32 quality  = item->Quality;
    float qualityMultiplier = Auctionator::GetQualityMultiplier(multiplierConfig, quality);

    // figure out if we are using our itemtemplate->BuyPrice or market price.
    uint32 price = item->BuyPrice;
    if (marketPrice > 0) {
        logInfo("Using Market over Template for bid eval [" + item->Name1 + "] " +
            std::to_string(marketPrice) + " <--> " + std::to_string(price) +
            " with multiplier of " + std::to_string(qualityMultiplier) + "x");
        price = marketPrice;
    }

    // calculate the max price we will pay for this item stack.
    return uint32(stackSize * price * qualityMultiplier);
}
