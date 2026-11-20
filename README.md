# SAMPVOICE (Multi-Version Edition)

[![Build](https://github.com/SakaZuki113/sampvoice/actions/workflows/build.yml/badge.svg)](https://github.com/SakaZuki113/sampvoice/actions/workflows/build.yml)
[![Release](https://img.shields.io/github/v/release/SakaZuki113/sampvoice?label=Release)](https://github.com/SakaZuki113/sampvoice/releases)

Language: [English](#english) | [Bahasa Indonesia](#bahasa-indonesia)

---

### Credits & Acknowledgments
* **Original Creator**: [CyberMor](https://github.com/CyberMor) ([CyberMor/sampvoice](https://github.com/CyberMor/sampvoice))
* **open.mp Reference & Port**: [AmyrAhmady (iAmir)](https://github.com/AmyrAhmady) ([AmyrAhmady/sampvoice](https://github.com/AmyrAhmady/sampvoice))
* **Multi-Version SA:MP & open.mp Fork**: [SakaZuki113](https://github.com/SakaZuki113) ([SakaZuki113/sampvoice](https://github.com/SakaZuki113/sampvoice))

---

<a name="english"></a>
# English

## Description
**SAMPVOICE** is a powerful Software Development Kit (SDK) designed for implementing high-performance, low-latency 3D positional voice communication in Pawn for **open.mp** and **SA-MP** servers.

This enhanced **Multi-Version Edition** introduces comprehensive multi-version compatibility, preventing client-side crashes across different SA-MP versions and providing an improved, distraction-free HUD experience.

### Version Support
* **Client**: All SA:MP versions — **0.3.7 (R1, R3, R5)**, **0.3.DL-R1**, and **open.mp**.
* **Server**: Latest **open.mp** version (as well as legacy SA-MP 0.3.7 and 0.3.DL server binaries).

---

## Key Features
* **Universal Multi-Version Compatibility**: Automatically detects and adjusts internal SA-MP player pool and remote ped offsets across 0.3.7-R1, 0.3.7-R3, 0.3.7-R5, and 0.3.DL-R1 without crashes.
* **Realistic 3D Positional Audio**: Realistic sound attenuation based on distance and camera orientation with BASS 3D audio engine.
* **Instant HUD Feedback**:
  * **Radar Mic Indicator**: Micro-icon above the minimap remains hidden when idle, lights up while speaking, and disappears **instantly** upon key release.
  * **Head Speaker Icon**: 3D speaker icon displays smoothly above the talking player's character in-game.
* **Multiple Voice Modes**: Supports dynamic range adjustments (e.g. Whisper, Normal, Shout).
* **Controlled Traffic & Low Latency**: Audio streams compressed with Opus codec for minimal bandwidth overhead.

---

## Installation

### For Players (Client)
1. Download `sampvoice-client.zip` from the [Releases page](https://github.com/SakaZuki113/sampvoice/releases).
2. Extract the contents directly into your **GTA San Andreas** root directory (where `gta_sa.exe` and `samp.dll` are located):
   * `sampvoice.asi`
   * `bass.dll`
   * `sampvoice/` folder (containing textures/icons and audio libraries)
> [!TIP]
> If you encounter an `Old bass.dll installed` or `BASS_IsStarted entry point not found` error on 0.3.DL / SA-MP Addon, download `bass.dll [0.3DL]` directly from the release page and replace `bass.dll` in your GTA folder.

### For Developers (Server)
1. Download `sampvoice-server-binaries.zip` from the [Releases page](https://github.com/SakaZuki113/sampvoice/releases).
2. Extract the plugin file into your server's `plugins/` folder:
   * **Windows**: `sampvoice.dll`
   * **Linux**: `sampvoice.so`
3. Add `sampvoice` to your server configuration:
   * **open.mp (`config.json`)**: Add `"sampvoice"` to the `pawn.legacy_plugins` array.
   * **SA-MP (`server.cfg`)**: Add `plugins sampvoice` (Windows) or `plugins sampvoice.so` (Linux).
   *(Note: If using `Pawn.RakNet`, load `sampvoice` after it).*
4. Place `sampvoice.inc` in your `qawno/include` (open.mp) or `pawno/include` (SA-MP) directory.

---

## Usage & Implementation Example

Include the library in your script:
```pawn
#include <sampvoice>
```

### Complete Modern Proximity Voice Implementation
Below is a clean, production-ready example featuring Push-To-Talk ('B' key) and dynamic distance mode switching ('Z' key: Whisper 4m, Normal 15m, Shout 45m):

```pawn
#include <open.mp> // or <a_samp>
#include <sampvoice>

#define VOICE_KEY_TALK      (0x42) // Key 'B' (Push-To-Talk)
#define VOICE_KEY_MODE      (0x5A) // Key 'Z' (Cycle Distance Mode)

#define VOICE_DIST_WHISPER  (4.0)  // Whisper: 4 meters
#define VOICE_DIST_NORMAL   (15.0) // Normal: 15 meters
#define VOICE_DIST_SHOUT    (45.0) // Shout: 45 meters

static SV_DLSTREAM:g_PlayerVoiceStream[MAX_PLAYERS] = {SV_NULL, ...};
static g_PlayerVoiceMode[MAX_PLAYERS] = {1, ...}; // Default: 1 (Normal)

public OnGameModeInit()
{
    // Initialize SampVoice with 40 kbps bitrate
    SvInit(40000);
    print("[SAMPVOICE] Voice system initialized.");
    return 1;
}

public OnGameModeExit()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_PlayerVoiceStream[i] != SV_NULL)
        {
            SvDeleteStream(g_PlayerVoiceStream[i]);
            g_PlayerVoiceStream[i] = SV_NULL;
        }
    }
    return 1;
}

forward Voice_SetupPlayer(playerid);
public Voice_SetupPlayer(playerid)
{
    if (!IsPlayerConnected(playerid) || g_PlayerVoiceStream[playerid] != SV_NULL)
        return 0;

    // Check if player has SampVoice client installed
    if (SvGetVersion(playerid) == 0)
    {
        SendClientMessage(playerid, 0xFF6600FF, "[VOICE] SampVoice client plugin not detected.");
        return 0;
    }

    // Create 3D dynamic local stream attached to player
    g_PlayerVoiceStream[playerid] = SvCreateDLStreamAtPlayer(VOICE_DIST_NORMAL, MAX_PLAYERS, playerid, 0x00FFBFFF, "Proximity Voice");
    if (g_PlayerVoiceStream[playerid] != SV_NULL)
    {
        SvAttachSpeakerToStream(g_PlayerVoiceStream[playerid], playerid);
    }

    // Register Push-To-Talk key ('B') and Mode Switch key ('Z')
    SvAddKey(playerid, VOICE_KEY_TALK, SV_TRUE);
    SvAddKey(playerid, VOICE_KEY_MODE, SV_FALSE);

    SendClientMessage(playerid, 0x00FF00FF, "[VOICE] Hold 'B' to talk. Press 'Z' to cycle distance (Whisper / Normal / Shout).");
    return 1;
}

public OnPlayerSpawn(playerid)
{
    if (g_PlayerVoiceStream[playerid] == SV_NULL)
    {
        // Delay slightly after connect/spawn to allow handshake packet
        SetTimerEx("Voice_SetupPlayer", 1000, false, "i", playerid);
    }
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    if (g_PlayerVoiceStream[playerid] != SV_NULL)
    {
        SvDeleteStream(g_PlayerVoiceStream[playerid]);
        g_PlayerVoiceStream[playerid] = SV_NULL;
    }
    g_PlayerVoiceMode[playerid] = 1;
    return 1;
}

// Key Press Callback
public OnPlayerActivationKeyPress(SV_UINT:playerid, SV_UINT:keyid)
{
    if (keyid == VOICE_KEY_TALK)
    {
        // Push-to-Talk ('B') is automatically recorded by the client
        // Update custom HUD / textdraws here if needed
    }
    else if (keyid == VOICE_KEY_MODE)
    {
        // Cycle mode: Whisper (0) -> Normal (1) -> Shout (2)
        g_PlayerVoiceMode[playerid] = (g_PlayerVoiceMode[playerid] + 1) % 3;
        new Float:dist = VOICE_DIST_NORMAL;

        switch (g_PlayerVoiceMode[playerid])
        {
            case 0:
            {
                dist = VOICE_DIST_WHISPER;
                SendClientMessage(playerid, 0xFFFF00FF, "[VOICE] Distance Mode: Whisper (4m)");
            }
            case 1:
            {
                dist = VOICE_DIST_NORMAL;
                SendClientMessage(playerid, 0x00FF00FF, "[VOICE] Distance Mode: Normal (15m)");
            }
            case 2:
            {
                dist = VOICE_DIST_SHOUT;
                SendClientMessage(playerid, 0xFF0000FF, "[VOICE] Distance Mode: Shout (45m)");
            }
        }

        if (g_PlayerVoiceStream[playerid] != SV_NULL)
        {
            SvUpdateDistanceForLStream(g_PlayerVoiceStream[playerid], dist);
        }
    }
    return 1;
}

// Key Release Callback
public OnPlayerActivationKeyRelease(SV_UINT:playerid, SV_UINT:keyid)
{
    if (keyid == VOICE_KEY_TALK)
    {
        // Push-to-Talk ('B') released
        // Update custom HUD / textdraws here if needed
    }
    return 1;
}
```

---

<a name="bahasa-indonesia"></a>
# Bahasa Indonesia

## Deskripsi
**SAMPVOICE** adalah Software Development Kit (SDK) mutakhir yang dirancang untuk mengimplementasikan sistem komunikasi suara (Voice Chat) 3D berkinerja tinggi, berlatensi rendah, serta berorientasi spasial menggunakan bahasa Pawn pada server **open.mp** dan **SA-MP**.

Edisi **Multi-Version** ini menghadirkan perbaikan menyeluruh pada kompatibilitas lintas versi SA-MP sehingga tidak lagi terjadi force close (crash) saat pemain saling bertemu atau spawn, serta dilengkapi pembaruan indikator HUD yang bersih dan responsif.

### Dukungan Versi
* **Client**: Seluruh versi SA:MP — **0.3.7 (R1, R3, R5)**, **0.3.DL-R1**, dan **open.mp**.
* **Server**: Versi **open.mp** terbaru (serta binary server SA-MP 0.3.7 dan 0.3.DL).

---

## Fitur Utama
* **Kompatibilitas Multi-Version Universal**: Mendeteksi versi SA-MP client secara otomatis dan menyesuaikan offset internal pool pemain serta remote ped tanpa mengalami crash antara 0.3.7 dan 0.3.DL.
* **Audio Spasial 3D Realistis**: Atenuasi volume dinamis berdasarkan jarak dan sudut pandang kamera pemain menggunakan BASS Audio Engine.
* **Indikator HUD Responsif & Bersih**:
  * **Ikon Mic Radar**: Ikon mic di atas radar minimap tidak muncul saat diam/pasif, langsung menyala ketika berbicara, dan **seketika menghilang** saat tombol bicara dilepas tanpa jeda/fading lambat.
  * **Ikon Speaker 3D**: Ikon speaker di atas kepala karakter pemain menyala secara natural saat pemain sedang berbicara.
* **Mode Jarak Fleksibel**: Mendukung perubahan jarak jangkauan suara secara langsung (Whisper, Normal, Shout).
* **Kompresi Suara Efisien**: Menggunakan Opus audio codec untuk konsumsi bandwidth rendah dan suara jernih.

---

## Panduan Instalasi

### Untuk Pemain (Client)
1. Unduh arsip `sampvoice-client.zip` dari [Halaman Releases](https://github.com/SakaZuki113/sampvoice/releases).
2. Ekstrak seluruh file ke dalam folder utama instalasi **GTA San Andreas** Anda (folder tempat `gta_sa.exe` dan `samp.dll` berada):
   * `sampvoice.asi`
   * `bass.dll`
   * Folder `sampvoice/` (ikon, font, dan library pendukung)
> [!TIP]
> Jika Anda mengalami kendala `Old bass.dll installed` atau `Entry Point Not Found (BASS_IsStarted)` pada SA-MP 0.3.DL / SA-MP Addon, unduh file `bass.dll [0.3DL]` langsung dari halaman release dan timpa file `bass.dll` di folder GTA San Andreas Anda.

### Untuk Pengembang (Server)
1. Unduh `sampvoice-server-binaries.zip` dari [Halaman Releases](https://github.com/SakaZuki113/sampvoice/releases).
2. Salin file plugin ke dalam folder `plugins/` pada server Anda:
   * **Windows**: `sampvoice.dll`
   * **Linux**: `sampvoice.so`
3. Daftarkan `sampvoice` pada konfigurasi server:
   * **open.mp (`config.json`)**: Masukkan `"sampvoice"` ke dalam bagian `pawn.legacy_plugins`.
   * **SA-MP (`server.cfg`)**: Tambahkan baris `plugins sampvoice` (Windows) atau `plugins sampvoice.so` (Linux).
   *(Catatan: Jika server menggunakan plugin `Pawn.RakNet`, letakkan `sampvoice` setelahnya).*
4. Tempatkan file `sampvoice.inc` di dalam folder include compiler Anda (`qawno/include` atau `pawno/include`).

---

## Tatacara Penggunaan & Contoh Implementasi Pawn

Untuk mulai menggunakan fungsionalitas plugin pada gamemode, sertakan header:
```pawn
#include <sampvoice>
```

### Contoh Implementasi 3D Proximity Voice Lengkap
Berikut adalah contoh skrip gamemode yang siap pakai untuk open.mp / SA-MP dengan tombol bicara Push-To-Talk ('B') dan tombol pergantian mode jarak ('Z' untuk Whisper 4m, Normal 15m, Shout 45m):

```pawn
#include <open.mp> // atau <a_samp>
#include <sampvoice>

#define VOICE_KEY_TALK      (0x42) // Tombol 'B' (Push-To-Talk)
#define VOICE_KEY_MODE      (0x5A) // Tombol 'Z' (Ganti Jarak)

#define VOICE_DIST_WHISPER  (4.0)  // Whisper: 4 meter
#define VOICE_DIST_NORMAL   (15.0) // Normal: 15 meter
#define VOICE_DIST_SHOUT    (45.0) // Shout: 45 meter

static SV_DLSTREAM:g_PlayerVoiceStream[MAX_PLAYERS] = {SV_NULL, ...};
static g_PlayerVoiceMode[MAX_PLAYERS] = {1, ...}; // Default: 1 (Normal)

public OnGameModeInit()
{
    // Inisialisasi SampVoice dengan bitrate 40 kbps
    SvInit(40000);
    print("[SAMPVOICE] Voice system initialized.");
    return 1;
}

public OnGameModeExit()
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (g_PlayerVoiceStream[i] != SV_NULL)
        {
            SvDeleteStream(g_PlayerVoiceStream[i]);
            g_PlayerVoiceStream[i] = SV_NULL;
        }
    }
    return 1;
}

forward Voice_SetupPlayer(playerid);
public Voice_SetupPlayer(playerid)
{
    if (!IsPlayerConnected(playerid) || g_PlayerVoiceStream[playerid] != SV_NULL)
        return 0;

    // Cek apakah pemain menggunakan client SampVoice
    if (SvGetVersion(playerid) == 0)
    {
        SendClientMessage(playerid, 0xFF6600FF, "[VOICE] Plugin SampVoice client tidak terpasang.");
        return 0;
    }

    // Buat 3D local stream dinamis pada posisi pemain
    g_PlayerVoiceStream[playerid] = SvCreateDLStreamAtPlayer(VOICE_DIST_NORMAL, MAX_PLAYERS, playerid, 0x00FFBFFF, "Proximity Voice");
    if (g_PlayerVoiceStream[playerid] != SV_NULL)
    {
        SvAttachSpeakerToStream(g_PlayerVoiceStream[playerid], playerid);
    }

    // Daftarkan tombol bicara PTT ('B') dan tombol ganti mode ('Z')
    SvAddKey(playerid, VOICE_KEY_TALK, SV_TRUE);
    SvAddKey(playerid, VOICE_KEY_MODE, SV_FALSE);

    SendClientMessage(playerid, 0x00FF00FF, "[VOICE] Tahan 'B' untuk bicara. Tekan 'Z' untuk ganti jarak suara.");
    return 1;
}

public OnPlayerSpawn(playerid)
{
    if (g_PlayerVoiceStream[playerid] == SV_NULL)
    {
        // Berikan jeda sejenak setelah spawn/connect agar handshake client selesai
        SetTimerEx("Voice_SetupPlayer", 1000, false, "i", playerid);
    }
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    if (g_PlayerVoiceStream[playerid] != SV_NULL)
    {
        SvDeleteStream(g_PlayerVoiceStream[playerid]);
        g_PlayerVoiceStream[playerid] = SV_NULL;
    }
    g_PlayerVoiceMode[playerid] = 1;
    return 1;
}

// Callback saat tombol ditekan
public OnPlayerActivationKeyPress(SV_UINT:playerid, SV_UINT:keyid)
{
    if (keyid == VOICE_KEY_TALK)
    {
        // Tombol Push-to-Talk ('B') otomatis direkam oleh client
        // Update status TextDraw / HUD kustom di sini jika dibutuhkan
    }
    else if (keyid == VOICE_KEY_MODE)
    {
        // Pergantian jarak secara berurutan: Whisper (0) -> Normal (1) -> Shout (2)
        g_PlayerVoiceMode[playerid] = (g_PlayerVoiceMode[playerid] + 1) % 3;
        new Float:dist = VOICE_DIST_NORMAL;

        switch (g_PlayerVoiceMode[playerid])
        {
            case 0:
            {
                dist = VOICE_DIST_WHISPER;
                SendClientMessage(playerid, 0xFFFF00FF, "[VOICE] Mode Jarak: Whisper (4m)");
            }
            case 1:
            {
                dist = VOICE_DIST_NORMAL;
                SendClientMessage(playerid, 0x00FF00FF, "[VOICE] Mode Jarak: Normal (15m)");
            }
            case 2:
            {
                dist = VOICE_DIST_SHOUT;
                SendClientMessage(playerid, 0xFF0000FF, "[VOICE] Mode Jarak: Shout (45m)");
            }
        }

        if (g_PlayerVoiceStream[playerid] != SV_NULL)
        {
            SvUpdateDistanceForLStream(g_PlayerVoiceStream[playerid], dist);
        }
    }
    return 1;
}

// Callback saat tombol dilepas
public OnPlayerActivationKeyRelease(SV_UINT:playerid, SV_UINT:keyid)
{
    if (keyid == VOICE_KEY_TALK)
    {
        // Tombol Push-to-Talk ('B') dilepas
        // Update status TextDraw / HUD kustom di sini jika dibutuhkan
    }
    return 1;
}
```

---

## Kompilasi / Building

Plugin dapat dikompilasi untuk target **Win32** dan **Linux x86** menggunakan CMake.

### Windows (Client & Server)
```sh
git clone https://github.com/SakaZuki113/sampvoice.git
cd sampvoice
git submodule update --init --recursive
mkdir build && cd build
cmake .. -A Win32 -DCMAKE_BUILD_TYPE=Release -DBUILD_CLIENT=ON
cmake --build . --config Release
```

### Linux (Server)
```sh
cd docker
chmod +x ./build.sh ./docker-entrypoint.sh
./build.sh
```
Binary server (`sampvoice.so`) akan dihasilkan pada direktori `docker/build/server/`.
