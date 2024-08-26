
#include "Log.h"
#include "Auctionator.h"
#include "Config.h"
#include "WorldSession.h"
#include "AuctionHouseMgr.h"
#include "GameTime.h"
#include "ObjectMgr.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "AuctionatorConfig.h"
#include "AuctionatorSeller.h"
#include "AuctionatorBidder.h"
#include "AuctionatorEvents.h"
#include "AuctionatorStructs.h"
#include "EventMap.h"
#include <vector>

Auctionator::Auctionator()
{
    SetLogPrefix("[Auctionator] ");
    InitializeConfig(sConfigMgr);
    Initialize();

    logInfo("Event init");

    AuctionatorHouses* houses = new AuctionatorHouses();
    houses->HordeAh = HordeAh;
    houses->AllianceAh = AllianceAh;
    houses->NeutralAh = NeutralAh;

    ObjectGuid buyerGuid = ObjectGuid::Create<HighGuid::Player>(config->characterGuid);
    events = AuctionatorEvents(config);
    events.SetPlayerGuid(buyerGuid);
    events.SetHouses(houses);
};

Auctionator::~Auctionator()
{}

void Auctionator::CreateAuction(AuctionatorItem newItem)
{
    // will need this when we want to know details of the item for filtering
    // ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(itemId);


    Player player(session);
    player.Initialize(config->characterGuid);
    ObjectAccessor::AddObject(&player);
    uint32 houseId = newItem.houseId;

    logDebug("Creating Auction for item: " + std::to_string(newItem.itemId));
    // Create the item (and add it to the update queue for the player ")
    Item* item = Item::CreateItem(newItem.itemId, 1, &player);

    logTrace("adding item to player queue");
    item->AddToUpdateQueueOf(&player);
    uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(newItem.itemId);
    if (randomPropertyId != 0) {
        logDebug("adding random properties");
        item->SetItemRandomProperties(randomPropertyId);
    }

    // set our quantity. If this is a stack it needs to get set here
    // on the item instance and not on the auction item.
    item->SetCount(1);
    if (newItem.stackSize > 1) {
        item->SetCount(newItem.stackSize);
    }

    logTrace("starting character transaction for AH item");
    auto trans = CharacterDatabase.BeginTransaction();

    logTrace("creating auction entry");
    AuctionEntry* auctionEntry = new AuctionEntry();
    auctionEntry->Id = sObjectMgr->GenerateAuctionID();
    auctionEntry->houseId = houseId;
    auctionEntry->item_guid = item->GetGUID();
    auctionEntry->item_template = item->GetEntry();
    auctionEntry->owner = player.GetGUID();
    auctionEntry->startbid = newItem.bid;
    auctionEntry->buyout = newItem.buyout;
    auctionEntry->bid = 0;
    auctionEntry->deposit = 100;
    auctionEntry->expire_time = (time_t) newItem.time + time(NULL);
    auctionEntry->auctionHouseEntry = GetAuctionHouseEntry(houseId);;

    logTrace("save item to db");
    item->SaveToDB(trans);

    logTrace("removed from character queue");
    item->RemoveFromUpdateQueueOf(&player);

    //
    // Add the Item we are auctioning to the managers item list.
    // This is NOT faction specific, it's a global list shared by
    // all factions.
    //
    logTrace("add item to auction mgr");
    sAuctionMgr->AddAItem(item);

    //
    // Add the AuctionEntry to the correct auction house. This IS
    // faction specific and you must use the correct house for the
    // faction you want the item to show up in or it will show up
    // ... somewhere else.
    //
    logTrace("add item entry to auction house: " + std::to_string(houseId));
    GetAuctionHouse(houseId)->AddAuction(auctionEntry);

    //
    // Save your AuctionHouseEntry object to the
    // `acore_characters`.`auctionhouse` table.
    //
    logTrace("save auction entry");
    auctionEntry->SaveToDB(trans);

    logTrace("commit character transaction");
    CharacterDatabase.CommitTransaction(trans);

    ObjectAccessor::RemoveObject(&player);
}

/**
 * Use this to get access to the AuctionHouseEntry object pointer
 * for a specific auction house.
 *
 * Ultimately this is just a global singleton.
*/
AuctionHouseEntry const *Auctionator::GetAuctionHouseEntry(uint32 houseId)
{
    switch(houseId) {
        case(AUCTIONHOUSE_ALLIANCE):
            return AllianceAhEntry;
            break;
        case(AUCTIONHOUSE_HORDE):
            return HordeAhEntry;
            break;
        default:
            return NeutralAhEntry;
    }
}

/**
 * Use this to get access to the AuctionHouseObject pointer for a
 * specific auction house.
 *
 * Ultimately this is just a global singleton.
*/
AuctionHouseObject *Auctionator::GetAuctionHouse(uint32 houseId) {
    switch(houseId) {
        case(AUCTIONHOUSE_ALLIANCE):
            return AllianceAh;
            break;
        case(AUCTIONHOUSE_HORDE):
            return HordeAh;
            break;
        default:
            return NeutralAh;
    }
}

void Auctionator::Initialize()
{
    std::string accountName = "Auctionator";

    HordeAh = sAuctionMgr->GetAuctionsMapByHouseId(AUCTIONHOUSE_HORDE);
    HordeAhEntry = sAuctionHouseStore.LookupEntry(AUCTIONHOUSE_HORDE);

    AllianceAh = sAuctionMgr->GetAuctionsMapByHouseId(AUCTIONHOUSE_ALLIANCE);
    AllianceAhEntry = sAuctionHouseStore.LookupEntry(AUCTIONHOUSE_ALLIANCE);

    NeutralAh = sAuctionMgr->GetAuctionsMapByHouseId(AUCTIONHOUSE_NEUTRAL);
    NeutralAhEntry = sAuctionHouseStore.LookupEntry(AUCTIONHOUSE_NEUTRAL);

    WorldSession _session(
        config->characterId,
        std::move(accountName),
        nullptr,
        SEC_GAMEMASTER,
        sWorld->getIntConfig(CONFIG_EXPANSION),
        0,
        LOCALE_enUS,
        0,
        false,
        false,
        0
    );

    session = &_session;
}

void Auctionator::InitializeConfig(ConfigMgr* configMgr)
{
    logInfo("Initializing Auctionator Config");

    config = new AuctionatorConfig();
    config->isEnabled = configMgr->GetOption<bool>("Auctionator.Enabled", false);
    logInfo("config->isEnabled: "
        + std::to_string(config->isEnabled));

    config->characterId = configMgr->GetOption<uint32>("Auctionator.CharacterId", 0);
    config->characterGuid = configMgr->GetOption<uint32>("Auctionator.CharacterGuid", 0);
    logInfo("CharacterIds: "
        + std::to_string(config->characterId)
        + "::"
        + std::to_string(config->characterGuid)
    );

    config->hordeSeller.enabled = configMgr->GetOption<uint32>("Auctionator.HordeSeller.Enabled", 0);
    config->allianceSeller.enabled = configMgr->GetOption<uint32>("Auctionator.AllianceSeller.Enabled", 0);
    config->neutralSeller.enabled = configMgr->GetOption<uint32>("Auctionator.NeutralSeller.Enabled", 0);

    config->hordeSeller.maxAuctions = configMgr->GetOption<uint32>("Auctionator.HordeSeller.MaxAuctions", 50);
    config->allianceSeller.maxAuctions = configMgr->GetOption<uint32>("Auctionator.AllianceSeller.MaxAuctions", 50);
    config->neutralSeller.maxAuctions = configMgr->GetOption<uint32>("Auctionator.NeutralSeller.MaxAuctions", 50);

    // Load our seller configurations
    config->sellerConfig.auctionsPerRun = configMgr->GetOption<uint32>("Auctionator.Seller.AuctionsPerRun", 100);
    config->sellerConfig.defaultPrice = configMgr->GetOption<uint32>("Auctionator.Seller.DefaultPrice", 10000000);
    config->sellerConfig.queryLimit = 
        configMgr->GetOption<uint32>("Auctionator.Seller.QueryLimit", config->sellerConfig.auctionsPerRun);
    config->sellerConfig.randomizeStackSize = configMgr->GetOption<uint32>("Auctionator.Seller.RandomizeStackSize", 1);
    config->sellerConfig.bidStartModifier = configMgr->GetOption<float>("Auctionator.Seller.BidStartModifier", 1.0f);

    // Load our bidder configurations
    config->allianceBidder.enabled = configMgr->GetOption<uint32>("Auctionator.AllianceBidder.Enabled", 0);
    config->allianceBidder.cycleMinutes = configMgr->GetOption<uint32>("Auctionator.AllianceBidder.CycleMinutes", 30);
    config->allianceBidder.maxPerCycle = configMgr->GetOption<uint32>("Auctionator.AllianceBidder.MaxPerCycle", 1);

    config->hordeBidder.enabled = configMgr->GetOption<uint32>("Auctionator.HordeBidder.Enabled", 0);
    config->hordeBidder.cycleMinutes = configMgr->GetOption<uint32>("Auctionator.HordeBidder.CycleMinutes", 30);
    config->hordeBidder.maxPerCycle = configMgr->GetOption<uint32>("Auctionator.HordeBidder.MaxPerCycle", 1);

    config->neutralBidder.enabled = configMgr->GetOption<uint32>("Auctionator.NeutralBidder.Enabled", 0);
    config->neutralBidder.cycleMinutes = configMgr->GetOption<uint32>("Auctionator.NeutralBidder.CycleMinutes", 30);
    config->neutralBidder.maxPerCycle = configMgr->GetOption<uint32>("Auctionator.NeutralBidder.MaxPerCycle", 1);

    config->bidOnOwn = configMgr->GetOption<uint32>("Auctionator.Bidder.BidOnOwn", 0);

    // load out multipliers for seller prices
    config->sellerMultipliers.poor
        = configMgr->GetOption<float>("Auctionator.Multipliers.Seller.Poor", 1.0f);
    config->sellerMultipliers.normal
        = configMgr->GetOption<float>("Auctionator.Multipliers.Seller.Normal", 1.0f);
    config->sellerMultipliers.uncommon
        = configMgr->GetOption<float>("Auctionator.Multipliers.Seller.Uncommon", 1.5f);
    config->sellerMultipliers.rare
        = configMgr->GetOption<float>("Auctionator.Multipliers.Seller.Rare", 2.0f);
    config->sellerMultipliers.epic
        = configMgr->GetOption<float>("Auctionator.Multipliers.Seller.Epic", 6.0f);
    config->sellerMultipliers.legendary
        = configMgr->GetOption<float>("Auctionator.Multipliers.Seller.Legendary", 10.0f);

    // load out multipliers for bidder prices
    config->bidderMultipliers.poor
        = configMgr->GetOption<float>("Auctionator.Multipliers.Bidder.Poor", 1.0f);
    config->bidderMultipliers.normal
        = configMgr->GetOption<float>("Auctionator.Multipliers.Bidder.Normal", 1.0f);
    config->bidderMultipliers.uncommon
        = configMgr->GetOption<float>("Auctionator.Multipliers.Bidder.Uncommon", 1.5f);
    config->bidderMultipliers.rare
        = configMgr->GetOption<float>("Auctionator.Multipliers.Bidder.Rare", 2.0f);
    config->bidderMultipliers.epic
        = configMgr->GetOption<float>("Auctionator.Multipliers.Bidder.Epic", 6.0f);
    config->bidderMultipliers.legendary
        = configMgr->GetOption<float>("Auctionator.Multipliers.Bidder.Legendary", 10.0f);

    logInfo("Auctionator config initialized");
}

/**
 * Update gets called on each "tick" of the global sAuctionHouseManager.
*/
void Auctionator::Update()
{
    logInfo("- - - - - - - - - - - - - - - - - - - -");

    logInfo("Neutral count: " + std::to_string(NeutralAh->Getcount()));
    logInfo("Alliance count: " + std::to_string(AllianceAh->Getcount()));
    logInfo("Horde count: " + std::to_string(HordeAh->Getcount()));

/*
    if (config->allianceSeller.enabled) {
        AuctionatorSeller sellerAlliance =
            AuctionatorSeller(gAuctionator, static_cast<uint32>(AUCTIONHOUSE_ALLIANCE));

        uint32 auctionCountAlliance = AllianceAh->Getcount();

        if (auctionCountAlliance <= config->allianceSeller.maxAuctions) {
            logInfo(
                "Alliance count is good, here we go: "
                + std::to_string(auctionCountAlliance)
                + " of " + std::to_string(config->allianceSeller.maxAuctions)
            );

            sellerAlliance.LetsGetToIt(100, AUCTIONHOUSE_ALLIANCE);
        } else {
            logInfo("Alliance count over max: " + std::to_string(auctionCountAlliance));
        }
    } else {
        logInfo("Alliance Seller Disabled");
    }

    if (config->hordeSeller.enabled) {
        AuctionatorSeller sellerHorde =
            AuctionatorSeller(gAuctionator, static_cast<uint32>(AUCTIONHOUSE_HORDE));

        uint32 auctionCountHorde = HordeAh->Getcount();

        if (auctionCountHorde <= config->hordeSeller.maxAuctions) {
            logInfo(
                "Horde count is good, here we go: "
                + std::to_string(auctionCountHorde)
                + " of " + std::to_string(config->hordeSeller.maxAuctions)
            );

            sellerHorde.LetsGetToIt(100, AUCTIONHOUSE_HORDE);
        } else {
            logInfo("Horde count over max: " + std::to_string(auctionCountHorde));
        }
    } else {
        logInfo("Horde Seller Disabled");
    }

    if (config->neutralSeller.enabled) {
        AuctionatorSeller sellerNeutral =
            AuctionatorSeller(gAuctionator, static_cast<uint32>(AUCTIONHOUSE_NEUTRAL));

        uint32 auctionCountNeutral = NeutralAh->Getcount();

        if (auctionCountNeutral <= config->neutralSeller.maxAuctions) {
            logInfo(
                "Neutral count is good, here we go: "
                + std::to_string(auctionCountNeutral)
                + " of " + std::to_string(config->neutralSeller.maxAuctions)
            );

            sellerNeutral.LetsGetToIt(100, AUCTIONHOUSE_NEUTRAL);
        } else {
            logInfo("Neutral count over max: " + std::to_string(auctionCountNeutral));
        }
    } else {
        logInfo("Neutral Seller Disabled");
    }
*/

    logInfo("UpdatingEvents");
    events.Update(1);

    logInfo("^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^");
}

AuctionHouseObject* Auctionator::GetAuctionMgr(uint32 auctionHouseId)
{
    switch(auctionHouseId) {
        case AUCTIONHOUSE_ALLIANCE:
            return AllianceAh;
            break;
        case AUCTIONHOUSE_HORDE:
            return HordeAh;
            break;
        default:
            return NeutralAh;
            break;
    }
}

void Auctionator::ExpireAllAuctions(uint32 houseId)
{
    if (houseId != AUCTIONHOUSE_ALLIANCE &&
        houseId != AUCTIONHOUSE_HORDE &&
        houseId != AUCTIONHOUSE_NEUTRAL
    ) {
        logDebug("Invalid houseId: " + std::to_string(houseId));
        return;
    }

    logDebug("Clearing auctions for houseId: " + std::to_string(houseId));
    //
    // we are going to grab our auctions map from the matching house
    // and iterate over it setting the expire_time for each auction to
    // 0. This will force the AHmgr to delete all of these auctions BUT
    // it won't happen till after the next tick of Auctionator passes
    // because our update happens before the server gets a chance to update.
    //
    AuctionHouseObject* ah = sAuctionMgr->GetAuctionsMapByHouseId(houseId);
    for (
        AuctionHouseObject::AuctionEntryMap::iterator itr,
        iter = ah->GetAuctionsBegin();
        iter != ah->GetAuctionsEnd();
        )
    {
        itr = iter++;
        AuctionEntry* auction = (*itr).second;
        logTrace("Expiring auction " + std::to_string(auction->Id) +
            " for house " + std::to_string(auction->houseId));
        auction->expire_time = 0;
    }

    logDebug("House auctions expired: " + std::to_string(houseId));
}

float Auctionator::GetQualityMultiplier(AuctionatorPriceMultiplierConfig config, uint32 quality)
{
    switch(quality) {
        case ITEM_QUALITY_POOR:
            return config.poor;
        case ITEM_QUALITY_NORMAL:
            return config.normal;
        case ITEM_QUALITY_UNCOMMON:
            return config.uncommon;
        case ITEM_QUALITY_RARE:
            return config.rare;
        case ITEM_QUALITY_EPIC:
            return config.epic;
        case ITEM_QUALITY_LEGENDARY:
            return config.legendary;
        default:
            return 1;
    }
}
