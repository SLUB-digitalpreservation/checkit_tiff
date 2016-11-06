/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/16
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include "validate_icc.h"
#include "ctype.h"
/** checks a ICC tag, see Annex B of http://www.color.org/specification/ICC1v43_2010-12.pdf
 */
ret_t check_icc(ctiff_t * ctif ) {
  // printf("DEBUG: check_icc()\n");
  tifp_check( ctif);
  //printf("DEBUG: check_icc() 2\n");
  tif_rules_tag(TIFFTAG_ICCPROFILE, "is correct");
  ifd_entry_t ifd_entry = TIFFGetRawIFDEntry(ctif, TIFFTAG_ICCPROFILE);
  // printf("DEBUG: count=%i\n", ifd_entry.count);
  // printf("DEBUG: datatype=%i\n", ifd_entry.datatype);
  uint32 icc_profile_size;
  uint32 count;
  char * icc_profile = NULL;
 //uint32 retval = TIFFGetField (ctif->tif, TIFFTAG_ICCPROFILE, &icc_profile_size, &icc_profile);
   switch (ifd_entry.datatype) { /* icc datatype should be undefined (val=7) */
      case TIFF_UNDEFINED: {
                       icc_profile_size = ifd_entry.count;
                       count = ifd_entry.count;
                     /*  offset */
                      if (ifd_entry.value_or_offset == is_offset) {
                        offset_t offset = read_offsetdata(ctif, ifd_entry.data32offset, count, ifd_entry.datatype);
                        icc_profile = (char *)offset.data32p;
                      } else {
                        perror("Should not occure");
                      }


                       break;
                     }
    default: { /*  none */
               char array[VALUESTRLEN];
               snprintf(array, sizeof(array), " but was datatype:%u",  ifd_entry.datatype);
               return tif_returns( TIFFTAG_ICCPROFILE, "of type undefined", array );
               break;

             };
  }

  //printf("DEBUG: iccprofile_size=%i\n", icc_profile_size);
  // printf("DEBUG: iccprofile='%s'\n", icc_profile);
  /* DEBUG
  char * p = icc_profile;
  int i=0;
  for (i = 0; i< icc_profile_size; i++, p++) {
    if (0 == i % 8) printf("|");
    printf("%c(%0x) ", (isalnum(*p)?*p:' '),*p);
  }
  printf("\n");
  */
#define ERRSIZE 1024
  char * errmessage = malloc(sizeof(char) * ERRSIZE);
  unsigned long errsize = ERRSIZE;
  int ret = parse_icc(icc_profile_size, icc_profile, errsize, errmessage);
  if (0 != ret) {
    return tif_fails_tag( TIFFTAG_ICCPROFILE, "pointing to valid ICC profile", errmessage);
  }
  free (errmessage);
  ret_t res;
  res.returnmsg=NULL;
  res.returncode=0;
  return res;
}

