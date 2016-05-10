/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <pcre.h>
#include <string.h>

/*
#define DEBUG
*/

ret_t check_tag_has_valuelist(ctiff_t * ctif, tag_t tag, int count, unsigned int * values) {
  //printf("check if tag %u (%s) has these %i-values", tag, TIFFTagName(tif, tag), count);
  tifp_check( ctif);
  char msg[200];
  snprintf(msg, sizeof(msg), "has these %i-values: ", count);
  int i;
  unsigned int * p = values;
  for (i=0; i< count; i++) {
    if (0 < i) secstrcat (msg, ", ", 200);
    secstrcat (msg, int2str(*p), 200);
    p++;
  }
  tif_rules_tag(tag, strdup(msg));
  ret_t res = check_tag_has_valid_type( ctif, tag);
 if (res.returncode == 0) {

  int i;
  unsigned int v[count];
  for (i=0; i< count; i++) {
    v[i] = *values;
    values++;
  }
  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, tag);
  if (count != ifd_entry.count) {
    char expected[10];
    snprintf(expected, sizeof(expected), "list has %u values", count);
    char value[10];
    snprintf(value, sizeof(value), "has %u values", ifd_entry.count);
    return tif_fails_tag( tag, strdup(expected), strdup(value));
  }
  ret_t res;
  res.returncode=0;
  res.returnmsg=NULL;
  switch (ifd_entry.datatype) {
    case TIFF_LONG: {
                      /*  value */
                      if (ifd_entry.value_or_offset == is_value) {
                        for (i=0; i< count; i++) {
                          if (v[i] != ifd_entry.data32) {
                            char expected[10];
                            snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                            char value[10];
                            snprintf(value, sizeof(value), "has value[%u]=%u", i,  ifd_entry.data32);
                            return tif_fails_tag( tag, strdup(expected), strdup(value));
                            //tif_fails_tag( tag2str(tif, tag), "tag %u (%s), tagvalue[%i]=%u differs from value=%u (long)\n",  tag, TIFFTagName(tif, tag), i, ifd_entry.data32, v[i]);
                          }
                        }
                      }
                      /*  offset */
                      if (ifd_entry.value_or_offset == is_offset) {
                        offset_t offset = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype);
                        uint32 * p = offset.data32p;
                        for (i=0; i< count; i++) {
                          uint32 pval = *p;
                          if (is_byteswapped(ctif))
                            TIFFSwabLong(&pval);
#ifdef DEBUG
                          printf("OFFSET: v[%i]=%u p[%i]=%u\n", i,v[i],i,pval);
#endif
                          if (v[i] != *p) {
                            char expected[10];
                            snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                            char value[10];
                            snprintf(value, sizeof(value), "has value[%u]=%u", i,  pval);
                            return tif_fails_tag( tag, strdup(expected), strdup(value));
                          }
                          p++;
                        }
                      }

                      return res;
                      break;
                    }
    case TIFF_SHORT: {
                       /*  value */
                       if (ifd_entry.value_or_offset == is_value) {
                         for (i=0; i< count; i++) {
                           int c = (v[i]) == (ifd_entry.data16[i]);
                           if (!c) {
                             char expected[10];
                             snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                             char value[10];
                             snprintf(value, sizeof(value), "has value[%u]=%u", i,  ifd_entry.data16[i]);
                             return tif_fails_tag( tag, strdup(expected), strdup(value));
                             //tif_fails("tag %u (%s), tagvalue[%i]=%u differs from value[%i]=%u (short)\n",  tag, TIFFTagName(tif, tag), i, ifd_entry.data16[i], i, v[i]);
                           }
                         }
                       }
                       /*  offset */
                       if (ifd_entry.value_or_offset == is_offset) {
                         offset_t offset = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype);
                         uint16 * p = offset.data16p;
                         for (i=0; i< count; i++) {
                           uint16 pval = *p;
                           if (is_byteswapped(ctif))
                             TIFFSwabShort(&pval);
#ifdef DEBUG
                           printf("OFFSET: v[%i]=%u p[%i]=%u\n", i,v[i],i,pval);
#endif
                           if (v[i] != pval) {
                             char expected[10];
                             snprintf(expected, sizeof(expected), "value[%u]=%u", i, v[i]);
                             char value[10];
                             snprintf(value, sizeof(value), "has value[%u]=%u", i,  pval);
                             return tif_fails_tag( tag, strdup(expected), strdup(value));
                             // tif_fails("tag %u (%s), tagvalue[%i]=%u differs from value=%u (short offset)\n",  tag, TIFFTagName(tif, tag), i, pval, v[i]);
                           }
                           p++;
                         }
                       }

                       return res;
                       break;
                     }
    default: /*  none */
                      {
                        char array[10];
                        snprintf(array, sizeof(array), "type:%i", ifd_entry.datatype);
                        return tif_fails_tag( tag, "of type long, short or float", array);
                      }

  }
 }
  return tif_fails_by_returns( res );
}
