#include <windows.h>
#include <string>

#ifndef GET_LAST_ERROR_AS_STRING
#define GET_LAST_ERROR_AS_STRING

const std::wstring GetLastErrorAsString(const DWORD errorMessageId = GetLastError()){
	if(errorMessageId == 0) return std::wstring();

	LPWSTR messageBuffer = nullptr;

	const size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &messageBuffer, 0, NULL);
	const std::wstring message(messageBuffer, size);

	LocalFree(messageBuffer);

	return message;
}

#endif
