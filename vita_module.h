/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#ifndef VITA_MODULE_H
#define VITA_MODULE_H

#include <stdint.h>

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

typedef struct {
	uint16_t     size;
	uint16_t     version;
	uint16_t     flags;
	uint16_t     num_syms_funcs;
	uint16_t     num_syms_vars;
	uint16_t     num_syms_unk;
	uint32_t     reserved1;
	uint32_t     module_nid;
	char         *module_name;
	uint32_t     reserved2;
	uint32_t     *func_nid_table;
	void         **func_entry_table;
	uint32_t     *var_nid_table;
	void         **var_entry_table;
	uint32_t     *unk_nid_table;
	void         **unk_entry_table;
} sce_module_imports;


#define EXPORT_MODULE_EXEC_NID       0x00000000

#define EXPORT_MODULE_STOP_FUNC_NID  0x79F8E492
#define EXPORT_MODULE_EXIT_FUNC_NID  0x913482A9
#define EXPORT_MODULE_START_FUNC_NID 0x935CD196
#define EXPORT_MODULE_INFO_VAR_NID   0x6C2224BA

void sce_print_module_info(const sce_module_info *modinfo);
void sce_print_module_exports(const sce_module_exports *modexp);


#endif
