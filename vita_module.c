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

int sce_load_module_exports(FILE *fp, const Elf32_Phdr *modexp_phdr, const sce_module_info *modinfo, sce_module_exports **modexps)
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
		*modexps = NULL;
		return -1;
	}

	return total/sizeof(sce_module_exports);
}


int sce_load_module_imports(FILE *fp, const Elf32_Phdr *modimp_phdr, const sce_module_info *modinfo, sce_module_imports **modimps)
{
	uint32_t imports_base = modimp_phdr->p_offset + modinfo->import_top;
	uint32_t imports_end = modimp_phdr->p_offset + modinfo->import_end;

	printf("Imports base: 0x%X\nImports end: 0x%X\n\n", imports_base, imports_end);

	size_t total = imports_end - imports_base;

	*modimps = malloc(total);

	fseek(fp, imports_base, SEEK_SET);

	size_t n = fread(*modimps, 1, total, fp);

	if (n != total) {
		fprintf(stderr, "Could not read module imports\n");
		free(*modimps);
		*modimps = NULL;
		return -1;
	}

	return total/sizeof(sce_module_imports);
}

int sce_load_module_export_nids(FILE *fp, const Elf32_Phdr *modexp_phdr, const sce_module_exports *modexp, sce_nid **nids)
{
	uint32_t nid_table_off = modexp->nid_table - modexp_phdr->p_vaddr + modexp_phdr->p_offset;

	uint32_t num_nids = modexp->num_syms_funcs + modexp->num_syms_vars + modexp->num_syms_unk;

	size_t nid_table_size = 4 * num_nids;

	*nids = malloc(nid_table_size);

	fseek(fp, nid_table_off, SEEK_SET);

	size_t n = fread(*nids, 1, nid_table_size, fp);

	if (n != nid_table_size) {
		fprintf(stderr, "Could not read module export NIDS\n");
		free(*nids);
		*nids = NULL;
		return -1;
	}

	return num_nids;
}

int sce_load_module_import_func_nids(FILE *fp, const Elf32_Phdr *modimp_phdr, const sce_module_imports *modimp, sce_nid **nids)
{
	uint32_t func_nid_table_off = modimp->func_nid_table - modimp_phdr->p_vaddr + modimp_phdr->p_offset;

	uint32_t num_nids = modimp->num_syms_funcs;

	size_t func_nid_table_size = 4 * num_nids;

	*nids = malloc(func_nid_table_size);

	fseek(fp, func_nid_table_off, SEEK_SET);

	size_t n = fread(*nids, 1, func_nid_table_size, fp);

	if (n != func_nid_table_size) {
		fprintf(stderr, "Could not read module import Function NIDS\n");
		free(*nids);
		*nids = NULL;
		return -1;
	}

	return num_nids;
}

int sce_load_module_import_var_nids(FILE *fp, const Elf32_Phdr *modimp_phdr, const sce_module_imports *modimp, sce_nid **nids)
{
	uint32_t var_nid_table_off = modimp->var_nid_table - modimp_phdr->p_vaddr + modimp_phdr->p_offset;

	uint32_t num_nids = modimp->num_syms_vars;

	size_t var_nid_table_size = 4 * num_nids;

	*nids = malloc(var_nid_table_size);

	fseek(fp, var_nid_table_off, SEEK_SET);

	size_t n = fread(*nids, 1, var_nid_table_size, fp);

	if (n != var_nid_table_size) {
		fprintf(stderr, "Could not read module import Variable NIDS\n");
		free(*nids);
		*nids = NULL;
		return -1;
	}

	return num_nids;
}

int sce_load_module_import_unk_nids(FILE *fp, const Elf32_Phdr *modimp_phdr, const sce_module_imports *modimp, sce_nid **nids)
{
	uint32_t unk_nid_table_off = modimp->unk_nid_table - modimp_phdr->p_vaddr + modimp_phdr->p_offset;

	uint32_t num_nids = modimp->num_syms_unk;

	size_t unk_nid_table_size = 4 * num_nids;

	*nids = malloc(unk_nid_table_size);

	fseek(fp, unk_nid_table_off, SEEK_SET);

	size_t n = fread(*nids, 1, unk_nid_table_size, fp);

	if (n != unk_nid_table_size) {
		fprintf(stderr, "Could not read module import unkiable NIDS\n");
		free(*nids);
		*nids = NULL;
		return -1;
	}

	return num_nids;
}

void sce_read_module_name(FILE *fp, uint32_t module_name_addr, const Elf32_Phdr *mod_phdr, char *modname, int modname_len)
{
	uint32_t nameoff = module_name_addr - mod_phdr->p_vaddr + mod_phdr->p_offset;
	fseek(fp, nameoff, SEEK_SET);
	fgets(modname, modname_len, fp);
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


void sce_print_module_export(const sce_module_exports *modexp)
{
	printf(
		"  size                                 0x%X\n"
		"  version                              0x%X\n"
		"  flags                                0x%X\n"
		"  num_syms_funcs                       0x%X\n"
		"  num_syms_vars                        0x%X\n"
		"  num_syms_unk                         0x%X\n"
		"  module_nid                           0x%X\n"
		"  module_name_off                      0x%X\n"
		"  nid_table                            0x%X\n"
		"  entry_table                          0x%X\n\n",
	modexp->size, modexp->version, modexp->flags,
	modexp->num_syms_funcs, modexp->num_syms_vars,
	modexp->num_syms_unk, modexp->module_nid,
	modexp->module_name, modexp->nid_table, modexp->entry_table);
}

void sce_print_module_import(const sce_module_imports *modimp)
{
	printf(
		"  size                                 0x%X\n"
		"  version                              0x%X\n"
		"  flags                                0x%X\n"
		"  num_syms_funcs                       0x%X\n"
		"  num_syms_vars                        0x%X\n"
		"  num_syms_unk                         0x%X\n"
		"  reserved1                            0x%X\n"
		"  module_nid                           0x%X\n"
		"  module_name_off                      0x%X\n"
		"  reserved2                            0x%X\n"
		"  func_nid_table                       0x%X\n"
		"  func_entry_table                     0x%X\n"
		"  var_nid_table                        0x%X\n"
		"  var_entry_table                      0x%X\n"
		"  unk_nid_table                        0x%X\n"
		"  unk_entry_table                      0x%X\n\n",
	modimp->size, modimp->version, modimp->flags,
	modimp->num_syms_funcs, modimp->num_syms_vars,
	modimp->num_syms_unk, modimp->reserved1, modimp->module_nid,
	modimp->module_name, modimp->reserved2, modimp->func_nid_table,
	modimp->func_entry_table, modimp->var_nid_table,
	modimp->var_entry_table, modimp->unk_nid_table,
	modimp->unk_entry_table);
}

void sce_print_module_export_nids(const sce_nid *nids, uint16_t num_syms_funcs, uint16_t num_syms_vars, uint16_t num_syms_unk)
{
	uint32_t j;
	uint32_t idx = 0;

	/* Print Function NIDS */
	for (j = 0; j < num_syms_funcs; j++, idx++) {
		printf("      Function NID %d: 0x%08X\n", j, nids[idx]);
	}

	/* Print Variable NIDS */
	for (j = 0; j < num_syms_vars; j++, idx++) {
		printf("      Variable NID %d: 0x%08X\n", j, nids[idx]);
	}

	/* Print Unknown NIDS */
	for (j = 0; j < num_syms_unk; j++, idx++) {
		printf("      Unknown NID %d: 0x%08X\n", j, nids[idx]);
	}

	printf("\n");
}

void sce_print_module_import_nids(const sce_nid *func_nids, const sce_nid *var_nids, const sce_nid *unk_nids,
	uint16_t num_syms_funcs, uint16_t num_syms_vars, uint16_t num_syms_unk)
{
	uint32_t j;

	/* Print Function NIDS */
	for (j = 0; j < num_syms_funcs; j++) {
		printf("      Function NID %d: 0x%08X\n", j, func_nids[j]);
	}

	/* Print Variable NIDS */
	for (j = 0; j < num_syms_vars; j++) {
		printf("      Variable NID %d: 0x%08X\n", j, var_nids[j]);
	}

	/* Print Unknown NIDS */
	for (j = 0; j < num_syms_unk; j++) {
		printf("      Unknown NID %d: 0x%08X\n", j, unk_nids[j]);
	}

	printf("\n");
}
