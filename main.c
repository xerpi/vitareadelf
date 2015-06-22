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
	n_exports = sce_load_module_exports(fp, &phdr[modinfo_seg], &modinfo, &modexps);

	printf("Read %d exports:\n\n", n_exports);

	for (i = 0; i < n_exports; i++) {
		char modname[128];
		uint32_t nameoff = modexps[i].module_name - phdr[modinfo_seg].p_vaddr + phdr[modinfo_seg].p_offset;
		fseek(fp, nameoff, SEEK_SET);
		fgets(modname, 128, fp);

		printf("sce_module_exports %d (%s)\n", i, modname);
		sce_print_module_export(&modexps[i]);

		/* Read exported NIDS */
		sce_nid *nids = NULL;
		int n_nids = 0; // Function + data + unk nids

		n_nids = sce_load_module_export_nids(fp, &phdr[modinfo_seg], &modexps[i], &nids);
		printf("    Number of NIDS: %d\n", n_nids);

		/* Print Function NIDS */
		sce_print_module_export_nids(nids,
			modexps[i].num_syms_funcs,
			modexps[i].num_syms_vars,
			modexps[i].num_syms_unk);

		free(nids);
	}

	sce_module_imports *modimps;
	int n_imports;
	n_imports = sce_load_module_imports(fp, &phdr[modinfo_seg], &modinfo, &modimps);

	printf("Read %d imports:\n\n", n_imports);

	for (i = 0; i < n_imports; i++) {
		char modname[128];
		uint32_t nameoff = modimps[i].module_name - phdr[modinfo_seg].p_vaddr + phdr[modinfo_seg].p_offset;
		fseek(fp, nameoff, SEEK_SET);
		fgets(modname, 128, fp);

		printf("sce_module_imports %d (%s)\n", i, modname);
		sce_print_module_import(&modimps[i]);

		/* Read imported NIDS */
		sce_nid *func_nids = NULL;
		sce_nid *var_nids = NULL;
		sce_nid *unk_nids = NULL;
		int n_nids = 0; // Function + data + unk nids

		n_nids += sce_load_module_import_func_nids(fp, &phdr[modinfo_seg], &modimps[i], &func_nids);
		n_nids += sce_load_module_import_var_nids(fp, &phdr[modinfo_seg], &modimps[i], &var_nids);
		n_nids += sce_load_module_import_unk_nids(fp, &phdr[modinfo_seg], &modimps[i], &unk_nids);

		printf("    Number of NIDS: %d\n", n_nids);

		/* Print Function NIDS */
		sce_print_module_import_nids(
			func_nids, var_nids, unk_nids,
			modimps[i].num_syms_funcs,
			modimps[i].num_syms_vars,
			modimps[i].num_syms_unk);

		free(func_nids);
		free(var_nids);
		free(unk_nids);
	}


	free(modimps);
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
