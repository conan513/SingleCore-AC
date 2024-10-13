/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "BreakingNews.h"

bool TryReadFile(std::string& path, std::string& bn_Result)
{
    std::ifstream bn_File(path);

    std::string bn_Buffer = "";
    bn_Result = "";

    if (!bn_File.is_open())
    {
        return false;
    }

    while (std::getline(bn_File, bn_Buffer))
    {
        bn_Result = bn_Result + (bn_Buffer);
    }

    bn_Result.erase(std::remove(bn_Result.begin(), bn_Result.end(), '\r'), bn_Result.cend());
    bn_Result.erase(std::remove(bn_Result.begin(), bn_Result.end(), '\n'), bn_Result.cend());

    return true;
}

bool TryReadNews(std::string& bn_Result)
{
    std::string path = sConfigMgr->GetOption<std::string>("BreakingNews.HtmlPath", "./Updates.html");
    bn_Title = sConfigMgr->GetOption<std::string>("BreakingNews.Title", "Breaking News");

    if (path == "")
    {
        LOG_ERROR("module", "Failed to read 'BreakingNews.HtmlPath'.");
        return false;
    }

    if (!TryReadFile(path, bn_Result))
    {
        LOG_ERROR("module", "Failed to read file '{}'.", path);
        return false;
    }

    return true;
}

std::vector<std::string> BreakingNewsServerScript::GetChunks(std::string s, uint8_t chunkSize)
{
    std::vector<std::string> chunks;

    for (uint32_t i = 0; i < s.size(); i += chunkSize)
    {
        chunks.push_back(s.substr(i, chunkSize));
    }

    return chunks;
}

void BreakingNewsServerScript::SendChunkedPayload(Warden* warden, WardenPayloadMgr* payloadMgr, std::string payload, uint32 chunkSize)
{
    bool verbose = sConfigMgr->GetOption<bool>("BreakingNews.Verbose", false);

    auto chunks = GetChunks(payload, chunkSize);

    if (!payloadMgr->GetPayloadById(_prePayloadId))
    {
        payloadMgr->RegisterPayload(_prePayload, _prePayloadId);
    }

    payloadMgr->QueuePayload(_prePayloadId);
    warden->ForceChecks();

    if (verbose)
    {
        LOG_INFO("module", "Sent pre-payload '{}'.", _prePayload);
    }

    for (auto const& chunk : chunks)
    {
        auto smallPayload = "wlbuf = wlbuf .. [[" + chunk + "]];";
    
        payloadMgr->RegisterPayload(smallPayload, _tmpPayloadId, true);
        payloadMgr->QueuePayload(_tmpPayloadId);
        warden->ForceChecks();

        if (verbose)
        {
            LOG_INFO("module", "Sent mid-payload '{}'.", smallPayload);
        }
    }

    if (!payloadMgr->GetPayloadById(_postPayloadId))
    {
        payloadMgr->RegisterPayload(_postPayload, _postPayloadId);
    }

    payloadMgr->QueuePayload(_postPayloadId);
    warden->ForceChecks();

    if (verbose)
    {
        LOG_INFO("module", "Sent post-payload '{}'.", _postPayload);
    }
}

void LoadBreakingNews()
{
    bn_Title = sConfigMgr->GetOption<std::string>("BreakingNews.Title", "Breaking News");

    if (!TryReadNews(bn_Body))
    {
        LOG_ERROR("module", "Failed to read breaking news.");
        return;
    }

    bn_Formatted = Acore::StringFormat(_midPayloadFmt, bn_Title, bn_Body);
}

bool BreakingNewsServerScript::CanPacketSend(WorldSession* session, WorldPacket& packet)
{
    if (!bn_Enabled)
    {
        return true;
    }

    if (packet.GetOpcode() == SMSG_CHAR_ENUM)
    {
        WardenWin* warden = (WardenWin*)session->GetWarden();
        if (!warden)
        {
            return true;
        }

        // Trying to use Warden before it has initialized,
        // so we exit.
        if (!warden->IsInitialized())
        {
            return true;
        }

        if (bn_Formatted == "")
        {
            return true;
        }


        auto payloadMgr = warden->GetPayloadMgr();
        if (!payloadMgr)
        {
            return true;
        }

        // Just in-case there are some payloads in the queue, we don't want to send the incorrect payload.
        payloadMgr->ClearQueuedPayloads();

        // Load in the updated news into the cache.
        if (!sConfigMgr->GetOption<bool>("BreakingNews.Cache", false))
        {
            LoadBreakingNews();
        }

        // The client truncates warden packets to around 256 and our payload may be larger than that.
        SendChunkedPayload(warden, payloadMgr, bn_Formatted, 128);
    }

    return true;
}

void BreakingNewsWorldScript::OnAfterConfigLoad(bool /*reload*/)
{
    bn_Enabled = sConfigMgr->GetOption<bool>("BreakingNews.Enable", false);

    if (!bn_Enabled)
    {
        return;
    }

    LoadBreakingNews();
}

// Add all scripts in one
void AddBreakingNewsScripts()
{
    new BreakingNewsWorldScript();
    new BreakingNewsServerScript();
}
