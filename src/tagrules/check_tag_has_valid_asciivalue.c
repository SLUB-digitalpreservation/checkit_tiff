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
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);

  TIFFDataType datatype =  TIFFGetRawTagType( ctif, tag );
#ifdef DEBUG
  printf("### datatype=%i \n", datatype);
#endif
  char *string=NULL;
  uint32 count=0;
  int r = 0;
  char * val=NULL;
  if (datatype == TIFF_ASCII) {
    int count = TIFFGetFieldASCII(ctif, tag, &val);
    if (0 < count) { /* there exists a tag */
      for (int i=0; i<count; i++) {
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
    ret.value_found = strncpy(ret.value_found, TIFFTypeName(datatype), VALUESTRLEN);
    ret.returncode = tagerror_unexpected_type_found;
    return ret;
  }
  if (0 != r) {
    char array[VALUESTRLEN];
    snprintf(array, sizeof(array), "'%s' (\\0 at position %i in %i-len)", val, r, count);
    ret.value_found = strncpy(ret.value_found, array, VALUESTRLEN);
    ret.returncode = tagerror_multiple_zeros_in_asciivalue;
    return ret;
  } else {
    ret.returncode=is_valid;
    return ret;
  }
  ret.returncode=should_not_occure;
  return ret;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
