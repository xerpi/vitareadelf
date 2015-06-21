/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include "vita_module.h"
#include <stdio.h>
#include <stdlib.h>

void sce_read_module_info(FILE *fp, const Elf32_Ehdr *ehdr, const Elf32_Phdr *phdrs, sce_module_info *modinfo, uint32_t *modinfo_seg, uint32_t *modinfo_off)
{
	*modinfo_seg = ehdr->e_entry >> 30;
	*modinfo_off = ehdr->e_entry & 0x3FFFFFFF;

	fseek(fp, phdrs[*modinfo_seg].p_offset + *modinfo_off, SEEK_SET);
	fread(modinfo, 1, sizeof(*modinfo), fp);
}

int sce_load_module_exports(FILE *fp, const Elf32_Ehdr *ehdr, const Elf32_Phdr *modexp_phdr, const sce_module_info *modinfo, sce_module_exports **modexps)
{
	uint32_t exports_base = modexp_phdr->p_offset + modinfo->export_top;
	uint32_t exports_end = modexp_phdr->p_offset + modinfo->export_end;

	printf("Exports base: 0x%X\nExports end: 0x%X\n\n", exports_base, exports_end);

	size_t total = exports_end - exports_base;

	*modexps = malloc(total);

	fseek(fp, exports_base, SEEK_SET);

	size_t n = fread(*modexps, 1, total, fp);

	if (n != total) {
		fprintf(stderr, "Could not read module exports\n");
		free(*modexps);
		return -1;
	}

	return total/sizeof(sce_module_exports);
}

void sce_print_module_info(const sce_module_info *modinfo)
{
	printf(
		"sce_module_info:\n"
		"  Attributes                           0x%X\n"
		"  Version                              0x%X\n"
		"  Name                                 %s\n"
		"  Type                                 0x%X\n"
		"  gp_value                             0x%X\n"
		"  export_top                           0x%X\n"
		"  export_end                           0x%X\n"
		"  import_top                           0x%X\n"
		"  import_end                           0x%X\n"
		"  library_nid                          0x%X\n"
		"  field_38                             0x%X\n"
		"  field_3C                             0x%X\n"
		"  field_40                             0x%X\n"
		"  module_start                         0x%X\n"
		"  module_stop                          0x%X\n"
		"  exidx_top                            0x%X\n"
		"  exidx_end                            0x%X\n"
		"  extab_top                            0x%X\n"
		"  extab_end                            0x%X\n\n",
	modinfo->attributes, modinfo->version, modinfo->name,
	modinfo->type, (uint32_t)(uintptr_t)modinfo->gp_value,
	modinfo->export_top, modinfo->export_end, modinfo->import_top,
	modinfo->import_end, modinfo->library_nid, modinfo->field_38,
	modinfo->field_3C, modinfo->field_40, modinfo->module_start,
	modinfo->module_stop, modinfo->exidx_top, modinfo->exidx_end,
	modinfo->extab_top, modinfo->extab_end);
}


void sce_print_module_exports(const sce_module_exports *modexp)
{
	printf(
		"  size                                 0x%X\n"
		"  version                              0x%X\n"
		"  flags                                0x%X\n"
		"  num_syms_funcs                       0x%X\n"
		"  num_syms_vars                        0x%X\n"
		"  num_syms_unk                         0x%X\n"
		"  module_nid                           0x%X\n"
		"  module_name                          0x%X\n"
		"  nid_table                            0x%X\n"
		"  entry_table                          0x%X\n\n",
	modexp->size, modexp->version, modexp->flags,
	modexp->num_syms_funcs, modexp->num_syms_vars,
	modexp->num_syms_unk, modexp->module_nid,
	modexp->module_name, modexp->nid_table, modexp->entry_table);
}
