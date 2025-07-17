#ifndef __patchfile_h__
#define __patchfile_h__
#include <stdint.h>

#define MAX_UF_SIZE (8192)
#define MAX_UF_BODY_SIZE (MAX_UF_SIZE - sizeof(patch_hdr_t))

#define MSRAM_DWORD_COUNT_MAX (0x54 * 2)
#define MSRAM_GROUP_SIZE  (0x8)
#define MSRAM_GROUP_COUNT_MAX (MSRAM_DWORD_COUNT_MAX / MSRAM_GROUP_SIZE)
#define PATCH_CR_OP_COUNT_MAX (0x10)
#define MSRAM_BASE_ADDRESS (0xFEB)

typedef struct __attribute__((packed)) {
	uint32_t      header_ver;
	uint32_t      update_rev;
	uint32_t      date_bcd;
	uint32_t      proc_sig;
	uint32_t      checksum;
	uint32_t      loader_ver;
	uint32_t      proc_flags;
	uint32_t      data_size;
	uint32_t      total_size;
	uint8_t       reserved[12];
} patch_hdr_t;

typedef struct __attribute__((packed)) {
	uint32_t      address;
	uint32_t      mask;
	uint32_t      value;
	uint32_t      integrity;
} patch_cr_op_t;


typedef struct {
	int           key_seed_offs;
	int           msram_offs;
	int           msram_integrity_offs;
	int           cr_ops_offs;
	int           cr_ops_count;
	int           msram_dword_count;
	int           filesize;
} epatch_layout_t;

typedef struct {
	uint32_t      msram[ MSRAM_DWORD_COUNT_MAX ];
	patch_cr_op_t cr_ops[ PATCH_CR_OP_COUNT_MAX ];
} patch_body_t;

typedef struct {
	patch_hdr_t   header;
	uint32_t      body[MAX_UF_BODY_SIZE / sizeof(uint32_t)];
} epatch_file_t;

#endif
