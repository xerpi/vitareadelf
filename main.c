/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"

static void usage();

int main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		goto exit_error;
	}

	FILE *fp = fopen(argv[1], "rb");

	Elf32_Ehdr ehdr;
	if (!elf_read_ehdr(fp, &ehdr)) {
		fprintf(stderr, "ERROR: loading ELF\n");
		goto exit_close;
	}

	Elf32_Phdr *phdr;
	if (!elf_load_phdr(fp, &ehdr, &phdr)) {
		fprintf(stderr, "ERROR: loading ELF Program Header\n");
		goto exit_close;
	}

	Elf32_Shdr *shdr;
	if (!elf_load_shdr(fp, &ehdr, &shdr)) {
		fprintf(stderr, "ERROR: loading ELF Section Header\n");
		goto exit_free_phdr;
	}

	//elf_print_ehdr(&ehdr);
	unsigned int modinfo_seg = ehdr.e_entry >> 30;
	unsigned int modinfo_off = ehdr.e_entry & 0x3FFFFFFF;
	printf("Segment containing sce_module_info: %d, offset: 0x%X\n",
		modinfo_seg, modinfo_off);

	int i;
	for (i = 0; i < ehdr.e_phnum; i++) {
		printf("Program Header 0x%X:\n", i);
		elf_print_phdr(&phdr[i]);
	}



	elf_free_shdr(&shdr);
	elf_free_phdr(&phdr);
	fclose(fp);
	return EXIT_SUCCESS;

//exit_free_shdr:
	elf_free_shdr(&shdr);
exit_free_phdr:
	elf_free_phdr(&phdr);
exit_close:
	fclose(fp);
exit_error:
	return EXIT_FAILURE;
}

void usage()
{
	fprintf(stderr,
		"vitareadelf by xerpi\n"
		"Usage:\n"
		"\t./vitareadelf sample.elf\n\n");
}
