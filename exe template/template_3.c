";

	const char *temp = getenv("TEMP");
	if (temp == NULL) {
		return 1;
	}

	char path[512];
	snprintf(path, sizeof(path), "%s\\%s", temp, name);

	FILE *f = fopen(path, "w");
	if (f == NULL) {
		return 1;
	}

	fputs(content, f);
	fclose(f);

	return 0;
}