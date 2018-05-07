/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
/* 
#define DEBUG 
*/

/* checks if TIF with tag and type ASCII */
ret_t check_tag_has_valid_asciivalue(ctiff_t * ctif, tag_t tag) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  ret=check_tag_quiet(ctif, tag);
  if (ret.returncode != is_valid) return ret;

  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
#ifdef DEBUG
  printf("### datatype=%i \n", datatype);
#endif
  char *string=NULL;
  uint32 count=0;
  int r = 0;
  if (datatype == TIFF_ASCII) {
    ret = TIFFGetFieldASCII(ctif, tag, &string, &count);
    if (0 < count) { /* there exists a tag */
      for (uint32 i=0; i<count; i++) {
        if (string[i] == '\0') {
          r = i+1;
          break;
        }
      }
    } else {
      ret.returncode = tagerror_expected_count_iszero;
      return ret;
    }
  } else {
    ret = set_value_found_ret(&ret, TIFFTypeName(datatype));
    ret.returncode = tagerror_unexpected_type_found;
    return ret;
  }
  if (0 != r) {
    char array[VALUESTRLEN];
    snprintf(array, sizeof(array), "'%s' (\\0 at position %i in %u-len)", string, r, count);
    ret = set_value_found_ret(&ret, array);
    ret.returncode = tagerror_multiple_zeros_in_asciivalue;
    return ret;
  } else {
    ret.returncode=is_valid;
    return ret;
  }
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
