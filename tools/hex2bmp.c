// (C) Copyright 2023 Vicharak Computers LLP
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 1) {
		printf("too few arguments\n");
		exit(1);
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		printf("file not found\n");
		exit(1);
	}

	FILE *logo_file = fopen("logo.h", "w");
	if (!logo_file) {
		printf("logo.h file not found\n");
		exit(1);
	}

	fprintf(logo_file, "unsigned char logo_bmp[] = {");

	int c = fgetc(fp);
	int i = 0;
	while (c != EOF) {
		if (!(i++ % 16))
			fprintf(logo_file, "\n");

		fprintf(logo_file, "0x%02x, ", c);
		c = fgetc(fp);
	}
	fprintf(logo_file, "\n};\nunsigned int logo_bmp_len = %d;", i);

	fclose(logo_file);
	fclose(fp);
}
