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

/*  check if only one IFD exists */
ret_t check_has_only_one_ifd(TIFF* tif) {
  //printf("check if only one IFD exists\n");
  tif_rules("only one IFD exists");
  /* next commented lines, because TIFFNumberOfDirectories are in endless loop,
   * if the TIFF file from https://github.com/EasyinnovaSL/DPFManager/blob/develop/src/test/resources/IFD%20struct/Circular%20E.tif
   * is read:
     if (1 == TIFFNumberOfDirectories( tif )) {
     ret_t res;
     res.returnmsg=NULL;
     res.returncode=0;
     return res;
     }*/
  //int fd = TIFFFileno( tif);
  /* seek the image file directory (bytes 4-7) */
  uint32 offset = get_first_IFD( tif );
  // printf("diroffset to %i (0x%04lx)\n", offset, offset);
  //printf("byte swapped? %s\n", (TIFFIsByteSwapped(tif)?"true":"false")); 
  thandle_t client = TIFFClientdata(tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
  if (! seekproc) {
	  perror ("could not get TIFFGetSeekProc");
  }
  if (! readproc) {
	perror ("could not get TIFFGetReadProc");
  }

   

  /* read and seek to IFD address */
  // lseek(fd, (off_t) offset, SEEK_SET);
  seekproc(client, offset, SEEK_SET);

  uint16 count;
  /*
  if (read(fd, &count, 2) != 2) {
    perror ("TIFF Header read error2");
    exit(EXIT_FAILURE);
  }
  */
  if ( readproc( client, &count, 2) != 2 ) {
	  perror ("TIFF Header read error2");
	  exit( EXIT_FAILURE );
  }

  if (TIFFIsByteSwapped(tif))
    TIFFSwabShort(&count);

  //lseek(fd, 12 * count, SEEK_CUR);
  seekproc(client, 12*count, SEEK_CUR);

  /* next 4 bytes are the new IFDn entry, should be empty */
  uint32 IFDn;
  /*
  if (read(fd, &IFDn, 4) != 4) {
    perror ("TIFF Header read error3");
    exit(EXIT_FAILURE);
  }
  */
  if ( readproc( client, &IFDn, 4) != 4 ) {
	  perror ("TIFF Header read error3");
	  exit( EXIT_FAILURE );
  }

  if (TIFFIsByteSwapped(tif))
    TIFFSwabLong(&IFDn);
  if (0 == IFDn) {
    ret_t res;
    res.returnmsg=NULL;
    res.returncode=0;
    return res;
  } else {
    // FIXME: tif_fails?
      char array[160];
      snprintf(array, sizeof(array), "baseline TIFF should have only one IFD, but IFD0 at 0x%08x has pointer to IFDn 0x%08x", offset, IFDn );
      return tif_fails( array);
  }
}
