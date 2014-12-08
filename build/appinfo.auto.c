#include "pebble_process_info.h"
#include "src/resource_ids.auto.h"

const PebbleProcessInfo __pbl_app_info __attribute__ ((section (".pbl_header"))) = {
  .header = "PBLAPP",
  .struct_version = { PROCESS_INFO_CURRENT_STRUCT_VERSION_MAJOR, PROCESS_INFO_CURRENT_STRUCT_VERSION_MINOR },
  .sdk_version = { PROCESS_INFO_CURRENT_SDK_VERSION_MAJOR, PROCESS_INFO_CURRENT_SDK_VERSION_MINOR },
  .process_version = { 2, 3 },
  .load_size = 0xb6b6,
  .offset = 0xb6b6b6b6,
  .crc = 0xb6b6b6b6,
  .name = "PebbleMsgPro",
  .company = "Yang Tsao Software",
  .icon_resource_id = RESOURCE_ID_IMAGE_APP_LOGO,
  .sym_table_addr = 0xA7A7A7A7,
  .flags = 0,
  .num_reloc_entries = 0xdeadcafe,
  .uuid = { 0x2D, 0x0B, 0x18, 0xB1, 0x0E, 0xE6, 0x41, 0xCA, 0x95, 0x79, 0xA0, 0xC4, 0xC6, 0x56, 0x8D, 0x93 },
  .virtual_size = 0xb6b6
};
