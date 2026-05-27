#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
	const char *name = "__test.html";
	const char *content = "<head>\
    <title>thingo</title>\
</head>\
<body>\
    m\
</body>";
    chdir("C:\\Program Files (x86)\\positron");

	const char *temp = getenv("TEMP");
	if (temp == NULL) {
		return 1;
	}
	
	char path[512];
	
	struct stat stats;
	
	snprintf(path, sizeof(path), "%s\\positron", temp);

	if (stat(path, &stats) != 0) {
		mkdir(path);
	}
	
	snprintf(path, sizeof(path), "%s\\positron\\%s", temp, name);

    if (stat("C:\\Program Files (x86)\\positron", &stats) != 0) {
        MessageBox(
			NULL,                             // Handle to owner window (NULL = no owner)
			"this app requires Positron to work, please download and install it.", // Text message content
			"dependency missing",             // Title bar caption
        MB_OK | MB_ICONINFORMATION        // Configuration flags (buttons & icons)
    	);

		ShellExecute(NULL, "open", "https://github.com/ahhhhhhhhhhhhhhhhhhhhhh1/positron/raw/refs/heads/installer-release/Positron%20Installer.exe?download=", NULL, NULL, SW_SHOWNORMAL);
    }

	printf("%s %s\n", "opening file: ", path);

	FILE *f = fopen(path, "w");
	if (f == NULL) {
		return 1;
	}

	printf("%s\n", "opened file. dumping content.");

	fputs(content, f);
	printf("%s\n", "dumped content. closing file.");
	fclose(f);
	printf("%s\n", "closed file");

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

	snprintf(path, sizeof(path), "\"C:\\Program Files (x86)\\positron\\positron.exe\" \"%s\\positron\\%s\"", temp, name);
	printf("%s: %s\n", "trying to run", path);
    // Start the child process.
    if (!CreateProcess(
        NULL, // Path to application
        path,                       // Command line arguments
        NULL,                       // Process handle not inheritable
        NULL,                       // Thread handle not inheritable
        FALSE,                      // Set handle inheritance to FALSE
        0,                          // No creation flags
        NULL,                       // Use parent's environment block
        NULL,                       // Use parent's starting directory
        &si,                        // Pointer to STARTUPINFO structure
        &pi)                        // Pointer to PROCESS_INFORMATION structure
    ) {
        printf("CreateProcess failed (%lu).\n", GetLastError());
        return 1;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;

}