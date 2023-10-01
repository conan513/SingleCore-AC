
#include <iostream>
#include <string>
#include "ScriptMgr.h"
#include "Chat.h"
#include "Auctionator.h"

using namespace Acore::ChatCommands;

class AuctionatorCommands : public CommandScript
{
    public:
        AuctionatorCommands() : CommandScript("AuctionatorCommandScript")
        {
        }

    private:
        std::vector<ChatCommand> GetCommands() const override
        {
            return std::vector<ChatCommand>
            {
               {
                  "auctionator",
                  SEC_GAMEMASTER,
                  true,
                  &HandleCommandOptions,
                  ""
               }
            };
        }

        static bool HandleCommandOptions(ChatHandler* handler, const char* args)
        {
            gAuctionator->logInfo("Executing Auctionator!");

            const char* command = strtok((char*)args, " ");

            if(!command)
            {
                command = "help"; 
            }

            const char* param1 = strtok(NULL, " ");
            const char* param2 = strtok(NULL, " ");
            const char* param3 = strtok(NULL, " ");
            const char* param4 = strtok(NULL, " ");

            if(strlen(command) == 0) {
                return true;
            }

            std::string commandString(command);

            gAuctionator->logDebug("Executing command: " + commandString);

            if (commandString.compare("add") == 0) {
                gAuctionator->logInfo("Adding new Item for GM");
                uint32 auctionHouseId = std::stoi(param1);
                uint32 itemId = std::stoi(param2);
                uint32 price = std::stoi(param3);
                AuctionatorCommands::AddItemForBuyout(auctionHouseId, itemId, price);
            } else if (commandString == "expireall") {
                if (!param1) {
                    handler->SendSysMessage("[Auctionator] expireall: No Auction House Specified!");
                    gAuctionator->logInfo("expireall: No Auction House Specified!");
                    return true;
                }
                uint32 houseId = std::stoi(param1); 
                
                handler->SendSysMessage("[Auctionator] Expiring all Auctions for house: " + std::to_string(houseId));
                gAuctionator->logInfo("expireall: Expiring all auctions for house: " + std::to_string(houseId));

                gAuctionator->ExpireAllAuctions(houseId);

                gAuctionator->logInfo("expireall: All auctions expired for house: " + std::to_string(houseId));
            } else if (commandString == "enable") {
                if (!param1) {
                    handler->SendSysMessage("[Auctionator] enable: No Auction House Specified!");
                    gAuctionator->logInfo("enable: No Auction House Specified!");
                    return true;
                }

                std::string toEnable(param1);
                if (toEnable == "hordeseller") {
                    gAuctionator->config->hordeSeller.enabled = 1;
                    gAuctionator->logInfo("Horde seller enabled");
                    return true;
                } else if (toEnable == "allianceseller") {
                    gAuctionator->config->allianceSeller.enabled = 1;
                    gAuctionator->logInfo("Alliance seller enabled");
                    return true;
                } else if (toEnable == "neutralseller") {
                    gAuctionator->config->neutralSeller.enabled = 1;
                    gAuctionator->logInfo("Neutral seller enabled");
                    return true;
                } else if (toEnable == "all") {
                    gAuctionator->config->hordeSeller.enabled = 1;
                    gAuctionator->config->allianceSeller.enabled = 1;
                    gAuctionator->config->neutralSeller.enabled = 1;
                    gAuctionator->logInfo("All sellers enabled");
                    return true;
                }
            } else if (commandString == "disable") {
                if (!param1) {
                    handler->SendSysMessage("[Auctionator] disable: No Auction House Specified!");
                    gAuctionator->logInfo("disable: No Auction House Specified!");
                    return true;
                }

                std::string toEnable(param1);
                if (toEnable == "hordeseller") {
                    gAuctionator->config->hordeSeller.enabled = 0;
                    gAuctionator->logInfo("Horde seller disabled");
                    return true;
                } else if (toEnable == "allianceseller") {
                    gAuctionator->config->allianceSeller.enabled = 0;
                    gAuctionator->logInfo("Alliance seller disabled");
                    return true;
                } else if (toEnable == "neutralseller") {
                    gAuctionator->config->neutralSeller.enabled = 0;
                    gAuctionator->logInfo("Neutral seller disabled");
                    return true;
                } else if (toEnable == "all") {
                    gAuctionator->config->hordeSeller.enabled = 0;
                    gAuctionator->config->allianceSeller.enabled = 0;
                    gAuctionator->config->neutralSeller.enabled = 0;
                    gAuctionator->logInfo("All sellers disabled");
                    return true;
                }
            } else if (commandString == "status") {
                AuctionatorCommands::ShowStatus(handler);
            } else if (commandString == "help") {
                AuctionatorCommands::ShowHelp(handler);
                return true;
            }


            return true;
        }

        static void AddItemForBuyout(uint32 auctionHouseId, uint32 itemId, uint32 price)
        {
            AuctionatorItem newItem;
            newItem.houseId = auctionHouseId;
            newItem.itemId = itemId;
            newItem.buyout = price;

            gAuctionator->CreateAuction(newItem, AUCTIONHOUSE_NEUTRAL);
        } 

        static void ShowHelp(ChatHandler* handler)
        {
            std::string helpString(R"(
Auctionator Help:
disable ...
enable ...
add ...
expireall ...
status
help
            )");
            handler->SendSysMessage(helpString);
        }

        static void ShowStatus(ChatHandler* handler)
        {
            std::string statusString = "[Auctionator] Status:\n\n";

            statusString += " Enabled: " + std::to_string(gAuctionator->config->isEnabled) + "\n\n";

            statusString += " Horde:\n";
            statusString += "  Enabled: " + std::to_string(gAuctionator->config->hordeSeller.enabled) + "\n";
            statusString += "  Max Auctions: " + std::to_string(gAuctionator->config->hordeSeller.maxAuctions) + "\n";
            statusString += "  Auctions: " + std::to_string(gAuctionator->GetAuctionHouse(AUCTIONHOUSE_HORDE)->Getcount()) + "\n";

            statusString += " Alliance:\n";
            statusString += "  Enabled: " + std::to_string(gAuctionator->config->allianceSeller.enabled) + "\n";
            statusString += "  Max Auctions: " + std::to_string(gAuctionator->config->allianceSeller.maxAuctions) + "\n";
            statusString += "  Auctions: " + std::to_string(gAuctionator->GetAuctionHouse(AUCTIONHOUSE_ALLIANCE)->Getcount()) + "\n";

            statusString += " Neutral:\n";
            statusString += "  Enabled: " + std::to_string(gAuctionator->config->neutralSeller.enabled) + "\n";
            statusString += "  Max Auctions: " + std::to_string(gAuctionator->config->neutralSeller.maxAuctions) + "\n";
            statusString += "  Auctions: " + std::to_string(gAuctionator->GetAuctionHouse(AUCTIONHOUSE_NEUTRAL)->Getcount()) + "\n";

            handler->SendSysMessage(statusString);
        }
};

void AddAuctionatorCommands()
{
    new AuctionatorCommands();
}
