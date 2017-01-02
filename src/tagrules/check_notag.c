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

/* checks if TIF does not have a specified tag,
 * needed only for checks to ensure whitelist */
ret_t check_notag(ctiff_t * ctif, tag_t tag) {
  tifp_check( ctif);
    ret_t res = check_tag_quiet( ctif, tag);
  if (res.returncode == 0) {
    return tif_no_tag( tag);
  } else {
    res.returnmsg=NULL;
    res.returncode=0;
    return res;
  }
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
