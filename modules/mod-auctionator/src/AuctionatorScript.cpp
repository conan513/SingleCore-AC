
#include "ScriptMgr.h"
#include "WorldSession.h"
#include "Config.h"
#include "Chat.h"
#include "Auctionator.h"
#include "Player.h"

class AuctionatorWorldScript : public WorldScript
{
public:
    AuctionatorWorldScript() : WorldScript("Auctionator_WorldScript") { }

    void OnStartup() override
    {
        LOG_INFO("server.loading", "[Auctionator]: Auctionator initializing...");
        if (gAuctionator->config->isEnabled) {
            LOG_INFO("server.loading", "[Auctionator]: Auctionator enabled.");
        } else {
            LOG_INFO("server.loading", "[Auctionator]: Auctionator disabled.");
        }
    }
};

class AuctionatorHouseScript : public AuctionHouseScript
{
    public:
        AuctionatorHouseScript() : AuctionHouseScript("AuctionatorHouseScript") {}

        void OnBeforeAuctionHouseMgrSendAuctionSuccessfulMail(
                AuctionHouseMgr*,
                AuctionEntry*,
                Player* owner,
                uint32& /*owner_accId*/,
                uint32& /*profit*/,
                bool& sendNotification,
                bool& updateAchievementCriteria,
                bool& /*sendMail*/
            ) override
        {
            if (owner && owner->GetGUID().GetCounter() == gAuctionator->config->characterGuid)
            {
                sendNotification = false;
                updateAchievementCriteria = false;
            }
        }

        void OnBeforeAuctionHouseMgrSendAuctionExpiredMail(
                AuctionHouseMgr* ,
                AuctionEntry*,
                Player* owner,
                uint32& /*owner_accId*/,
                bool& sendNotification,
                bool& /*sendMail*/
            ) override
        {
            if (owner && owner->GetGUID().GetCounter() == gAuctionator->config->characterGuid)
                sendNotification = false;
        }

        void OnBeforeAuctionHouseMgrSendAuctionOutbiddedMail(
                AuctionHouseMgr* /*auctionHouseMgr*/,
                AuctionEntry* auction,
                Player* oldBidder,
                uint32& /*oldBidder_accId*/,
                Player* newBidder,
                uint32& newPrice,
                bool& /*sendNotification*/,
                bool& /*sendMail*/
            ) override
        {
            if (oldBidder && !newBidder)
                oldBidder->GetSession()->SendAuctionBidderNotification(
                    auction->GetHouseId(),
                    auction->Id,
                    ObjectGuid::Create<HighGuid::Player>(gAuctionator->config->characterGuid),
                    newPrice,
                    auction->GetAuctionOutBid(),
                    auction->item_template);
        }

        // void OnAuctionAdd(AuctionHouseObject* /*ah*/, AuctionEntry* auction) override
        // {
        //     auctionbot->IncrementItemCounts(auction);
        // }

        // void OnAuctionRemove(AuctionHouseObject* /*ah*/, AuctionEntry* auction) override
        // {
        //     auctionbot->DecrementItemCounts(auction, auction->item_template);
        // }

        void OnBeforeAuctionHouseMgrUpdate() override
        {
            gAuctionator->Update();
        }

};


class AuctionatorMailScript : public MailScript
{
public:
    AuctionatorMailScript() : MailScript("AuctionatorMailScript") { }

    void OnBeforeMailDraftSendMailTo(MailDraft* /*mailDraft*/, MailReceiver const& receiver, MailSender const& sender, MailCheckMask& /*checked*/, uint32& /*deliver_delay*/, uint32& /*custom_expiration*/, bool& deleteMailItemsFromDB, bool& sendMail) override
    {
        if (receiver.GetPlayerGUIDLow() == gAuctionator->config->characterGuid)
        {
            if (sender.GetMailMessageType() == MAIL_AUCTION)        // auction mail with items
                deleteMailItemsFromDB = true;
            sendMail = false;
        }
    }
};

void AddAuctionatorScripts()
{
    new AuctionatorWorldScript();
    new AuctionatorHouseScript();
    new AuctionatorMailScript();
};
