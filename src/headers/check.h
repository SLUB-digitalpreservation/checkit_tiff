/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff 
 * (see file LICENSE)
 *
 */

#ifndef _FIXIT_TIFF_CHECK
#define _FIXIT_TIFF_CHECK
#include <stdlib.h>
#include "tiff.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef int thandle_t;

typedef uint16 tag_t;

typedef enum{ true=1, false=0 } bool_t;

typedef enum{ is_memmap, is_filep } ct_ioflag_t ; /* flag */

typedef enum{ has_sorted_tags, has_unsorted_tags, unknown_tag_order } tagorder_t;

typedef struct ctiff_s {
  ct_ioflag_t ioflag;
  tagorder_t tagorder;
  int tag_cache[65536];
  uint8 * streamp; /* TODO: address of memory-mapped tif stream */
  uint8 * actual_streamp;
  uint32 streamlen; /* TODO: length of tif stream/file (in bytes) */
  thandle_t fd; /* filedescriptor */
  uint32 ifd0pos; /* offset in tif stream (in bytes) */
  uint16 ifd0c; /* count of tags in first ifd */
  char isbyteswapped; /* true if BigEndian */
  char *filename; /* filename */
  /* TODO: add file size */
} ctiff_t;

typedef enum{
  rm_default,
  rm_file,
  rm_rule,
  rm_tag,
  rm_value,
  rm_expected,
  rm_hard_error,
  rm_error,
  rm_warning,
  rm_precondition,
  rm_logicalor_error,
  rm_logicalor_valid,
  rm_is_valid,
  rm_endrule,
  rm_endtiff,
  rm_mode,
  rm_lineno,
  rm_error_description,
  rm_summary_valid,
  rm_summary_invalid,
} rm_type_t;
typedef struct retmsg_s {
  rm_type_t rm_type;
  char * rm_msg;
  struct retmsg_s * next;
} retmsg_t;

#define ICC_ERROR_OFFSET 100

typedef enum {
  is_valid=0, /* no problems detected */
  calling_error_count_size, /* a called function has wrong arguments */
  could_not_allocate_memory, /* malloc fails */
  could_not_print, /* snprintf, fprintf, print fails */
  should_not_occur, /* dummy, for dead code */
  tagerror_expected_count_differs, /* if a tag reports count=m, but the rule expects count=n */
  tagerror_expected_count_iszero, /* if a tag reports count=0, but the rule expects count=n */
  tagerror_expected_count_isgreaterone,
  tagerror_value_differs, /* if a tag reports value=m, but the rule expects value=n */
  tagerror_unexpected_type_found, /* if a tag reports a type=m, but a type=n was expected */
  tagerror_datetime_not_plausible, /* if a datetime tag has date values out of range */
  tagerror_datetime_wrong_format, /* if a datetime tag has date with wrong format string */
  tagerror_datetime_wrong_size, /* if a datetime tag has date with wrong size */
  tagerror_value_not_found, /* if a value of a given tag is not found */
  tagwarn_type_of_unknown_tag_could_not_be_checked, /* there is no explicite type check defined yet */
  tag_exist, /* if a tag exists, but should not (only used by fc_notag) */
  tag_does_not_exist, /* if a tag does not exist, but should (only used by fc_tag_quiet) */
  tagerror_encoded_as_value_excited_space, /* tag encoded as value instead as offset, but there is no enough space to hold it, pE. ICC-Profile encoded without offset */
  tagerror_pcre_nomatch, /*  regex does not match value */
  pcre_compile_error, /* regex is wrong */
  tagerror_multiple_zeros_in_asciivalue, /* doubled \0 in string */
  tagerror_denominator_is_zero_in_fract,
  ifderror_offset_used_twice,
  ifderror_multiple_ifd_detected,
  ifderror_tags_not_in_ascending_order,
  tagerror_offset_not_word_aligned,
  ifderror_offset_not_word_aligned,
  iccerror_profileclass= ICC_ERROR_OFFSET + 0, /* profile class ('%s'), should be one of following strings for device classes: 'scnr', 'mntr', 'prtr' or for profile classes: 'link', 'spac', 'abst', 'nmcl'" */
  iccerror_colorspacedata= ICC_ERROR_OFFSET + 1, /* colorspace data ('%s'), should be one of following strings: 'XYZ ' 'Lab ' 'Luv ' 'YCbr' 'Yvx ' 'RGB ' 'GRAY' 'HSV ' 'HLS ' 'CMYK' 'CMY ' '2CLR' '3CLR' '4CLR' '5CLR' '6CLR' '7CLR' '8CLR' '9CLR' 'ACLR' 'BCLR' 'CCLR' 'DCLR' 'ECLR' 'FCLR'" */
  iccerror_connectionspacedata= ICC_ERROR_OFFSET + 2, /* "connection space data ('%s') should be one of following strings: 'XYZ ' 'Lab '" */
  iccerror_primaryplatformsignature= ICC_ERROR_OFFSET + 3, /* primary plattform signature ('%s') should be empty or one of following strings: 'APPL', 'MSFT', 'SGI ', 'SUNW', 'TGNT' */
  iccerror_header_1v43_2010= ICC_ERROR_OFFSET + 4, /* Invalid ICC profile 1v43_2010, see http://www.color.org/specification/ICC1v43_2010-12.pdf for details  */
  iccerror_header_v240_v430= ICC_ERROR_OFFSET + 5, /* Invalid ICC profile ICC.1:2001-04, see http://www.color.org/ICC_Minor_Revision_for_Web.pdf for details */
  iccerror_header_generic= ICC_ERROR_OFFSET + 6, /* size < 10 */
  iccerror_preferredcmmtype= ICC_ERROR_OFFSET + 7, /* preferred cmmtype ('%s') should be empty or (possibly, because ICC validation is alpha code) one of following strings: 'ADBE' 'ACMS' 'appl' 'CCMS' 'UCCM' 'UCMS' 'EFI ' 'FF  ' 'EXAC' 'HCMM' 'argl' 'LgoS' 'HDM ' 'lcms' 'KCMS' 'MCML' 'WCS ' 'SIGN' 'RGMS' 'SICC' 'TCMM' '32BT' 'WTG ' 'zc00'" */
  iccerror_committed_size_differs   = ICC_ERROR_OFFSET + 8,
  iccerror_header_v500_2016 = ICC_ERROR_OFFSET +9,
  iccerror_header_version_undefined = ICC_ERROR_OFFSET +10,
  iccerror_header_version_outdated = ICC_ERROR_OFFSET +11,
  tiff_seek_error_header,
  tiff_read_error_header,
  tiff_seek_error_offset,
  tiff_read_error_offset,
  tiff_byteorder_error, /* not an expected byteorder found */
  tiff_ifd0_offset_must_be_greater_than_eight, /* must be greater than 8, because first 8 Bytes contains the TIFF header */
  code_error_streampointer_empty,
  code_error_filedescriptor_empty,
  code_error_ctif_empty,
  parser_error_wrong_function_found_in_parser_state_exe_stack,
  parser_logicalor_error,
  tagerror_expected_offsetdata,
  tagerror_count_results_in_offsets_overflow, /* example: rational is defined as 2 uint32. offset is defined as uint32. If we read count>2147483647 we got offset overflow */
  parser_logical_combine_open, /* if fc_logicalcombine was called first (no error) */
  parser_logical_combine_close, /* if fc_logicalcombine was called first (no error) */
  tagerror_offset_is_zero, 


} returncode_t;

typedef struct ret_s {
  returncode_t returncode;
  char * value_found;
  int logical_or_count;
} ret_t;



typedef struct ifd_entry_s {
  uint32 count; /*  count of data */
  TIFFDataType datatype;
  enum{ is_offset, is_value, is_error } value_or_offset;
  union {
    uint32 data32;
    uint16 data16[2];
    uint8 data8[4];
    uint32 data32offset;
  };
} ifd_entry_t;

typedef struct offset_s {
  uint32 count; /*  count of data */
  TIFFDataType datatype;
  union {
    uint8  *data8p;
    uint16 *data16p;
    uint32 *data32p;
    char   *datacharp;
    int8   *datas8p;
    int16  *datas16p;
    int32  *datas32p;
    float  *datafloatp;
    double *datadoublep;
    uint64 *data64p;
    int64  *datas64p;
  };
} offset_t;


typedef enum {
	mt_unused, /* memory areas, which are not referenced within TIFF file */
	mt_constant, /* constant values, which are fix part of TIFF file */
	mt_ifd, /* memory areas, which are parts of the IFD (but no value!) */
	mt_offset_to_ifd0,
	mt_offset_to_ifd, /* offset to nex ifd */
	mt_ifd_embedded_standardized_value, /* memory areas, with standardized values embedded in ifd */
	mt_ifd_embedded_registered_value, /* memory areas, with registered values embedded in ifd */
	mt_ifd_embedded_private_value, /* memory areas, with private values embedded in ifd */
	mt_ifd_offset_to_standardized_value, /* memory areas, which points to standardized values */
	mt_ifd_offset_to_registered_value, /* memory areas, which points to registered values */
	mt_ifd_offset_to_private_value, /* memory areas, which points to private values */
        mt_ifd_offset_to_stripoffsets, /* offsets which points to stripoffsets */
        mt_stripoffset_value, /* offsets which points to stripoffset values, hint: if compression is used stripbytecounts holds only decompressed values! */
	mt_standardized_value, /* memory areas, which contains standardized values */
	mt_registered_value, /* memory areas, which contains registered values */
	mt_private_value, /* memory areas, which contains private values */
	mt_END_marker
} memtype_t;

static const char* memtype_string[] = {
	"unused/unknown", /* memory areas", which are not referenced within TIFF file */
	"constant", /* constant values", which are fix part of TIFF file */
	"ifd", /* memory areas", which are parts of the IFD (but no value!) */
	"offset_to_ifd0", /* offset to nex ifd */
	"offset_to_ifd", /* offset to nex ifd */
	"ifd_embedded_standardized_value", /* memory areas", with standardized values embedded in ifd */
	"ifd_embedded_registered_value", /* memory areas", with registered values embedded in ifd */
	"ifd_embedded_private_value", /* memory areas", with private values embedded in ifd */
	"ifd_offset_to_standardized_value", /* memory areas", which points to standardized values */
	"ifd_offset_to_registered_value", /* memory areas", which points to registered values */
	"ifd_offset_to_private_value", /* memory areas", which points to private values */
        "ifd_offset_to_stripoffsets", /* offsets which points to stripoffsets */
        "stripoffset_value", /* stripoffset values, hint: if compression is used stripbytecounts holds only decompressed values! */
	"standardized_value", /* memory areas", which contains standardized values */
	"registered_value", /* memory areas", which contains registered values */
	"private_value", /* memory areas", which contains private values */
};

typedef struct mem_map_entry_s {
	uint32 offset; /* address within the tiff */
	uint32 count; /* count of bytes beginning with offset */
	memtype_t mem_type; /* type of memory */
} mem_map_entry_t;

typedef struct mem_map_s {
	int count;
	mem_map_entry_t * base_p;
	uint32 max_len; /* TIFF length */
	int max_entries;
} mem_map_t;


#define MAXSTRLEN 1024
#define EXPECTSTRLEN 500
#define VALUESTRLEN 500
#define TIFFAILSTRLEN (EXPECTSTRLEN + VALUESTRLEN)
#define MAXRESULT 200000
returncode_t tifp_check( ctiff_t * ctif);
/* 
ret_t tif_returns(tag_t tag, const char* expected, const char* value);
ret_t tif_fails_tag(tag_t tag, const char* expected, const char* value);
ret_t tif_fails(const char* fail_message);
ret_t tif_fails_by_returns( ret_t ret );
ret_t tif_rules_tag(tag_t tag, const char *msg);
ret_t tif_rules(const char *msg);
ret_t tif_files(const char *msg);
ret_t tif_no_tag(tag_t tag);
ret_t _empty_result();
*/

const char * float2str(float v);
const char* tag2str(tag_t tag);
const char* int2str(int v);
const char* frac2str(int d, int n);
const char* range2str(int d, int n);
off_t ct_seek(ctiff_t * ctif, off_t pos, int whence);
ssize_t ct_read(ctiff_t * ctif, void *buf, size_t count);
ret_t check_tag_has_some_of_these_values( ctiff_t * ctif, tag_t tag, int count, const unsigned int * values);
ret_t check_tag_has_valuelist( ctiff_t * ctif, tag_t tag, int count, const unsigned int * values);
ret_t check_tag_has_value_in_range(ctiff_t * ctif, tag_t tag, unsigned int a, unsigned int b);
ret_t check_tag_has_value(ctiff_t * ctif, tag_t tag, unsigned int value);
ret_t check_tag_has_value_quiet(ctiff_t * ctif, tag_t tag, unsigned int expected_value);
ret_t check_tag(ctiff_t * ctif, tag_t tag);
ret_t check_tag_quiet(ctiff_t * ctif, tag_t tag);
ret_t check_notag(ctiff_t * ctif, tag_t tag);
ret_t check_tag_has_valid_type(ctiff_t * ctif, tag_t tag);
ret_t check_datetime(ctiff_t * ctif);
ret_t check_icc(ctiff_t * ctif);
ret_t check_has_only_one_ifd(ctiff_t * ctif);
ret_t check_tagorder(ctiff_t * ctif);
ret_t check_tag_has_valid_asciivalue(ctiff_t * ctif, tag_t tag); 
ret_t check_tag_has_value_matching_regex(ctiff_t * ctif, tag_t tag, const char* regex_string);
ret_t check_all_offsets_are_word_aligned(ctiff_t * ctif);
ret_t check_all_offsets_are_used_once_only(ctiff_t * ctif);
ret_t check_all_offsets_are_greater_zero(ctiff_t * ctif);
ret_t check_all_IFDs_are_word_aligned(ctiff_t * ctif);


mem_map_t * scan_mem_map(ctiff_t * ctif) ;
void print_mem_map( mem_map_t * memmap_p);
void print_mem_stats( mem_map_t * memmap_p);
#endif
/* _FIXIT_TIFF_CHECK */
