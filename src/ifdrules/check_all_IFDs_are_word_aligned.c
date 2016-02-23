/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>


/* check if IFDs are word aligned */
ret_t check_all_IFDs_are_word_aligned(TIFF * tif) {
  printf("check if all IFDs are word aligned\n");
  uint32 ifd = get_first_IFD( tif ); /*  TODO: check all other IFDs, too */
  if ( 0 != (ifd & 1)) {
    // FIXME: tif_fails?
    char array[80];
    snprintf(array, sizeof(array), "offset of first IFD points to 0x%08x and is not word-aligned", ifd);
    return tif_fails(array);
  }
  ret_t res;
  res.returnmsg=NULL;
  res.returncode=0;
  return res;
}
