#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winhttp.h>
#include <wincrypt.h>
#include <wchar.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "crypt32.lib")

const char *EXPECTED_HASH =
	"a215081f87090bb75f462819b66d382bf37961893d2a710692c6866fc4d87d47";

int file_exists(const char *path) {
	DWORD attr = GetFileAttributesA(path);
	return (attr != INVALID_FILE_ATTRIBUTES);
}

int download_file(const wchar_t *host, const wchar_t *path, const char *outPath) {
	HINTERNET hSession = WinHttpOpen(L"Installer/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession) return 0;

	HINTERNET hConnect = WinHttpConnect(hSession, host,
		INTERNET_DEFAULT_HTTPS_PORT, 0);

	if (!hConnect) return 0;

	HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path,
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_SECURE);

	if (!hRequest) return 0;

	if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
		return 0;

	if (!WinHttpReceiveResponse(hRequest, NULL))
		return 0;

	HANDLE file = CreateFileA(outPath, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE) return 0;

	DWORD bytesRead = 0;
	BYTE buffer[4096];

	while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
		DWORD written;
		WriteFile(file, buffer, bytesRead, &written, NULL);
	}

	CloseHandle(file);

	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);

	return 1;
}

int sha256_file(const char *path, char *outHex) {
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE) return 0;

	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;

	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
	CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);

	BYTE buffer[4096];
	DWORD bytesRead;

	while (ReadFile(file, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
		CryptHashData(hHash, buffer, bytesRead, 0);
	}

	BYTE hash[32];
	DWORD hashSize = 32;

	CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashSize, 0);

	for (int i = 0; i < 32; i++) {
		sprintf(outHex + (i * 2), "%02x", hash[i]);
	}

	outHex[64] = 0;

	CloseHandle(file);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return 1;
}

int run_installer(const char *path) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    char cmd[MAX_PATH + 10]; // Added extra space for the flag

    // Format the command string to include quotes around the path and add the /S flag
    snprintf(cmd, sizeof(cmd), "\"%s\" /S", path);

    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        return 0;

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 1;
}

int launch_positron(const char* normal_path) {
    // 1. Calculate how much space we need for the wide string conversion
    int wide_length = MultiByteToWideChar(CP_UTF8, 0, normal_path, -1, NULL, 0);
    if (wide_length <= 0) return 0;

    // 2. Allocate temporary memory for the wide string
    wchar_t* wide_path = (wchar_t*)malloc(wide_length * sizeof(wchar_t));
    if (!wide_path) return 0;

    // 3. Do the actual conversion
    MultiByteToWideChar(CP_UTF8, 0, normal_path, -1, wide_path, wide_length);

    // 4. Set up Windows process structures
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    LPCWSTR app_path = L"C:\\Program Files (x86)\\positron\\positron.exe";
    
    // 5. Build the arguments using wide characters
    wchar_t arg_buffer[32768];
    swprintf_s(arg_buffer, 32768, L"\"%s\" \"%s\"", app_path, wide_path);

    // 6. Launch securely
    BOOL success = CreateProcessW(app_path, arg_buffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    // 7. Clean up memory and handles
    free(wide_path); 
    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    return 0;
}

int main() {
	const char *name = "test.html";
	const char *content = "<head>\
    <title>thingo</title>\
</head>\
<body>\
    m\
</body>";

	const char *temp = getenv("TEMP");
	if (temp == NULL) {
		return 1;
	}

	const char *installDir = "C:\\Program Files (x86)\\positron";
	const char *installer = "C:\\Windows\\Temp\\Positron Installer.exe";

	if (!file_exists(installDir)) {
		printf("Missing dependency: Positron. Downloading...\n");

		if (!download_file(L"github.com",
			L"/ahhhhhhhhhhhhhhhhhhhhhh1/positron/raw/refs/heads/installer-release/Positron%20Installer.exe",
			installer)) {
			printf("Download failed\n");
			return 1;
		}

		char hash[65];
		if (!sha256_file(installer, hash)) {
			printf("Hash failed\n");
			return 1;
		}

		printf("SHA256: %s\n", hash);

		if (strcmp(hash, EXPECTED_HASH) != 0) {
			printf("Hash mismatch - aborting\n");
			return 1;
		}

		if (!run_installer(installer)) {
			printf("Installer failed\n");
			return 1;
		}
		printf("Continuing execution...\n");
	}


	char path[512];
	snprintf(path, sizeof(path), "%s\\%s", temp, name);

	FILE *f = fopen(path, "w");
	if (f == NULL) {
		return 1;
	}

	fputs(content, f);
	fclose(f);

    // Run the safe launcher
    launch_positron(path);

    return 0;

}