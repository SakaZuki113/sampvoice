/*
    This is a SampVoice project file
    Author: CyberMor <cyber.mor.2020@gmail.ru>
    open.mp version author: AmyrAhmady (iAmir) <hhm6@yahoo.com>

    See more here https://github.com/AmyrAhmady/sampvoice
    Original repository: https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "KeyFilter.h"

#include "Logger.h"

bool KeyFilter::AddKey(const BYTE keyId, const bool isPushToTalk) noexcept
{
    if (KeyFilter::statusKeys[keyId])
        return false;

    Logger::LogToFile("[dbg:keyfilter] : adding key (id:0x%hhx, ptt:%d)...", keyId, isPushToTalk);

    KeyFilter::statusKeys[keyId] = true;
    KeyFilter::pttKeys[keyId] = isPushToTalk;

    return true;
}

bool KeyFilter::RemoveKey(const BYTE keyId) noexcept
{
    KeyFilter::PushReleaseEvent(keyId);

    if (!KeyFilter::statusKeys[keyId])
        return false;

    Logger::LogToFile("[dbg:keyfilter] : removing key (id:0x%hhx)...", keyId);

    KeyFilter::statusKeys[keyId] = false;
    KeyFilter::pttKeys[keyId] = false;

    return true;
}

void KeyFilter::RemoveAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : removing all keys...");

    for (WORD keyId { 0 }; keyId < KeyFilter::statusKeys.size(); ++keyId)
        KeyFilter::RemoveKey(keyId);

    KeyFilter::activeKeys = 0;
    KeyFilter::activePttKeys = 0;
}

void KeyFilter::ReleaseAllKeys() noexcept
{
    Logger::LogToFile("[dbg:keyfilter] : releasing all keys...");

    for (WORD keyId { 0 }; keyId < KeyFilter::statusKeys.size(); ++keyId)
        KeyFilter::PushReleaseEvent(keyId);

    KeyFilter::activeKeys = 0;
    KeyFilter::activePttKeys = 0;
}

bool KeyFilter::PushPressEvent(const BYTE keyId) noexcept
{
    if (!KeyFilter::statusKeys[keyId])
        return false;

    if (KeyFilter::pressedKeys[keyId])
        return false;

    const bool isPtt = KeyFilter::pttKeys[keyId];
    const int newPttKeys = isPtt ? (KeyFilter::activePttKeys + 1) : KeyFilter::activePttKeys;

    KeyFilter::pressedKeys[keyId] = KeyFilter::keyQueue
        .try_emplace(keyId, true, KeyFilter::activeKeys + 1, newPttKeys, isPtt);

    if (KeyFilter::pressedKeys[keyId])
    {
        ++KeyFilter::activeKeys;
        if (isPtt) ++KeyFilter::activePttKeys;
    }

    return KeyFilter::pressedKeys[keyId];
}

bool KeyFilter::PushReleaseEvent(const BYTE keyId) noexcept
{
    if (!KeyFilter::pressedKeys[keyId])
        return false;

    const bool isPtt = KeyFilter::pttKeys[keyId];
    const int newPttKeys = (isPtt && KeyFilter::activePttKeys > 0) ? (KeyFilter::activePttKeys - 1) : KeyFilter::activePttKeys;

    KeyFilter::pressedKeys[keyId] = !KeyFilter::keyQueue
        .try_emplace(keyId, false, KeyFilter::activeKeys - 1, newPttKeys, isPtt);

    if (!KeyFilter::pressedKeys[keyId])
    {
        --KeyFilter::activeKeys;
        if (isPtt && KeyFilter::activePttKeys > 0) --KeyFilter::activePttKeys;
    }

    return !KeyFilter::pressedKeys[keyId];
}

bool KeyFilter::PopEvent(KeyEvent& event) noexcept
{
    if (KeyFilter::keyQueue.empty())
        return false;

    event = *KeyFilter::keyQueue.front();
    KeyFilter::keyQueue.pop();

    return true;
}

SPSCQueue<KeyEvent> KeyFilter::keyQueue { 256 };

std::array<bool, 256> KeyFilter::pressedKeys {};
std::array<bool, 256> KeyFilter::statusKeys {};
std::array<bool, 256> KeyFilter::pttKeys {};

int KeyFilter::activeKeys { 0 };
int KeyFilter::activePttKeys { 0 };
