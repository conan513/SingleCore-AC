
#ifndef AUCTIONATORBASE_H
#define AUCTIONATORBASE_H

#include "AuctionatorBase.h"
#include "Log.h"

class AuctionatorBase
{
    private:
        std::string logPrefix = "[Auctionator] ";
    public:
        void logDebug(std::string message) {
            std::string output = logPrefix + message;
            LOG_DEBUG("auctionator", output);
        }

        void logError(std::string message) {
            std::string output = logPrefix + message;
            LOG_ERROR("auctionator", output);
        }

        void logInfo(std::string message) {
            std::string output = logPrefix + message;
            LOG_INFO("auctionator", output);
        }

        void logTrace(std::string message) {
            std::string output = logPrefix + message;
            LOG_TRACE("auctionator", output);
        }

        void SetLogPrefix(std::string prefix)
        {
            logPrefix = prefix;
        }
};

#endif
