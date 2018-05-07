/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2018
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <unistd.h>
#include <errno.h>

/* checks the geo tiff tags:
 * - geokey directory tag (34735) -> geokeys
 * - geodouble params tag (34736)
 * - geoascii params tag  (34737)
 *
 *   +-------+
 *   | 34735 |
 *   +-------+
 *   | SHORT |
 *   | N     | N=4*n, n>=1
 *   | Offset|----,                  +-------+
 *   +-------+    |             .--->| 34736 |
 *                |             |    +-------+
 *         / +---------+        |    |DOUBLE |     +--------+
 *        /  |KeyDirVer|        |    |Count  |     |Value 1 |
 *      n=1  |KeyRev.  |        |    |Offset |---->|Value 2 |
 *           |MinorRev.|        |    +-------+     :        :
 *        \  |NumOfKeys| => x   |                  |Value n |
 *         \ +---------+        |    +-------+     +--------+
 *           :         :        +--->| 34737 |
 *        /  +---------+        |    +-------+
 *       /   |Key ID   |        |    |ASCII  |     +--------+
 * n=2.. x+1 |TifTagLoc| -------'    |Count  |     |Value 1 |
 *           |Count    |             |Offset |---->|Value 2 |
 *       \   |Val/Offst|             +-------+     :        :
 *        \  +---------+                           |Value n |
 *           :         :                           +--------+
 *
 *
 * TiffTagLoc - if 0, then value is SHORT and stored in Val/Offst, could also
 * point to 34735, 34736, 34737
 *
 * Key IDs are keys from GeoTIFF reference http://geotiff.maptools.org/spec/geotiff2.7.html#2.7 and should be sorted
 *
 *
 * details at http://geotiff.maptools.org/spec/geotiff2.4.html
 *
 *
 */

ret_t check_all_geotiff_tags(ctiff_t * ctif) {
  GET_EMPTY_RET(ret)
    tifp_check( ctif);
  /* first, detect if geokey directory tag exists */
  /* mark n
   * find offset for geokeys
   * check if number of keys in header
   * check all offsets
   */
  if (-1 < TIFFGetRawTagListIndex(ctif, 34735)) {
    ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, 34735);
    /*  only one ifd? */
    if (ifd_entry.count < 4) {
      ret = set_value_found_ret(&ret, int2str(ifd_entry.count));
      ret.returncode = tagerror_expected_count_less_than_four;
      return ret;
    }
    if (ifd_entry.count % 4 != 0) {
      ret = set_value_found_ret(&ret, int2str(ifd_entry.count));
      ret.returncode = tagerror_expected_count_not_a_multiple_of_four;
      return ret;
    }
    uint32 count = ifd_entry.count;
unsigned int v[count];
char value[VALUESTRLEN];
  memset(value, '\0', VALUESTRLEN);
 
    /*  type is short? */
    switch (ifd_entry.datatype) {
      case TIFF_SHORT: {
                         /*  offset */
                         if (ifd_entry.value_or_offset == is_offset) {
                           offset_t offset;
                           ret = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype, &offset, &ret);
                           if (ret.returncode != is_valid) {
                             return ret;
                           }
                           uint16 * p = offset.data16p;
                           uint16 * q = p;
                           q+=3;
                           uint16 num_of_keys = *(q);
                           // printf("num_of_keys=%i\n", num_of_keys);
                           if ( /* check if "count" and "NumberOfKeys" consistent */
                               ((uint32) num_of_keys * 4) != (count - 4)
                           ) {
                             snprintf(value, sizeof(value), "NumberOfKeys (4*%u=%u) differs with count of tag entries (%u-4=%u) for GeoKeyDirectory", num_of_keys, ((uint32) num_of_keys * 4),  count, (count - 4));
                             ret = set_value_found_ret(&ret, value);
                             ret.returncode = tagerror_value_differs;
                             return ret;
                           }

                           /* TODO: check the rest of the GeoKeyTag, therefore  we ignore the first 4 values */
                           for (int i=4; i< count; i++) {
#ifdef DEBUG
                             uint16 pval = *p;
                             printf("SHORTOFFSET (tag=%i): v[%i]=%u p[%i]=0x%04x\n", tag, i,v[i],i,pval);
#endif
                             p++;
                           }
                         } else {
                           ret = set_value_found_ret(&ret, "Offset expected");
                           ret.returncode = tagerror_expected_offsetdata;
                           return ret;
                         }
                         break;
                       }
      default: { /*  none */
                 ret = set_value_found_ret(&ret, TIFFTypeName(ifd_entry.datatype));
                 ret.returncode = tagerror_unexpected_type_found;
                 return ret;
                 /* break; */

               };
    };
  }
  ret.returncode=is_valid;
  return ret;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
