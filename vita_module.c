/*
 * Copyright (c) 2015 Sergi Granell (xerpi)
 */

#include "vita_module.h"
#include <stdio.h>

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
