
#include "AuctionHouseMgr.h"

#ifndef AUCTIONATOR_STRUCTS_H
#define AUCTIONATOR_STRUCTS_H

struct AuctionatorEntries
{
    AuctionHouseEntry const* HordeAhEntry;
    AuctionHouseEntry const* AllianceAhEntry;
    AuctionHouseEntry const* NeutralAhEntry;
};

struct AuctionatorHouses
{
    AuctionHouseObject* HordeAh;
    AuctionHouseObject* AllianceAh;
    AuctionHouseObject* NeutralAh;
};

#endif
