#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

int file_exists(const char *path) {
	DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES &&
			!(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <path>\n", argv[0]);
		return 1;
	}

	if (!file_exists(argv[1])) {
		fprintf(stderr, "File not found: '%s'\n", argv[1]);
		return 1;
	}

	char fullPath[MAX_PATH];
	if (!GetFullPathNameA(argv[1], MAX_PATH, fullPath, NULL)) {
		fprintf(stderr, "Failed to resolve path\n");
		return 1;
	}

	char cmd[2048];
	snprintf(cmd, sizeof(cmd),
		"chrome\\chrome.exe --app=\"file:///%s\"",
		fullPath);

	system(cmd);

	return 0;
}