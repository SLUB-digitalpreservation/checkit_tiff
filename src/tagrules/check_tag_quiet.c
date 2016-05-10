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
    int i;
  ret_t res;
  res.returnmsg=NULL;
  for (i= 0; i < get_ifd0_count( ctif ); i++) {
    if (tag > 253 && tag == TIFFGetRawTagListEntry( ctif, i )) {
      res.returncode=0;
      return res;
    };
  }
  res.returncode=1;
  return res;
}
