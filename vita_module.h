/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#ifndef VITA_MODULE_H
#define VITA_MODULE_H

#include <stdint.h>
#include "elf.h"

typedef uint32_t sce_nid;

typedef struct __attribute__((packed)) {
	uint16_t     attributes;
	uint16_t     version;
	uint8_t      name[27];
	uint8_t      type;
	uint32_t     gp_value;
	uint32_t     export_top;
	uint32_t     export_end;
	uint32_t     import_top;
	uint32_t     import_end;
	uint32_t     library_nid;
	uint32_t     field_38;
	uint32_t     field_3C;
	uint32_t     field_40;
	uint32_t     module_start;
	uint32_t     module_stop;
	uint32_t     exidx_top;
	uint32_t     exidx_end;
	uint32_t     extab_top;
	uint32_t     extab_end;
} sce_module_info;

typedef struct __attribute__((packed)) {
	uint16_t    size;
	uint16_t    version;
	uint16_t    flags;
	uint16_t    num_syms_funcs;
	uint32_t    num_syms_vars;
	uint32_t    num_syms_unk;
	uint32_t    module_nid;
	uint32_t    module_name;
	uint32_t    nid_table;
	uint32_t    entry_table;
} sce_module_exports;

typedef struct __attribute__((packed)) {
	uint16_t     size;
	uint16_t     version;
	uint16_t     flags;
	uint16_t     num_syms_funcs;
	uint16_t     num_syms_vars;
	uint16_t     num_syms_unk;
	uint32_t     reserved1;
	uint32_t     module_nid;
	uint32_t     module_name;
	uint32_t     reserved2;
	uint32_t     func_nid_table;
	uint32_t     func_entry_table;
	uint32_t     var_nid_table;
	uint32_t     var_entry_table;
	uint32_t     unk_nid_table;
	uint32_t     unk_entry_table;
} sce_module_imports;


#define EXPORT_MODULE_EXEC_NID       0x00000000

#define EXPORT_MODULE_STOP_FUNC_NID  0x79F8E492
#define EXPORT_MODULE_EXIT_FUNC_NID  0x913482A9
#define EXPORT_MODULE_START_FUNC_NID 0x935CD196
#define EXPORT_MODULE_INFO_VAR_NID   0x6C2224BA

void sce_read_module_info(FILE *fp, const Elf32_Ehdr *ehdr,
	const Elf32_Phdr *phdrs, sce_module_info *modinfo,
	uint32_t *modinfo_seg, uint32_t *modinfo_off);


/* Returns the number of exports */
int sce_load_module_exports(FILE *fp, const Elf32_Phdr *modexp_phdr,
	const sce_module_info *modinfo, sce_module_exports **modexps);

/* Returns the number of imports */
int sce_load_module_imports(FILE *fp, const Elf32_Phdr *modimp_phdr,
	const sce_module_info *modinfo, sce_module_imports **modimps);

/* Returns the total number of NIDS (func + var + unk) */
int sce_load_module_export_nids(FILE *fp, const Elf32_Phdr *modexp_phdr,
	const sce_module_exports *modexp, sce_nid **nids);

/* Returns the number of Function NIDS */
int sce_load_module_import_func_nids(FILE *fp, const Elf32_Phdr *modimp_phdr,
	const sce_module_imports *modimp, sce_nid **nids);

/* Returns the number of Variable NIDS */
int sce_load_module_import_var_nids(FILE *fp, const Elf32_Phdr *modimp_phdr,
	const sce_module_imports *modimp, sce_nid **nids);

/* Returns the number of Unkown NIDS */
int sce_load_module_import_unk_nids(FILE *fp, const Elf32_Phdr *modimp_phdr,
	const sce_module_imports *modimp, sce_nid **nids);

void sce_read_module_name(FILE *fp, uint32_t module_name_addr,
	const Elf32_Phdr *mod_phdr, char *modname, int modname_len);

void sce_print_module_info(const sce_module_info *modinfo);
void sce_print_module_export(const sce_module_exports *modexp);
void sce_print_module_import(const sce_module_imports *modimp);
void sce_print_module_export_nids(const sce_nid *nids,
	uint16_t num_syms_funcs,
	uint16_t num_syms_vars,
	uint16_t num_syms_unk);
void sce_print_module_import_nids(
	const sce_nid *func_nids,
	const sce_nid *var_nids,
	const sce_nid *unk_nids,
	uint16_t num_syms_funcs,
	uint16_t num_syms_vars,
	uint16_t num_syms_unk);


#endif
