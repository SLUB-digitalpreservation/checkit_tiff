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
  tifp_check( ctif);
  ret_t res;
  res.returnmsg=NULL;
  if (-1 < TIFFGetRawTagListIndex(ctif, tag)) {
      res.returncode=0;
      return res;
  }
  res.returncode=1;
  return res;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
