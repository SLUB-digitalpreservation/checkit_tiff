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
#include <errno.h>

ret_t check_tagorder(ctiff_t * ctif) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }
  tifp_check( ctif);
  if (has_sorted_tags != ctif->tagorder) {
    uint32 offset = get_ifd0_pos(ctif);
    int count = get_ifd0_count(ctif);

    /* read count of tags (2 Bytes) */
    int i;
    /* replace i/o operatrions with in-memory-operations */
    uint8 * ifdentries = NULL;
    ifdentries = malloc ( sizeof(uint8) * 12 * count);
    ct_seek(ctif, offset+2, SEEK_SET);

    if ( ct_read( ctif, ifdentries, 12 * count) != 12*count ) {
      snprintf(ret.value_found, VALUESTRLEN, "%i bytes, errorcode=%i", 12*count, errno);
      ret.returncode = tiff_read_error_header;
      return ret;
    } else {
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
          snprintf(ret.value_found, VALUESTRLEN, "previous tag:%u (%s) , actual tag:%u (%s) at pos %i of %i\n", lasttag,  TIFFTagName(lasttag),  tag,  TIFFTagName(tag), i, count);
          ret.returncode = ifderror_tags_not_in_ascending_order;
          return ret;
        }
        lasttag = tag;
        e+=10;
      }
      /* loop each tag until end or given tag found */
      free( ifdentries );
    }
  } 
  ret.returncode=is_valid;
  return ret;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
