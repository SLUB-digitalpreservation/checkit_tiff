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

typedef struct retmsg_s {
  enum{ rm_default, rm_file, rm_rule, rm_tag, rm_value, rm_expected, rm_hard_error, rm_error, rm_warning } rm_type;
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

#define MAXSTRLEN 1024

void tifp_check( TIFF * tif);
ret_t tif_returns(tag_t tag, const char* expected, const char* value);
ret_t tif_fails_tag(tag_t tag, const char* expected, const char* value);
ret_t tif_fails(const char* fail_message);
ret_t tif_fails_by_returns( ret_t ret );
ret_t tif_rules_tag(tag_t tag, const char *msg);
ret_t tif_rules(const char *msg);
ret_t tif_files(const char *msg);

const char * float2str(float v);
const char* tag2str(tag_t tag);
const char* int2str(int v);
const char* frac2str(int d, int n);
const char* range2str(int d, int n);

ret_t check_tag_has_some_of_these_values( TIFF* tif, tag_t tag, int count, unsigned int * values);
ret_t check_tag_has_valuelist( TIFF* tif, tag_t tag, int count, unsigned int * values);
ret_t check_tag_has_value_in_range(TIFF* tif, tag_t tag, unsigned int a, unsigned int b);
ret_t check_tag_has_value(TIFF* tif, tag_t tag, unsigned int value);
ret_t check_tag_has_value_quiet(TIFF* tif, tag_t tag, unsigned int value);
ret_t check_tag(TIFF* tif, tag_t tag);
ret_t check_tag_quiet(TIFF* tif, tag_t tag);
ret_t check_notag(TIFF* tif, tag_t tag);
ret_t check_tag_has_valid_type(TIFF* tiff, tag_t tag);
ret_t check_datetime(TIFF* tif);
ret_t check_has_only_one_ifd(TIFF* tif);
ret_t check_tagorder(TIFF* tif);
ret_t check_tag_has_valid_asciivalue(TIFF* tif, tag_t tag); 
ret_t check_tag_has_value_matching_regex(TIFF* tif, tag_t tag, const char* value);
ret_t check_all_offsets_are_word_aligned(TIFF * tif);
ret_t check_all_offsets_are_used_once_only(TIFF * tif);
ret_t check_all_IFDs_are_word_aligned(TIFF * tif);
#endif
/* _FIXIT_TIFF_CHECK */
