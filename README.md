# DefenderDisabler

A small Windows C++ program that disables Microsoft Defender Antivirus by writing to the same registry keys used by Windows Group Policy. Shared for **educational purposes**, to show how these policy-based registry keys work.

> [!WARNING]
> **This program weakens the security of the machine it runs on.**
> - It requires **Administrator rights** and will refuse to run without them.
> - It turns off real-time protection, cloud-delivered protection and related Microsoft Defender Antivirus features, leaving the system more exposed to malware.
> - On current versions of Windows, **Tamper Protection** (enabled by default) is designed to block exactly this kind of registry change and/or revert it automatically — see [How it works](#how-it-works) below.
> - Use this **only** on a machine you own, ideally in a **disposable virtual machine**, never on a shared, managed, work, or someone else's computer.
> - The author provides this code as-is, for study, and is **not responsible** for any damage, data loss, or security incident resulting from its use or misuse.
> - See [Restoring Windows Defender](#restoring-windows-defender) to undo its effect.

## What it does

The program is a single Win32 GUI application (`main.cpp` + `GetLastErrorAsString.cpp`, no external dependencies beyond the Windows SDK). When launched it:

1. Checks whether the current process token is elevated (`IsAdministrator`). If not, it shows a message box ("This application must be executed with administrator rights.") and exits.
2. If elevated, it opens (or creates) `HKEY_LOCAL_MACHINE\SOFTWARE\Policies\Microsoft\Windows Defender` and sets the following `REG_DWORD` values to `1`:
   - `DisableAntiSpyware`
   - `DisableRealtimeMonitoring`
   - `DisableAntiVirus`
   - `DisableSpecialRunningModes`
   - `DisableRoutinelyTakingAction`
   - `ServiceKeepAlive`
3. Under the `Real-Time Protection` subkey, it sets `DisableRealtimeMonitoring`, `DisableBehaviorMonitoring`, `DisableOnAccessProtection`, `DisableScanOnRealtimeEnable`, and `DisableIOAVProtection` to `1`.
4. Under the `Signature Updates` subkey, it sets `ForceUpdateFromMU` to `1`.
5. Under the `Spynet` subkey, it sets `DisableBlockAtFirstSeen` to `1`.
6. Shows a success message box asking the user to restart the computer, or an error message box if any registry operation failed.

All of this is a direct sequence of `RegOpenKeyExA` / `RegCreateKeyExA` / `RegSetValueExA` calls against documented registry values. There is no networking, no file dropping, no persistence mechanism (no scheduled task, service, or startup entry installed by the program itself), and no obfuscation — the program only edits the registry values listed above and then exits.

## How it works

The keys above live under `HKLM\SOFTWARE\Policies\Microsoft\Windows Defender`, which is the same registry location that Windows uses for **Group Policy** settings for Microsoft Defender Antivirus (the policies normally configured through `gpedit.msc`, under *Computer Configuration → Administrative Templates → Windows Components → Microsoft Defender Antivirus*). Setting these values has the same effect as configuring the corresponding Group Policy settings by hand; it is not a hidden or undocumented technique.

On Windows 10 (since the "Tamper Protection" feature was introduced) and on Windows 11, **Tamper Protection** is enabled by default and is specifically designed to prevent third-party processes — including this kind of registry write — from changing core Microsoft Defender Antivirus settings. With Tamper Protection on, the registry operations above are expected to fail or to have no lasting effect, and Defender will typically restore its protected settings on its own. This repository does not attempt to detect, disable, or work around Tamper Protection in any way.

## Requirements

- Windows (the code uses the Win32 API and registry functions directly).
- A C++ compiler with the Windows SDK headers, e.g. MinGW-w64 (`g++`) or MSVC.
- Administrator rights to run the resulting executable.

## Building

Compile the source files directly, for example with MinGW-w64:

```bash
g++ main.cpp -municode -mwindows -static -static-libgcc -static-libstdc++ -o DefenderDisabler.exe
```

## Usage

Run the compiled executable **as Administrator**. A User Account Control (UAC) prompt will appear; accepting it elevates the process. The program then applies the registry changes described above and shows a message box indicating success (with a prompt to restart the computer) or failure.

## Restoring Windows Defender

To reverse the changes made by this tool:

1. Open **Windows Security → Virus & threat protection → Manage settings**, and turn **Real-time protection**, **Cloud-delivered protection**, and **Tamper Protection** back on.
2. Alternatively, delete the registry values the tool created (or remove the entire `HKEY_LOCAL_MACHINE\SOFTWARE\Policies\Microsoft\Windows Defender` key and its subkeys, if nothing else on the machine depends on it) using `regedit`, then restart the computer.
3. If Group Policy is available (`gpedit.msc`), set the corresponding Microsoft Defender Antivirus policies back to **Not Configured**.
4. If Defender's own PowerShell module is still available, settings can also be reviewed with `Get-MpComputerStatus` and adjusted with `Set-MpPreference` (for example `Set-MpPreference -DisableRealtimeMonitoring $false`).

With Tamper Protection enabled, Windows is designed to prevent or automatically reverse these registry changes on its own.

## License

[ISC](LICENSE.md) © 2022 Kayo Souza
