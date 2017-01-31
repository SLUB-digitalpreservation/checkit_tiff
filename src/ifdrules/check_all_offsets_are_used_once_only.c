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


/* check if offsets are used only once */
ret_t check_all_offsets_are_used_once_only(ctiff_t * ctif) {
  ret_t ret;
  ret.value_found = malloc(VALUESTRLEN);
  if (NULL == ret.value_found) {
    ret.returncode=could_not_allocate_memory;
    return ret;
  }
  tifp_check( ctif);
  int count = get_ifd0_count( ctif);
  int tagidx;
  uint32 offsets[ count ];
  uint32 tags[ count ];
  int count_of_offsets = 0;
  int i;

  for (tagidx = 0; tagidx< count; tagidx++) {
    offsets[ tagidx ] = 0;
    tags[ tagidx ] = 0;
  }
  for (tagidx = 0; tagidx< count; tagidx++) {
    ifd_entry_t ifd_entry = TIFFGetRawTagIFDListEntry( ctif, tagidx );
    if (ifd_entry.value_or_offset==is_offset) {
      uint32 offset = ifd_entry.data32offset;
      uint32 tag = TIFFGetRawTagListEntry( ctif, tagidx);
      for (i=0; i< count_of_offsets; i++) {
        if (offsets[ i ] == offset) {
          // FIXME: tif_fails?
          char array[TIFFAILSTRLEN];
          snprintf(array, sizeof(array), "offset of tag %u (%s) points to %08x, which address is used previously by tag %u (%s)", tag, TIFFTagName(tag), offset, tags[i], TIFFTagName(tags[i]) );
          ret.value_found = strncpy(ret.value_found, array, VALUESTRLEN);
          ret.returncode = ifderror_offset_used_twice;
          return ret;
        }
      }
      offsets[ ++count_of_offsets ] = offset;
      tags[ count_of_offsets ] = tag;
    }
  }
  ret.returncode=is_valid;
  return ret;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
