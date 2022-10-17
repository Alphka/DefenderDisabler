#include <windows.h>
#include <string>
#include "GetLastErrorAsString.cpp"

bool IsAdministrator(){
	HANDLE token;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)){
		TOKEN_ELEVATION elevation;
		DWORD size;

		if(GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) return elevation.TokenIsElevated;
	}

	return false;
}

void AssertSuccess(const DWORD arg){
	if(arg == ERROR_SUCCESS) return;
	throw GetLastErrorAsString(arg);
}

void DisableDefender(const PHKEY phKey, const LPCSTR lpSubKey){
	const LPCSTR lpValueName = "DisableAntiSpyware";

	AssertSuccess(RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_ALL_ACCESS, phKey));

	const uint32_t data = 1;

	AssertSuccess(RegSetValueEx(*phKey, lpValueName, 0, REG_DWORD, (LPBYTE) &data, sizeof(data)));
}

void DisableRealTimeProtection(PHKEY phKey){
	RegCreateKeyEx(*phKey, "Real-Time Protection", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, phKey, 0);

	const HKEY hKey = *phKey;
	const uint32_t data = 1;
	const size_t dataSize = sizeof(data);

	AssertSuccess(RegSetValueEx(hKey, "DisableRealtimeMonitoring", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueEx(hKey, "DisableBehaviorMonitoring", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueEx(hKey, "DisableOnAccessProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueEx(hKey, "DisableOnAccessProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
	AssertSuccess(RegSetValueEx(hKey, "DisableIOAVProtection", 0, REG_DWORD, (LPBYTE) &data, dataSize));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	if(!IsAdministrator()){
		MessageBox(NULL, "This application must be executed with administrator rights.", "Operation failed", NULL);
		return EXIT_FAILURE;
	}

	try{
		HKEY hKey;
		DisableDefender(&hKey, "SOFTWARE\\Policies\\Microsoft\\Windows Defender");
		DisableRealTimeProtection(&hKey);
	}catch(const std::wstring error){
		MessageBoxW(NULL, L"Error: " + (*error.c_str()), L"Operation failed", NULL);
		return EXIT_FAILURE;
	}

	return MessageBox(NULL, "Windows Defender disabled.\nRestart your computer to apply configurations.", "Success", NULL);
}
