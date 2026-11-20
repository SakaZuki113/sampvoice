#include "StreamAtPlayer.h"

#include <audio/bass.h>
#include <samp/CNetGame.h>
#include <game/CPed.h>
#include <game/CCamera.h>

#include "StreamInfo.h"

StreamAtPlayer::StreamAtPlayer(const D3DCOLOR color, std::string name,
                               const float distance, const WORD playerId) noexcept
    : LocalStream(StreamType::LocalStreamAtPlayer, color, std::move(name), distance)
    , playerId(playerId)
{}

void StreamAtPlayer::Tick() noexcept
{
    this->LocalStream::Tick();

    const auto pNetGame = SAMP::pNetGame();
    if (!pNetGame) return;

    const auto pPlayerPool = pNetGame->GetPlayerPool();
    if (!pPlayerPool) return;

    const auto pPlayer = pPlayerPool->GetPlayer(this->playerId);
    if (!pPlayer) return;

    const auto pPlayerPed = pPlayer->GetPed();
    if (!pPlayerPed) return;

    const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
    if (!pPlayerGamePed) return;

    const auto pPlayerMatrix = pPlayerGamePed->GetMatrix();
    if (!pPlayerMatrix) return;

    for (const auto& channel : this->GetChannels())
    {
        if (channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix->pos),
                nullptr, nullptr);
        }
    }
}

void StreamAtPlayer::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    BASS_3DVECTOR initPos { 0, 0, 0 };
    bool posFound = false;

    const auto pNetGame = SAMP::pNetGame();
    if (pNetGame)
    {
        const auto pPlayerPool = pNetGame->GetPlayerPool();
        if (pPlayerPool)
        {
            const auto pPlayer = pPlayerPool->GetPlayer(this->playerId);
            if (pPlayer)
            {
                const auto pPlayerPed = pPlayer->GetPed();
                if (pPlayerPed)
                {
                    const auto pPlayerGamePed = pPlayerPed->m_pGamePed;
                    if (pPlayerGamePed)
                    {
                        const auto pPlayerMatrix = pPlayerGamePed->GetMatrix();
                        if (pPlayerMatrix)
                        {
                            initPos = *reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix->pos);
                            posFound = true;
                        }
                    }
                }
            }
        }
    }

    if (!posFound)
    {
        initPos = *reinterpret_cast<const BASS_3DVECTOR*>(&TheCamera.GetPosition());
    }

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        &initPos,
        &kZeroVector, &kZeroVector);
}
