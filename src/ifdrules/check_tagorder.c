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

ret_t check_tagorder(ctiff_t * ctif) {
  tif_rules("tags are in ascending order");
  thandle_t client = (ctif->tif);


  uint32 offset = get_ifd0_pos(ctif);
  int count = get_ifd0_count(ctif);

  /* read count of tags (2 Bytes) */
  int i;
  /* replace i/o operatrions with in-memory-operations */
  uint8 * ifdentries = NULL;
  ifdentries = malloc ( sizeof(uint8) * 12 * count);
  lseek(client, offset+2, SEEK_SET);

  if ( read( client, ifdentries, 12 * count) != 12*count ) {
    perror ("TIFF Header read error5");
    exit( EXIT_FAILURE );
  }

  uint8 * e = ifdentries;
  uint16 lasttag = 0;
  for (i = 0; i<count; i++) {
    uint8 lo = *e;
    e++;
    uint8 hi = *e;
    uint16 tag = (hi << 8) + lo;
    e++;
    if (is_byteswapped(ctif))
      TIFFSwabShort(&tag);
    if (i>0 && lasttag >= tag) {
      // printf("tag idx=%i, tag=%u (0x%04x) (0x%02x) (0x%02x)\n", i, tag, tag, hi, lo);
      free( ifdentries );
      // FIXME: tif_fails?
      char array[TIFFAILSTRLEN];
      snprintf(array, sizeof(array), "Invalid TIFF directory; tags are not sorted in ascending order, previous tag:%u (%s) , actual tag:%u (%s) at pos %i of %i\n", lasttag,  TIFFTagName(lasttag),  tag,  TIFFTagName(tag), i, count);
      return tif_fails(array);
    }
    lasttag = tag;
    e+=10;
  }
  /* loop each tag until end or given tag found */
  free( ifdentries );
  ret_t res;
  res.returnmsg=NULL;
  res.returncode=0;
  return res;
}


