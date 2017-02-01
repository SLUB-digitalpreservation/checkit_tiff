/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
/* #define DEBUG */


/* checks if TIF has a specified tag */
ret_t check_tag_quiet(ctiff_t * ctif, tag_t tag) {
 ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }

  tifp_check( ctif);
  if (-1 < TIFFGetRawTagListIndex(ctif, tag)) {
      ret.returncode=is_valid;
  } else {
    ret.returncode=tag_does_not_exist;
  }
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
