
#ifndef AUCTIONATORSELLER_H
#define AUCTIONATORSELLER_H

#include "Auctionator.h"
#include "AuctionHouseMgr.h"

class AuctionatorSeller : public AuctionatorBase
{
    private:
        Auctionator* nator;
        uint32 auctionHouseId;
        AuctionHouseObject* ahMgr;

    public:
        AuctionatorSeller(Auctionator* natorParam, uint32 auctionHouseIdParam);
        ~AuctionatorSeller();
        void LetsGetToIt(uint32 maxCount, uint32 houseId);
        uint32 GetRandomNumber(uint32 min, uint32 max);
};

#endif  //AUCTIONATORSELLER_H
