
#ifndef AUCTIONATOR_H
#define AUCTIONATOR_H

#include "Common.h"
#include "ObjectGuid.h"

struct AuctionEntry;
class Player;
class WorldSession;

#include "ItemTemplate.h"
#include "AuctionatorConfig.h"
#include "AuctionHouseMgr.h"
#include "Config.h"

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
class Auctionator
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

    public:
        ~Auctionator();
        void CreateAuction(AuctionatorItem newItem, uint32 houseId);
        void ExpireAllAuctions(uint32 houseId);
        AuctionHouseEntry const *GetAuctionHouseEntry(uint32 houseId);
        AuctionHouseObject *GetAuctionHouse(uint32 houseId);
        void Initialize();
        void InitializeConfig(ConfigMgr* configMgr);
        AuctionatorConfig *config;
        void logInfo(std::string message);
        void logDebug(std::string message);
        void logTrace(std::string message);
        void Update();

        AuctionHouseObject *GetAuctionMgr(uint32 auctionHouseId);


        static Auctionator *getInstance()
        {
            static Auctionator instance;
            return &instance;
        }
};

#define gAuctionator Auctionator::getInstance()

#endif