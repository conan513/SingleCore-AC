
#ifndef AUCTIONATOR_CONFIG_H
#define AUCTIONATOR_CONFIG_H

#include "Common.h"

struct AuctionatorHouseConfig
{
    public:
        uint32 enabled = 0;
        uint32 maxAuctions = 100;
};

struct AuctionatorBidderConfig
{
    public:
        uint32 enabled;
        uint32 cycleMinutes;
        uint32 maxPerCycle;
};

struct AuctionatorPriceMultiplierConfig
{
    public:
        float poor = 1.0f;
        float normal = 1.0f;
        float uncommon = 1.5f;
        float rare = 2.0f;
        float epic = 6.0f;
        float legendary = 10.0f;
};

struct AuctionatorSellerConfig
{
    public:
        uint32 queryLimit = 1000;
        uint32 defaultPrice = 10000000;
        uint32 auctionsPerRun = 100;
        uint32 randomizeStackSize = 1;
        float bidStartModifier = .3;
};

class AuctionatorConfig
{
    private:

    public:
        AuctionatorConfig() {};
        bool isEnabled = false;
        uint32 characterId = 0;
        uint32 characterGuid = 0;
        uint32 auctionHouseId = 7;

        AuctionatorHouseConfig hordeSeller;
        AuctionatorHouseConfig allianceSeller;
        AuctionatorHouseConfig neutralSeller;

        AuctionatorBidderConfig allianceBidder;
        AuctionatorBidderConfig hordeBidder;
        AuctionatorBidderConfig neutralBidder;

        AuctionatorPriceMultiplierConfig sellerMultipliers;
        AuctionatorPriceMultiplierConfig bidderMultipliers;

        AuctionatorSellerConfig sellerConfig;

        uint32 bidOnOwn = 0;
};

#endif
