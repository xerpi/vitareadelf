/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elf.h"
#include "vita_module.h"

static void usage();

int main(int argc, char *argv[])
{
	uint32_t i;

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

	elf_print_ehdr(&ehdr);

	for (i = 0; i < ehdr.e_phnum; i++) {
		printf("Program Header %d:\n", i);
		elf_print_phdr(&phdr[i]);
	}

	uint32_t modinfo_seg;
	uint32_t modinfo_off;
	sce_module_info modinfo;

	sce_read_module_info(fp, &ehdr, phdr, &modinfo, &modinfo_seg, &modinfo_off);

	printf("Segment containing sce_module_info: %d, offset: 0x%X\n\n",
		modinfo_seg, modinfo_off);

	sce_print_module_info(&modinfo);

	sce_module_exports *modexps;
	int n_exports;
	n_exports = sce_load_module_exports(fp, &ehdr, &phdr[modinfo_seg], &modinfo, &modexps);

	printf("Read %d exports:\n\n", n_exports);

	for (i = 0; i < n_exports; i++) {
		printf("sce_module_exports %d:\n", i);
		sce_print_module_exports(&modexps[i]);
	}

	free(modexps);
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
