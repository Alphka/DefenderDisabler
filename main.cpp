#include <windows.h>
#include <cstdint>
#include <string>
#include "GetLastErrorAsString.cpp"

bool IsAdministrator(){
	HANDLE token;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)){
		TOKEN_ELEVATION elevation;
		DWORD size = sizeof(elevation);

		if(GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)){
			CloseHandle(token);
			return elevation.TokenIsElevated;
		}
	}

	if(token) CloseHandle(token);

	return false;
}

void AssertSuccess(const DWORD arg){
	if(arg == ERROR_SUCCESS) return;
	throw GetLastErrorAsString(arg);
}

void DisableDefender(){
	HKEY hKey;

	AssertSuccess(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_ALL_ACCESS, &hKey));

	const uint32_t data = 1;

	AssertSuccess(RegSetValueExA(hKey, "DisableAntiSpyware", 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
	AssertSuccess(RegSetValueExA(hKey, "DisableRealtimeMonitoring", 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
	AssertSuccess(RegSetValueExA(hKey, "DisableAntiVirus", 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
	AssertSuccess(RegSetValueExA(hKey, "DisableSpecialRunningModes", 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
	AssertSuccess(RegSetValueExA(hKey, "DisableRoutinelyTakingAction", 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
	AssertSuccess(RegSetValueExA(hKey, "ServiceKeepAlive", 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));

	RegCloseKey(hKey);
}

void DisableRealTimeProtection(){
	HKEY hKey;

	AssertSuccess(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_ALL_ACCESS, &hKey));
	AssertSuccess(RegCreateKeyExA(hKey, "Real-Time Protection", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0));

	const uint32_t data = 1;
	const size_t dataSize = sizeof(data);

	AssertSuccess(RegSetValueExA(hKey, "DisableRealtimeMonitoring", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableBehaviorMonitoring", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableOnAccessProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableScanOnRealtimeEnable", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableIOAVProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));

	RegCloseKey(hKey);
}

void DisableSignatureUpdates(){
	HKEY hKey;

	AssertSuccess(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_ALL_ACCESS, &hKey));
	AssertSuccess(RegCreateKeyExA(hKey, "Signature Updates", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0));

	const uint32_t data = 1;
	const size_t dataSize = sizeof(data);

	AssertSuccess(RegSetValueExA(hKey, "ForceUpdateFromMU", 0, REG_DWORD, (LPBYTE) &data, dataSize));

	RegCloseKey(hKey);
}

void DisableSpyNet(){
	HKEY hKey;

	AssertSuccess(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Policies\\Microsoft\\Windows Defender", 0, KEY_ALL_ACCESS, &hKey));
	AssertSuccess(RegCreateKeyExA(hKey, "Spynet", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0));

	const uint32_t data = 1;
	const size_t dataSize = sizeof(data);

	AssertSuccess(RegSetValueExA(hKey, "DisableBlockAtFirstSeen", 0, REG_DWORD, (LPBYTE) &data, dataSize));

	RegCloseKey(hKey);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
	if(!IsAdministrator()){
		MessageBoxA(NULL, "This application must be executed with administrator rights.", "Operation failed", MB_OK);
		return EXIT_FAILURE;
	}

	try{
		DisableDefender();
		DisableRealTimeProtection();
		DisableSignatureUpdates();
		DisableSpyNet();

		MessageBoxA(NULL, "Windows Defender disabled.\nRestart your computer to apply configurations.", "Success", MB_OK);
	}catch(const std::wstring error){
		MessageBoxW(NULL, (L"Error: " + error).c_str(), L"Operation failed", MB_OK);

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
