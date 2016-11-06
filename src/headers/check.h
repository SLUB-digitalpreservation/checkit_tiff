/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#ifndef _FIXIT_TIFF_CHECK
#define _FIXIT_TIFF_CHECK
#include <stdlib.h>
#include <tiff.h>
#include <tiffio.h>

typedef struct ctiff_s {
	void * streamp; /* TODO: adress of memory-mapped tif stream */
	uint32 streamlen; /* TODO: length of memory-mapped tif stream (in bytes) */
	TIFF * tif;
	void * ifd0p; /* adress of first ifd in memory-mapped tif stream */
	uint32 ifd0pos; /* offset in tif stream (in bytes) */
	uint16 ifd0c; /* count if tags in first ifd */
	char isbyteswapped; /* true if BigEndian */
	char *filename; /* filename */
	/* TODO: add file size */
} ctiff_t;

typedef enum{ rm_default, rm_file, rm_rule, rm_tag, rm_value, rm_expected, rm_hard_error, rm_error, rm_warning, rm_precondition, rm_logicalor_error } rm_type_t;
typedef struct retmsg_s {
  rm_type_t rm_type;
  char * rm_msg;

} retmsg_t;

typedef struct ret_s {
  int returncode;
  int count;
  retmsg_t * returnmsg;
} ret_t;


typedef struct ifd_entry_s {
  uint16 count;
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
  uint16 count;
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

typedef uint16 tag_t;

typedef enum {
	unused, /* memory areas, which are not referenced within TIFF file */
	constant, /* constant values, which are fix part of TIFF file */
	ifd, /* memory areas, which are parts of the IFD (but no value!) */
	ifd_offset, /* offset to nex ifd */
	ifd_embedded_standardized_value, /* memory areas, with standardized values embedded in ifd */
	ifd_embedded_registered_value, /* memory areas, with registered values embedded in ifd */
	ifd_embedded_private_value, /* memory areas, with private values embedded in ifd */
	ifd_offset_to_standardized_value, /* memory areas, which points to standardized values */
	ifd_offset_to_registered_value, /* memory areas, which points to registered values */
	ifd_offset_to_private_value, /* memory areas, which points to private values */
	standardized_value, /* memory areas, which contains standardized values */
	registered_value, /* memory areas, which contains registered values */
	private_value, /* memory areas, which contains private values */
} memtype_t;

typedef struct mem_map_s {
	uint32 offset; /* adress within the tiff */
	uint32 count; /* count of bytes beginning with offset */
	memtype_t mem_type; /* type of memory */
} mem_map_t;
	


#define MAXSTRLEN 1024
#define EXPECTSTRLEN 160
#define VALUESTRLEN 160
#define TIFFAILSTRLEN (EXPECTSTRLEN + VALUESTRLEN)
#define MAXRESULT 200000
void tifp_check( ctiff_t * ctif);
ret_t tif_returns(tag_t tag, const char* expected, const char* value);
ret_t tif_fails_tag(tag_t tag, const char* expected, const char* value);
ret_t tif_fails(const char* fail_message);
ret_t tif_fails_by_returns( ret_t ret );
ret_t tif_rules_tag(tag_t tag, const char *msg);
ret_t tif_rules(const char *msg);
ret_t tif_files(const char *msg);
ret_t tif_no_tag(tag_t tag);

ret_t _empty_result();

const char * float2str(float v);
const char* tag2str(tag_t tag);
const char* int2str(int v);
const char* frac2str(int d, int n);
const char* range2str(int d, int n);

ret_t check_tag_has_some_of_these_values( ctiff_t * ctif, tag_t tag, int count, unsigned int * values);
ret_t check_tag_has_valuelist( ctiff_t * ctif, tag_t tag, int count, unsigned int * values);
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
ret_t check_all_IFDs_are_word_aligned(ctiff_t * ctif);


mem_map_t * scan_mem_map(ctiff_t * ctif) ;
#endif
/* _FIXIT_TIFF_CHECK */
