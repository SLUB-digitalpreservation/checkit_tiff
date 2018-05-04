/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-18
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
/* #define DEBUG */

/* checks if TIF does not have a specified tag,
 * needed only for checks to ensure whitelist */
ret_t check_notag(ctiff_t * ctif, tag_t tag) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  if (-1 == TIFFGetRawTagListIndex(ctif, tag)) {
      ret.returncode = is_valid;
  } else {
    char msg[VALUESTRLEN];
    snprintf(msg, VALUESTRLEN, "tag %i exists", tag);
    ret = set_value_found_ret(&ret, msg);
    ret.returncode=tag_exist;
  }
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
