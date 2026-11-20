/*
    This is a SampVoice project file
    Author: CyberMor <cyber.mor.2020@gmail.ru>
    open.mp version author: AmyrAhmady (iAmir) <hhm6@yahoo.com>

    See more here https://github.com/AmyrAhmady/sampvoice
    Original repository: https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "AddressesBase.h"

#include <cstring>
#include "Logger.h"

AddressesBase::SampVersion AddressesBase::globalVersion { AddressesBase::SampVersion::Unknown };

static DWORD FindPattern(const DWORD start, const DWORD length, const BYTE* pattern, const size_t patternLen) noexcept
{
    if (start == 0 || length < patternLen) return 0;
    for (DWORD i = 0; i <= length - patternLen; ++i)
    {
        if (std::memcmp(reinterpret_cast<const void*>(start + i), pattern, patternLen) == 0)
        {
            return start + i;
        }
    }
    return 0;
}

AddressesBase::AddressesBase(const DWORD baseAddr) noexcept
    : baseAddr(baseAddr)
{
    if (baseAddr == 0)
        return;

    const auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(baseAddr);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        Logger::LogToFile("[err:addrbase] : invalid DOS header signature");
        return;
    }

    const auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>(baseAddr + dosHeader->e_lfanew);
    if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
    {
        Logger::LogToFile("[err:addrbase] : invalid NT header signature");
        return;
    }

    const DWORD entryPoint = ntHeader->OptionalHeader.AddressOfEntryPoint;
    switch (entryPoint)
    {
        case 0x31DF13: this->version = SampVersion::R1; break;
        case 0x3195DD: this->version = SampVersion::R2; break;
        case 0xCC4D0:  this->version = SampVersion::R3; break;
        case 0xCBCB0:  this->version = SampVersion::R4; break;
        case 0xCBC90:  this->version = SampVersion::R5; break;
        case 0xFDB60:  this->version = SampVersion::DL; break;
        default:       this->version = SampVersion::Unknown; break;
    }

    AddressesBase::globalVersion = this->version;
    Logger::LogToFile("[dbg:addrbase] : detected SA-MP entryPoint: 0x%X, version: %d",
        entryPoint, static_cast<int>(this->version));

    // Known static offsets by version
    switch (this->version)
    {
    case SampVersion::R1:
        this->pNetGameAddr           = baseAddr + 0x21A0F8;
        this->pChatAddr              = baseAddr + 0x21A0E4;
        this->pInputBoxAddr          = baseAddr + 0x21A0E8;
        this->pScoreboardAddr        = baseAddr + 0x21A0B4;
        this->pGameAddr              = baseAddr + 0x21A10C;
        this->rcInitAddr             = baseAddr + 0x2401D3;
        this->bassInitCallAddr       = baseAddr + 0x628DF;
        this->bassSetConfigAddr      = baseAddr + 0x6290F;
        this->sampInitAddr           = baseAddr + 0x2565E2;
        this->sampDestructAddr       = baseAddr + 0x9380;
        this->openChatFunc           = baseAddr + 0x657E0;
        this->switchModeFunc         = baseAddr + 0x5D7B0;
        this->openScoreboardFunc     = baseAddr + 0x6AD30;
        this->createPlayerInPoolFunc = baseAddr + 0x10D50;
        this->deletePlayerFromPoolFunc = baseAddr + 0x10B90;
        this->spawnLocalPlayerFunc   = baseAddr + 0x3AD0;
        this->drawLabelFunc          = baseAddr + 0x686C0;
        break;
    case SampVersion::R3:
        this->pNetGameAddr           = baseAddr + 0x26E8DC;
        this->pChatAddr              = baseAddr + 0x26E8C8;
        this->pInputBoxAddr          = baseAddr + 0x26E8CC;
        this->pScoreboardAddr        = baseAddr + 0x26E894;
        this->pGameAddr              = baseAddr + 0x26E8F4;
        this->rcInitAddr             = baseAddr + 0xB658;
        this->bassInitCallAddr       = baseAddr + 0x65D2F;
        this->bassSetConfigAddr      = baseAddr + 0x65D5F;
        this->sampInitAddr           = baseAddr + 0xC57E2;
        this->sampDestructAddr       = baseAddr + 0x9510;
        this->openChatFunc           = baseAddr + 0x68D10;
        this->switchModeFunc         = baseAddr + 0x60B50;
        this->openScoreboardFunc     = baseAddr + 0x6EC80;
        this->createPlayerInPoolFunc = baseAddr + 0x13E80;
        this->deletePlayerFromPoolFunc = baseAddr + 0x13CB0;
        this->spawnLocalPlayerFunc   = baseAddr + 0x3AD0;
        this->drawLabelFunc          = baseAddr + 0x6C630;
        break;
    case SampVersion::R5:
        this->pNetGameAddr           = baseAddr + 0x26EB94;
        this->pChatAddr              = baseAddr + 0x26EB80;
        this->pInputBoxAddr          = baseAddr + 0x26EB84;
        this->pScoreboardAddr        = baseAddr + 0x26EB4C;
        this->pGameAddr              = baseAddr + 0x26EBAC;
        this->sampDestructAddr       = baseAddr + 0x9880;
        break;
    case SampVersion::DL:
        this->pNetGameAddr           = baseAddr + 0x2ACA24;
        this->pChatAddr              = baseAddr + 0x2ACA10;
        this->pInputBoxAddr          = baseAddr + 0x2ACA14;
        this->pScoreboardAddr        = baseAddr + 0x2AC9DC;
        this->pGameAddr              = baseAddr + 0x2ACA3C;
        this->sampDestructAddr       = baseAddr + 0x9570;
        break;
    default:
        break;
    }

    const DWORD codeBase = baseAddr + ntHeader->OptionalHeader.BaseOfCode;
    const DWORD codeSize = ntHeader->OptionalHeader.SizeOfCode;

    // 1. Universal BASS_Init scan
    const BYTE bassInitPattern[] = { 0x6A, 0x00, 0x68, 0x44, 0xAC, 0x00, 0x00, 0x6A, 0xFF, 0xE8 };
    const DWORD bassFound = FindPattern(codeBase, codeSize, bassInitPattern, sizeof(bassInitPattern));
    if (bassFound != 0)
    {
        this->bassInitCallAddr = bassFound + 9;
        const BYTE bassSetConfigPattern[] = { 0x50, 0x6A, 0x05, 0xE8 };
        const DWORD setConfigFound = FindPattern(this->bassInitCallAddr, 120, bassSetConfigPattern, sizeof(bassSetConfigPattern));
        if (setConfigFound != 0)
        {
            this->bassSetConfigAddr = setConfigFound;
        }
    }

    // 2. Universal rcInitAddr scan (RakClient creation in CNetGame constructor)
    const BYTE push3247Pattern[] = { 0x68, 0x47, 0x32, 0x00, 0x00 };
    const DWORD pushFound = FindPattern(codeBase, codeSize, push3247Pattern, sizeof(push3247Pattern));
    if (pushFound != 0)
    {
        DWORD fnStart = pushFound;
        for (DWORD p = pushFound; p >= (pushFound > 64 ? pushFound - 64 : codeBase); --p)
        {
            if (*reinterpret_cast<const BYTE*>(p) == 0xCC || *reinterpret_cast<const BYTE*>(p) == 0x90)
            {
                fnStart = p + 1;
                break;
            }
        }

        for (DWORD p = codeBase; p < codeBase + codeSize - 10; ++p)
        {
            if (*reinterpret_cast<const BYTE*>(p) == 0xE8)
            {
                const INT rel = *reinterpret_cast<const INT*>(p + 1);
                if (p + 5 + rel == fnStart)
                {
                    const BYTE* after = reinterpret_cast<const BYTE*>(p + 5);
                    if (after[0] == 0x8B && after[1] == 0xCE && after[2] == 0x89)
                    {
                        this->rcInitAddr = p + 5 + 2;
                        break;
                    }
                    else if (after[0] == 0x89)
                    {
                        this->rcInitAddr = p + 5;
                        break;
                    }
                }
            }
        }
    }

    // 3. Universal sampInitAddr scan
    if (this->pNetGameAddr != 0)
    {
        BYTE movPNetGamePattern[5];
        movPNetGamePattern[0] = 0xA3;
        *reinterpret_cast<DWORD*>(&movPNetGamePattern[1]) = this->pNetGameAddr;
        const DWORD movFound = FindPattern(codeBase, codeSize, movPNetGamePattern, 5);
        if (movFound != 0)
        {
            this->sampInitAddr = movFound + 5;
        }
    }

    this->isValid = (this->bassInitCallAddr != 0 &&
                     this->bassSetConfigAddr != 0 &&
                     this->rcInitAddr != 0 &&
                     this->sampInitAddr != 0 &&
                     this->pNetGameAddr != 0);

    Logger::LogToFile("[dbg:addrbase] : init complete (valid: %d, bassInit: 0x%X, bassSetCfg: 0x%X, rcInit: 0x%X, sampInit: 0x%X, pNetGame: 0x%X)",
        this->isValid ? 1 : 0,
        this->bassInitCallAddr ? this->bassInitCallAddr - baseAddr : 0,
        this->bassSetConfigAddr ? this->bassSetConfigAddr - baseAddr : 0,
        this->rcInitAddr ? this->rcInitAddr - baseAddr : 0,
        this->sampInitAddr ? this->sampInitAddr - baseAddr : 0,
        this->pNetGameAddr ? this->pNetGameAddr - baseAddr : 0);
}

bool AddressesBase::IsValid() const noexcept
{
    return this->isValid;
}

AddressesBase::SampVersion AddressesBase::GetVersion() const noexcept
{
    return this->version;
}

AddressesBase::SampVersion AddressesBase::GetGlobalVersion() noexcept
{
    return AddressesBase::globalVersion;
}

DWORD AddressesBase::GetBaseAddr() const noexcept
{
    return this->baseAddr;
}

DWORD AddressesBase::GetRcInitAddr() const noexcept
{
    return this->rcInitAddr;
}

DWORD AddressesBase::GetBassInitCallAddr() const noexcept
{
    return this->bassInitCallAddr;
}

DWORD AddressesBase::GetBassSetConfigAddr() const noexcept
{
    return this->bassSetConfigAddr;
}

DWORD AddressesBase::GetSampInitAddr() const noexcept
{
    return this->sampInitAddr;
}

DWORD AddressesBase::GetSampDestructAddr() const noexcept
{
    return this->sampDestructAddr;
}

DWORD AddressesBase::GetOpenChatFunc() const noexcept
{
    return this->openChatFunc;
}

DWORD AddressesBase::GetSwitchModeFunc() const noexcept
{
    return this->switchModeFunc;
}

DWORD AddressesBase::GetOpenScoreboardFunc() const noexcept
{
    return this->openScoreboardFunc;
}

DWORD AddressesBase::GetCreatePlayerInPoolFunc() const noexcept
{
    return this->createPlayerInPoolFunc;
}

DWORD AddressesBase::GetDeletePlayerFromPoolFunc() const noexcept
{
    return this->deletePlayerFromPoolFunc;
}

DWORD AddressesBase::GetSpawnLocalPlayerFunc() const noexcept
{
    return this->spawnLocalPlayerFunc;
}

DWORD AddressesBase::GetDrawLabelFunc() const noexcept
{
    return this->drawLabelFunc;
}

DWORD AddressesBase::GetPNetGameAddr() const noexcept
{
    return this->pNetGameAddr;
}

DWORD AddressesBase::GetPChatAddr() const noexcept
{
    return this->pChatAddr;
}

DWORD AddressesBase::GetPInputBoxAddr() const noexcept
{
    return this->pInputBoxAddr;
}

DWORD AddressesBase::GetPScoreboardAddr() const noexcept
{
    return this->pScoreboardAddr;
}

DWORD AddressesBase::GetPGameAddr() const noexcept
{
    return this->pGameAddr;
}
