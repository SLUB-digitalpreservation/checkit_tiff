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
ret_t check_tag(ctiff_t * ctif, tag_t tag) {
  ret_t res;
  res.returnmsg=NULL;
  res.returncode=0;
  tifp_check( ctif);
  tif_rules_tag(tag, "exists");
  res = check_tag_quiet( ctif, tag);
  if (res.returncode == 0) {
    return check_tag_has_valid_type( ctif, tag);
  } else {
    return tif_fails_tag(tag, "", "was not found, but requested because defined");
  }
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
