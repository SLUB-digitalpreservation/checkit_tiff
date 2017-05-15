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


/* check if offsets are word aligned */
ret_t check_all_offsets_are_word_aligned(ctiff_t * ctif) {
  GET_EMPTY_RET(ret)
  tifp_check( ctif);
  int count = get_ifd0_count( ctif);
  int tagidx;
  for (tagidx = 0; tagidx< count; tagidx++) {
    ifd_entry_t ifd_entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    if (ifd_entry.value_or_offset==is_offset) {
      uint32 offset = ifd_entry.data32offset;
      if ( 0 != (offset & 1)) {
        uint32 tag = TIFFGetRawTagListEntry( ctif, tagidx);
        // FIXME: tif_fails?
        char array[TIFFAILSTRLEN];
        snprintf(array, sizeof(array), "tag %i pointing to 0x%08x and is not word-aligned", tag, offset);
        ret = set_value_found_ret (&ret, array);
        ret.returncode = tagerror_offset_not_word_aligned;
        return ret;
      }
    }
  }
  ret.returncode=is_valid;
  return ret;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
