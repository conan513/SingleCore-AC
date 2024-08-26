
#ifndef AUCTIONATOR_EVENTS_H
#define AUCTIONATOR_EVENTS_H

#include "AuctionatorBase.h"
#include "AuctionatorConfig.h"
#include "AuctionatorStructs.h"
#include "EventMap.h"
#include "ObjectMgr.h"
#include <unordered_map>
#include <functional>

using FunctionType = void (*)();

class AuctionatorEvents : public AuctionatorBase
{
    private:
        ObjectGuid auctionatorGuid;
        EventMap events;
        std::unordered_map<std::string, std::function<void()>> eventFunctions;
        std::unordered_map<uint32, std::string> eventToFunction;
        AuctionatorConfig* config;
        AuctionatorHouses* houses;

    public:
        AuctionatorEvents() {};
        AuctionatorEvents(AuctionatorConfig* auctionatorConfig);
        ~AuctionatorEvents();
        void InitializeEvents();
        void EventAllianceBidder();
        void EventHordeBidder();
        void EventNeutralBidder();
        void EventAllianceSeller();
        void EventHordeSeller();
        void EventNeutralSeller();
        void ExecuteEvents();
        void Update(uint32 deltaMinutes = 1);
        void SetHouses(AuctionatorHouses* auctionatorHouses);
        void SetPlayerGuid(ObjectGuid playerGuid);
        EventMap GetEvents();
};

#endif
