#include <windows.h>
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

void DisableDefender(const PHKEY phKey, const LPCSTR lpSubKey){
	const LPCSTR lpValueName = "DisableAntiSpyware";

	AssertSuccess(RegOpenKeyExA(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_ALL_ACCESS, phKey));

	const uint32_t data = 1;

	AssertSuccess(RegSetValueExA(*phKey, lpValueName, 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
}

void DisableRealTimeProtection(PHKEY phKey){
	RegCreateKeyExA(*phKey, "Real-Time Protection", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, phKey, 0);

	const HKEY hKey = *phKey;
	const uint32_t data = 1;
	const size_t dataSize = sizeof(data);

	AssertSuccess(RegSetValueExA(hKey, "DisableRealtimeMonitoring", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableBehaviorMonitoring", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableOnAccessProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableOnAccessProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueExA(hKey, "DisableIOAVProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow){
	if(!IsAdministrator()){
		MessageBoxA(NULL, "This application must be executed with administrator rights.", "Operation failed", MB_OK);
		return EXIT_FAILURE;
	}

	HKEY hKey;
	bool isError;

	try{
		DisableDefender(&hKey, "SOFTWARE\\Policies\\Microsoft\\Windows Defender");
		DisableRealTimeProtection(&hKey);

		MessageBoxA(NULL, "Windows Defender disabled.\nRestart your computer to apply configurations.", "Success", MB_OK);
		RegCloseKey(hKey);
	}catch(const std::wstring error){
		MessageBoxW(NULL, L"Error: " + *error.c_str(), L"Operation failed", MB_OK);

		if(hKey) RegCloseKey(hKey);

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
