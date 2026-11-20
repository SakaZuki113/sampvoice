/*
    This is a SampVoice project file
    Author: CyberMor <cyber.mor.2020@gmail.ru>
    open.mp version author: AmyrAhmady (iAmir) <hhm6@yahoo.com>

    See more here https://github.com/AmyrAhmady/sampvoice
    Original repository: https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <Windows.h>

class AddressesBase {

    AddressesBase() = delete;
    AddressesBase(const AddressesBase&) noexcept = default;
    AddressesBase(AddressesBase&&) = delete;
    AddressesBase& operator=(const AddressesBase&) noexcept = default;
    AddressesBase& operator=(AddressesBase&&) = delete;

public:

    enum class SampVersion {
        Unknown,
        R1,
        R2,
        R3,
        R4,
        R5,
        DL
    };

    explicit AddressesBase(DWORD baseAddr) noexcept;

    ~AddressesBase() noexcept = default;

public:

    bool IsValid() const noexcept;
    SampVersion GetVersion() const noexcept;
    static SampVersion GetGlobalVersion() noexcept;

    DWORD GetBaseAddr() const noexcept;
    DWORD GetRcInitAddr() const noexcept;
    DWORD GetBassInitCallAddr() const noexcept;
    DWORD GetBassSetConfigAddr() const noexcept;
    DWORD GetSampInitAddr() const noexcept;
    DWORD GetSampDestructAddr() const noexcept;
    DWORD GetOpenChatFunc() const noexcept;
    DWORD GetSwitchModeFunc() const noexcept;
    DWORD GetOpenScoreboardFunc() const noexcept;
    DWORD GetCreatePlayerInPoolFunc() const noexcept;
    DWORD GetDeletePlayerFromPoolFunc() const noexcept;
    DWORD GetSpawnLocalPlayerFunc() const noexcept;
    DWORD GetDrawLabelFunc() const noexcept;

    DWORD GetPNetGameAddr() const noexcept;
    DWORD GetPChatAddr() const noexcept;
    DWORD GetPInputBoxAddr() const noexcept;
    DWORD GetPScoreboardAddr() const noexcept;
    DWORD GetPGameAddr() const noexcept;

private:

    static SampVersion globalVersion;

    bool isValid { false };
    SampVersion version { SampVersion::Unknown };

    DWORD baseAddr { NULL };
    DWORD rcInitAddr { NULL };
    DWORD bassInitCallAddr { NULL };
    DWORD bassSetConfigAddr { NULL };
    DWORD sampInitAddr { NULL };
    DWORD sampDestructAddr { NULL };
    DWORD openChatFunc { NULL };
    DWORD switchModeFunc { NULL };
    DWORD openScoreboardFunc { NULL };
    DWORD createPlayerInPoolFunc { NULL };
    DWORD deletePlayerFromPoolFunc { NULL };
    DWORD spawnLocalPlayerFunc { NULL };
    DWORD drawLabelFunc { NULL };

    DWORD pNetGameAddr { NULL };
    DWORD pChatAddr { NULL };
    DWORD pInputBoxAddr { NULL };
    DWORD pScoreboardAddr { NULL };
    DWORD pGameAddr { NULL };

};

using AddressesBasePtr = std::unique_ptr<AddressesBase>;
#define MakeAddressesBase std::make_unique<AddressesBase>
