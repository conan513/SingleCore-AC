
#ifndef AUCTIONATOR_H
#define AUCTIONATOR_H

#include "Common.h"
#include "ObjectGuid.h"
#include "ItemTemplate.h"
#include "AuctionatorConfig.h"
#include "AuctionHouseMgr.h"
#include "Config.h"
#include "EventMap.h"
#include "AuctionatorEvents.h"
#include "AuctionatorBase.h"

struct AuctionatorItem
{
    uint32 itemId;
    uint32 houseId;
    uint32 bid;
    uint32 buyout;
    uint32 time;
    uint32 quantity;
    uint32 stackSize;

    AuctionatorItem() :
        itemId(0),
        houseId(AUCTIONHOUSE_NEUTRAL),
        bid(0),
        buyout(0),
        time(172800),
        quantity(1),
        stackSize(1) {}
};

class Auctionator : public AuctionatorBase
{
    private:
        Auctionator();
        AuctionHouseObject* HordeAh;
        AuctionHouseObject* AllianceAh;
        AuctionHouseObject* NeutralAh;
        AuctionHouseEntry const* HordeAhEntry;
        AuctionHouseEntry const* AllianceAhEntry;
        AuctionHouseEntry const* NeutralAhEntry;
        WorldSession *session;
        AuctionatorEvents events;

    public:
        ~Auctionator();
        void CreateAuction(AuctionatorItem newItem);
        void ExpireAllAuctions(uint32 houseId);
        AuctionHouseEntry const *GetAuctionHouseEntry(uint32 houseId);
        AuctionHouseObject *GetAuctionHouse(uint32 houseId);
        void Initialize();
        void InitializeConfig(ConfigMgr* configMgr);
        AuctionatorConfig *config;
        void Update();

        AuctionHouseObject *GetAuctionMgr(uint32 auctionHouseId);
        static float GetQualityMultiplier(AuctionatorPriceMultiplierConfig config, uint32 quality);

        static Auctionator *getInstance()
        {
            static Auctionator instance;
            return &instance;
        }
};

// I am doing this because when i started this project (and likely still)
// i sucked at c++ and just copied what i saw in the rest of tcore/acore.
// But. Don't do this global singleton bullsh!t. It's really bad and it
// makes this codebase impossible to do any real unit testing on. 
// TODO: figure out how to get rid of this really bad sh!t.
#define gAuctionator Auctionator::getInstance()

#endif
