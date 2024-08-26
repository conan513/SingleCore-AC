
#include "AuctionatorEvents.h"
#include "AuctionatorBidder.h"
#include "AuctionatorSeller.h"
#include "Log.h"
#include <functional>

AuctionatorEvents::AuctionatorEvents(AuctionatorConfig* auctionatorConfig)
{
    SetLogPrefix("[AuctionatorEvents] ");
    events = EventMap();
    config = auctionatorConfig;
    InitializeEvents();
}

AuctionatorEvents::~AuctionatorEvents()
{}

void AuctionatorEvents::InitializeEvents()
{
    logInfo("Initializing events");

    // register the functions for each of our events
    // these strings are mapped to the events below
    eventFunctions["AllianceBidder"] =
        std::bind(&AuctionatorEvents::EventAllianceBidder, this);
    eventFunctions["HordeBidder"] =
        std::bind(&AuctionatorEvents::EventHordeBidder, this);
    eventFunctions["NeutralBidder"] =
        std::bind(&AuctionatorEvents::EventNeutralBidder, this);

    // map our event functions from above to our events below
    eventToFunction = {
            {1, "AllianceBidder"},
            {2, "HordeBidder"},
            {3, "NeutralBidder"},
            {4, "AllianceSeller"},
            {5, "HordeSeller"},
            {6, "NeutralSeller"}
        };
    //
    // schedule the events for our bidders
    //

    // AllianceBidder
    if (config->allianceBidder.enabled) {
        events.ScheduleEvent(1, config->allianceBidder.cycleMinutes);
    }
    // HordeBidder
    if (config->hordeBidder.enabled) {
        events.ScheduleEvent(2, config->hordeBidder.cycleMinutes);
    }
    // NeutralBidder
    if (config->neutralBidder.enabled) {
        events.ScheduleEvent(3, config->neutralBidder.cycleMinutes);
    }

    if (config->allianceSeller.enabled) {
        events.ScheduleEvent(4, 1);
    }

    if (config->hordeSeller.enabled) {
        events.ScheduleEvent(5, 1);
    }

    if (config->neutralSeller.enabled) {
        events.ScheduleEvent(6, 1);
    }
}

void AuctionatorEvents::ExecuteEvents()
{
    logInfo("Executing events");
    uint32 currentEvent = events.ExecuteEvent();
    while (currentEvent != 0) {
        logInfo("Executing event: " + eventToFunction[currentEvent]);

        if (eventToFunction[currentEvent] != "") {
            try {
                // this shit stopped working and I have no idea why
                // so until i revisit we will do it the hard way below.
                // eventFunctions[eventToFunction[currentEvent]]();

                switch(currentEvent) {
                    case 1:
                        EventAllianceBidder();
                        if (config->allianceBidder.enabled) {
                            events.ScheduleEvent(currentEvent, config->allianceBidder.cycleMinutes);
                        }
                        break;
                    case 2:
                        EventHordeBidder();
                        if (config->hordeBidder.enabled) {
                            events.ScheduleEvent(currentEvent, config->hordeBidder.cycleMinutes);
                        }
                        break;
                    case 3:
                        EventNeutralBidder();
                        if (config->neutralBidder.enabled) {
                            events.ScheduleEvent(currentEvent, config->neutralBidder.cycleMinutes);
                        }
                        break;
                    case 4:
                        EventAllianceSeller();
                        if (config->allianceSeller.enabled) {
                            events.ScheduleEvent(currentEvent, 1);
                        }
                        break;
                    case 5:
                        EventHordeSeller();
                        if (config->hordeSeller.enabled) {
                            events.ScheduleEvent(currentEvent, 1);
                        }
                        break;
                    case 6:
                        EventNeutralSeller();
                        if (config->neutralSeller.enabled) {
                            events.ScheduleEvent(currentEvent, 1);
                        }
                        break;
                }
            } catch(const std::exception& e) {
                logError("Issue calling handler");
                logError(e.what());
            }
        }

        currentEvent = events.ExecuteEvent();
    }
}

void AuctionatorEvents::Update(uint32 deltaMinutes)
{
    events.Update(deltaMinutes);
    ExecuteEvents();
}

void AuctionatorEvents::SetPlayerGuid(ObjectGuid playerGuid)
{
    auctionatorGuid = playerGuid;
}

void AuctionatorEvents::SetHouses(AuctionatorHouses* auctionatorHouses)
{
    houses = auctionatorHouses;
}

void AuctionatorEvents::EventAllianceBidder()
{
    logInfo("Starting Alliance Bidder");
    AuctionatorBidder bidder = AuctionatorBidder(AUCTIONHOUSE_ALLIANCE, auctionatorGuid, config);
    bidder.SpendSomeCash();
}

void AuctionatorEvents::EventHordeBidder()
{
    logInfo("Starting Horde Bidder");
    AuctionatorBidder bidder = AuctionatorBidder(AUCTIONHOUSE_HORDE, auctionatorGuid, config);
    bidder.SpendSomeCash();
}

void AuctionatorEvents::EventNeutralBidder()
{
    logInfo("Starting Neutral Bidder");
    AuctionatorBidder bidder = AuctionatorBidder(AUCTIONHOUSE_NEUTRAL, auctionatorGuid, config);
    bidder.SpendSomeCash();
}

void AuctionatorEvents::EventAllianceSeller()
{
    AuctionatorSeller sellerAlliance =
        AuctionatorSeller(gAuctionator, static_cast<uint32>(AUCTIONHOUSE_ALLIANCE));

    uint32 auctionCountAlliance = houses->AllianceAh->Getcount();

    if (auctionCountAlliance <= config->allianceSeller.maxAuctions) {
        logInfo(
            "Alliance count is good, here we go: "
            + std::to_string(auctionCountAlliance)
            + " of " + std::to_string(config->allianceSeller.maxAuctions)
        );

        sellerAlliance.LetsGetToIt(
            config->sellerConfig.auctionsPerRun,
            AUCTIONHOUSE_ALLIANCE
        );
    } else {
        logInfo("Alliance count over max: " + std::to_string(auctionCountAlliance));
    }
    
}

void AuctionatorEvents::EventHordeSeller()
{
    AuctionatorSeller sellerHorde =
        AuctionatorSeller(gAuctionator, static_cast<uint32>(AUCTIONHOUSE_HORDE));

    uint32 auctionCountHorde = houses->HordeAh->Getcount();

    if (auctionCountHorde <= config->hordeSeller.maxAuctions) {
        logInfo(
            "Horde count is good, here we go: "
            + std::to_string(auctionCountHorde)
            + " of " + std::to_string(config->hordeSeller.maxAuctions)
        );

        sellerHorde.LetsGetToIt(
            config->sellerConfig.auctionsPerRun,
            AUCTIONHOUSE_HORDE
        );
    } else {
        logInfo("Horde count over max: " + std::to_string(auctionCountHorde));
    }
}

void AuctionatorEvents::EventNeutralSeller()
{
    AuctionatorSeller sellerNeutral =
        AuctionatorSeller(gAuctionator, static_cast<uint32>(AUCTIONHOUSE_NEUTRAL));

    uint32 auctionCountNeutral = houses->NeutralAh->Getcount();

    if (auctionCountNeutral <= config->neutralSeller.maxAuctions) {
        logInfo(
            "Neutral count is good, here we go: "
            + std::to_string(auctionCountNeutral)
            + " of " + std::to_string(config->neutralSeller.maxAuctions)
        );

        sellerNeutral.LetsGetToIt(
            config->sellerConfig.auctionsPerRun,
            AUCTIONHOUSE_NEUTRAL
        );

    } else {
        logInfo("Neutral count over max: " + std::to_string(auctionCountNeutral));
    }
}

EventMap AuctionatorEvents::GetEvents()
{
    return events;
}
