
#ifndef AUCTIONATORSELLER_H
#define AUCTIONATORSELLER_H

#include "Auctionator.h"
#include "AuctionHouseMgr.h"
 
class AuctionatorSeller
{
    private:
        Auctionator* nator;
        uint32 auctionHouseId;
        AuctionHouseObject* ahMgr;

    public:
        AuctionatorSeller(Auctionator* natorParam, uint32 auctionHouseIdParam);
        ~AuctionatorSeller();
        void LetsGetToIt(uint32 maxCount, uint32 houseId);
};

#endif  //AUCTIONATORSELLER_H
