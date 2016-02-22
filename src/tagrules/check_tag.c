/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "../check.h"
#include "../check_helper.h"
/* #define DEBUG */


/* checks if TIF has a specified tag */
ret_t check_tag_quiet(TIFF* tif, tag_t tag) {
  tifp_check( tif);
    int i;
  ret_t res;
  res.returnmsg=NULL;
  for (i= 0; i < TIFFGetRawTagListCount( tif ); i++) {
    if (tag == TIFFGetRawTagListEntry( tif, i ) && tag > 253) {
      res.returncode=0;
      return res;
    };
  }
  res.returncode=1;
  return res;
}

/* checks if TIF has a specified tag */
ret_t check_tag(TIFF* tif, tag_t tag) {
  printf("check if tag %u (%s) exists\n", tag, TIFFTagName(tif, tag));
  tifp_check( tif);
    ret_t res = check_tag_quiet( tif, tag);
  if (res.returncode == 0) {
    res = check_tag_has_valid_type( tif, tag);
    if (res.returncode == 0) {
      return check_tag_has_valid_asciivalue(tif, tag);
    }
    return tif_fails_by_returns(res);
  } else {
    return tif_fails_tag( tag2str(tif, tag), "", "was not found, but requested because defined");
  }
}

/* checks if TIF does not have a specified tag,
 * needed only for checks to ensure whitelist */
ret_t check_notag(TIFF* tif, tag_t tag) {
  tifp_check( tif);
    ret_t res = check_tag_quiet( tif, tag);
  if (res.returncode == 0) {
    return tif_fails_tag( tag2str( tif, tag), "", "which is not whitelisted or rule has no matched dependency");
  } else {
    res.returnmsg=NULL;
    res.returncode=0;
    return res;
  }
}

