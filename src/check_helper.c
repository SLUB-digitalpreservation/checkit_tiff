/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/* 
#define DEBUG
*/



static int enabled_cache = 0;

void clear_cache () {
	enabled_cache = 0;
}


//------------------------------------------------------------------------------
ret_t check_tag_has_fvalue(TIFF*  tif, tag_t tag, float value)
{
  float val;
  int found=TIFFGetField(tif, tag, &val);
  if (1 == found) {
    if ( fabs(val - value) < 0.01 ) {
      ret_t res;
      res.returnmsg=NULL;
      res.returncode=0;
      return res;
    } else {
      tif_returns("tag %u should have value %f, but have count/value=%f\n", tag, value, val);
    }

  } else {
    tif_returns("tag %u should exist, because defined\n", tag);
  }
}

//------------------------------------------------------------------------------
ret_t check_tag_has_u16value(TIFF*  tif, tag_t tag, uint16 value)
{
  uint16 val;
  int found=TIFFGetField(tif, tag, &val);
  if (1 == found) {
    if ( val == value ) {  
      ret_t res;
      res.returnmsg=NULL;
      res.returncode=0;
      return res;
    } else {
      tif_returns("tag %u should have value %u, but have count/value=%u\n", tag, value, val);
    }

  } else {
    tif_returns("tag %u should exist, because defined\n", tag);
  }
}


//------------------------------------------------------------------------------
ret_t check_tag_has_u32value(TIFF*  tif, tag_t tag, uint32 value)
{
  uint32 val;
  int found=TIFFGetField(tif, tag, &val);
  if (1 == found) {
    if ( val == value )  {
      ret_t res;
      res.returnmsg=NULL;
      res.returncode=0;
      return res;
    } else {
      tif_returns("tag %u should have value %u, but have count/value=%u\n", tag, value, val);
    }

  } else {
    tif_returns("tag %u should exist, because defined\n", tag);
  }
}

const char * TIFFTagName( TIFF * tif, tag_t tag ) {
   const TIFFField* fieldp = TIFFFieldWithTag(tif, tag);
   if (NULL != fieldp) {
#ifndef OLDTIFF   
        return TIFFFieldName(fieldp);
#else
        char * tagstring;
        tagstring =malloc( sizeof(char) *MAXSTRLEN );
        if (NULL==tagstring) {
          fprintf(stderr, "could not allocate memory for tagstring\n");
          exit(EXIT_FAILURE);
          }; 
          snprintf (tagstring, MAXSTRLEN-1, "tag %u", tag);
	  char * ret = strdupa( tagstring );
	  free (tagstring); 
          return ret;
#endif
   } else {
	  switch (tag) {
		  case 32932: return ("TIFF annotation data"); /* http://web.archive.org/web/20050309141348/http://www.kofile.com/support%20pro/faqs/annospec.htm */
		  case 33445: return ("MD_Filetag"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33446: return ("MD_ScalePixel"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33447: return ("MD_Colortable"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33448: return ("MD_LabName"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33449: return ("MD_SampleInfo"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33450: return ("MD_PrepDate"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33451: return ("MD_PrepTime"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33452: return ("MD_FileUnits"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
		  case 33918: return ("INGR Packet Data Tag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 33919: return ("INGR Flag Registers"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 33920: return ("IrasB Transormation Matrix"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 33922: return ("ModelTiepointTag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 34264: return ("ModelTransformationTag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 34732: return ("ImageLayer"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag, see RFC2301*/
                  case 34820: return ("EMC/PixTool SpecificTag"); /* thgere is more informatioon about meaning needed. If tag exists, there was string "Untitled" encoded (as binary) */
		  case 34908: return ("HylaFax FaxRecvParams"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 34909: return ("HylaFax FaxSubAdress"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 34910: return ("HylaFax FaxRecvTime"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 37724: return ("ImageSourceData"); /* http://justsolve.archiveteam.org/wiki/PSD, http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/ */
		  case 42112: return ("GDAL_Metadata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 42113: return ("GDAL_nodata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 50215: return ("Oce Scanjob Description"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 50216: return ("Oce Application Selector"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 50217: return ("Oce Identification Number"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 50218: return ("Oce ImageLogic Characteristics"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 50784: return ("Alias Layer Metadata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
		  case 50933: return ("ExtraCameraProfiles"); /* http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_spec_1.4.0.0.pdf */
		  default: return ("undefined tag"); 
	  }
   }
}

int parse_header_and_endianess(TIFF * tif) {
  thandle_t client = TIFFClientdata(tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
  if (! seekproc) {
	  perror ("could not get TIFFGetSeekProc");
  }
  if (! readproc) {
	perror ("could not get TIFFGetReadProc");
  }

   /* seek the image file directory (bytes 4-7) */

  //lseek(fd, (off_t) 0, SEEK_SET);
  seekproc(client, 0, SEEK_SET);
  uint16 header;
  uint16 magic;
  int ret;
  /*
  if (read(fd, &header, 2) != 2) {
    perror ("TIFF Header read error");
    exit( EXIT_FAILURE );
  }
  */
  if ( readproc( client, &header, 2) != 2 ) {
	  perror ("TIFF Header read error");
	  exit( EXIT_FAILURE );
  }
  if (header == 0x4949) ret = 0; /* little endian */
  else if (header == 0x4d4d) ret = 1; /*  big endian */
  else {
    fprintf (stderr, "TIFF Header error, not Byte Order Bytes for TIFF: 0x%04x\n", header);
    if (header == 0x4550) fprintf( stderr, "could be a Microsoft Document Image file (little endian), if header begins with by 0x45 0x50 0x2a 0x00\n");
    exit(EXIT_FAILURE);
  }
  /*
  if (read(fd, &magic, 2) != 2) {
    perror ("TIFF Header read error");
    exit( EXIT_FAILURE );
  }
  */ 
  if ( readproc( client, &magic, 2) != 2 ) {
	  perror ("TIFF Header read error");
	  exit( EXIT_FAILURE );
  }

  uint16 magic2 = magic;
  if (ret) TIFFSwabShort( &magic2 ); /*  big endian */
  if (magic2 == 42) { return ret; }
  else { 
	  fprintf (stderr, "TIFF Header error, not a MAGIC BYTE for TIFF: 0x%04x\n", magic);
	  if (magic2==0x2b00) fprintf (stderr, "\tbut could be a BigTIFF, see http://www.awaresystems.be/imaging/tiff/bigtiff.html\n");
	  if (magic2==0x5500) fprintf (stderr, "\tbut could be a Panasonic Raw/RW2, see http://libopenraw.freedesktop.org/wiki/Panasonic_RAW/\n");
	  if (magic2==0xbc01) fprintf (stderr, "\tbut could be a JPEG XR, see http://www.itu.int/rec/T-REC-T.832\n");
	  if (magic2==0x4e31) fprintf (stderr, "\tbut could be a Navy Image FileFormat, see http://www.navsea.navy.mil/nswc/carderock/tecinfsys/cal-std/doc/28002c.pdf\n");
	  if (magic2==0x5243) fprintf (stderr, "\tbut could be a DNG camera profile, see http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_spec_1.4.0.0.pdf\n");
	  if (magic2==0x524f) fprintf (stderr, "\tbut could be an Olympus ORF, see http://libopenraw.freedesktop.org/wiki/Olympus_ORF/\n");
	  if (magic2==0x5253) fprintf (stderr, "\tbut could be an Olympus ORF, see http://libopenraw.freedesktop.org/wiki/Olympus_ORF/\n");
    exit(EXIT_FAILURE); 
  }
}

uint32 get_first_IFD(TIFF * tif) {
	/* memoize it */
	static uint32 memoized_offset = 0;
	static int is_memoized = 0;
	static TIFF * memoized_tif = NULL;
	//printf("get_first_IFD is memoized? %s\n", is_memoized ? "true" : "false");
	if (! is_memoized || (memoized_tif != tif) || (!enabled_cache)) {
		int isByteSwapped = parse_header_and_endianess(tif);
		/* seek the image file directory (bytes 4-7) */
		thandle_t client = TIFFClientdata(tif);
		TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
		TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
		if (! seekproc) {
			perror ("could not get TIFFGetSeekProc");
		}
		if (! readproc) {
			perror ("could not get TIFFGetReadProc");
		}
		/*lseek(fd, (off_t) 4, SEEK_SET); */
		seekproc(client, 4, SEEK_SET);
		uint32 offset;
		/*if (read(fd, &offset, 4) != 4) {
		  perror ("TIFF Header read error");
		  exit( EXIT_FAILURE );
		  }
		  */
		if ( readproc( client, &offset, 4) != 4 ) {
			perror ("TIFF Header read error");
			exit( EXIT_FAILURE );
		}
		if (isByteSwapped) {
			TIFFSwabLong (&offset);
		}
		memoized_offset = offset;
		memoized_tif = tif;
		is_memoized = 1;
		enabled_cache = 1;

	}
	return memoized_offset;
}

/* scans first IDF and returns count of tags
 * Hint: sideeffect, if succeed the seek points to beginning of the first
 * IFD-entry */
int TIFFGetRawTagListCount (TIFF * tif) {
	thandle_t client = TIFFClientdata(tif);
	TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
	TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
	if (! seekproc) {
		perror ("could not get TIFFGetSeekProc");
	}
	if (! readproc) {
		perror ("could not get TIFFGetReadProc");
	}

	//int fd = TIFFFileno( tif);
	/* seek the image file directory (bytes 4-7) */
	uint32 offset = get_first_IFD( tif );

	// printf("diroffset to %i (0x%04lx)\n", offset, offset);
	//printf("byte swapped? %s\n", (TIFFIsByteSwapped(tif)?"true":"false")); 
	/* read and seek to IFD address */
	//lseek(fd, (off_t) offset, SEEK_SET);
	seekproc(client, offset, SEEK_SET);

	uint16 count;
	/*if (read(fd, &count, 2) != 2) {
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
	return count;
}

/* scans first IDF and returns the n-th tag */
tag_t TIFFGetRawTagListEntry( TIFF  * tif, int tagidx ) {
	int count = TIFFGetRawTagListCount( tif);
	thandle_t client = TIFFClientdata(tif);
	TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
	TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
	if (! seekproc) {
		perror ("could not get TIFFGetSeekProc");
	}
	if (! readproc) {
		perror ("could not get TIFFGetReadProc");
	}
	//int fd = TIFFFileno( tif);
	//printf("count %i\n", count);
	/* read count of tags (2 Bytes) */
	int i;
	/* replace i/o operatrions with in-memory-operations */
	uint8 * ifdentries = NULL;
	ifdentries = malloc ( sizeof(uint8) * 12 * count);
	/*
	   if (read(fd, ifdentries, 12 * count) != 12*count) {
	   perror ("TIFF Header read error3");
	   exit(EXIT_FAILURE);
	   }
	   */
	if ( readproc( client, ifdentries, 12 * count) != 12*count ) {
		perror ("TIFF Header read error3");
		exit( EXIT_FAILURE );
	}

	uint8 * e = ifdentries;
	uint16 ret = 0;
	for (i = 0; i<count; i++) {
		uint8 lo = *e;
		e++;
		uint8 hi = *e;
		uint16 tagid = (hi << 8) + lo;
		e++;
		if (TIFFIsByteSwapped(tif))
			TIFFSwabShort(&tagid);
		if (i == tagidx) {
			// printf("tag idx=%i, tag=%u (0x%04x) (0x%02x) (0x%02x)\n", i, tagid, tagid, hi, lo);
			ret = tagid;
			goto LABEL1;
		}
		e+=10;
	}
LABEL1:
	/* loop each tag until end or given tag found */
	free( ifdentries );
	return ret;
}

/*
#define offset_malloc(fd, of, os, count ) {\
	of = NULL; of = malloc ( sizeof(os) * count);\
       	if (read(fd, of,  sizeof(os) * count) != sizeof(os) *count)\
	       	perror ("TIFF Offset read error2") ;\
}
*/
#define offset_malloc(fd, of, os, count ) {\
	of = NULL; of = malloc ( sizeof(os) * count);\
	  if ( readproc( client, of, sizeof(os) * count) != sizeof(os) *count ) {\
		  perror ("TIFF Offset read error2");\
		  exit( EXIT_FAILURE );\
	  }\
}


/*  get count-data datastream from offset-address */
offset_t read_offsetdata( TIFF * tif, uint32 address, uint16 count, uint16 datatype) {
  //int fd = TIFFFileno( tif);
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
  //lseek(fd, (off_t) address, SEEK_SET);
  seekproc(client, address, SEEK_SET);



#ifdef DEBUG
  printf("read_offsetdata(tif, adress=%u, count=%u, datatype=%u)\n", address, count, datatype);
#endif

  offset_t offset;
  offset.count = count;
  offset.datatype = datatype;
  switch (datatype) {
    case 1: /* 8-bit unsigned integer */
    case 7: /* !8-bit untyped data */
      /* 
      offset.data8p = NULL;
      offset.data8p = malloc ( sizeof(uint8) * count);
      if (read(fd, offset.data8p,  sizeof(uint8) * count) != sizeof(uint8) *count)
        perror ("TIFF Offset read error");
      */
      offset_malloc(fd, offset.data8p, uint8, count)
      break;
    case 2: /* 8-bit bytes w/ last byte null */
      assert( sizeof(char) == sizeof(uint8));
      offset_malloc(fd, offset.datacharp, char, count)
      break;
    case 6: /* !8-bit signed integer */
      offset_malloc(fd, offset.datas8p, int8, count)
      break;
    case 3: /* 16-bit unsigned integer */
      offset_malloc(fd, offset.data16p, uint16, count)
      break;
    case 8: /* !16-bit signed integer */
      offset_malloc(fd, offset.datas16p, int16, count)
      break;
    case 4: /* 32-bit unsigned integer */
    case 13: /* %32-bit unsigned integer (offset) */
      offset_malloc(fd, offset.data32p, uint32, count)
      break;
    case 9: /* !32-bit signed integer */
      offset_malloc(fd, offset.datas32p, uint32, count)
      break;
    case 5: /* 64-bit unsigned fraction */
      fprintf(stderr, "offsetdata datatype=%i not supported yet", datatype);
      exit(EXIT_FAILURE);
    case 10: /* !64-bit signed fraction */
      fprintf(stderr, "offsetdata datatype=%i not supported yet", datatype);
      exit(EXIT_FAILURE);
    case 11: /* !32-bit IEEE floating point */
      assert( sizeof(float) == 4);
      offset_malloc(fd, offset.datafloatp, float, count)
      break;
    case 12: /* !64-bit IEEE floating point */
      assert( sizeof(double) == 8);
      offset_malloc(fd, offset.datadoublep, double, count)
      break;
    case 16: /* BigTIFF 64-bit unsigned integer */
    case 18: /* BigTIFF 64-bit unsigned integer (offset) */
      assert( sizeof(double) == 8);
      offset_malloc(fd, offset.data64p, uint64, count)
      break;
    case 17: /* BigTIFF 64-bit signed integer */
      assert( sizeof(double) == 8);
      offset_malloc(fd, offset.datas64p, int64, count)
        break;
    default: /*  should not occure */
      fprintf(stderr, "offsetdata datatype=%i not supported yet", datatype);
      exit(EXIT_FAILURE);
  };
  return offset;
}

/* scans first IDF and returns the type of the n-th tag */
ifd_entry_t TIFFGetRawTagIFDListEntry( TIFF  * tif, int tagidx ) {
  int count = TIFFGetRawTagListCount( tif);
#ifdef DEBUG
  printf(" count of tags = %i\n", count);
#endif
  // int fd = TIFFFileno( tif);
  thandle_t client = TIFFClientdata(tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
  if (! seekproc) {
	  perror ("could not get TIFFGetSeekProc");
  }
  if (! readproc) {
	perror ("could not get TIFFGetReadProc");
  }


  //printf("count %i\n", count);
  /* read count of tags (2 Bytes) */
  int i;
  ifd_entry_t ifd_entry;
  ifd_entry.value_or_offset = is_error;
  /* replace i/o operatrions with in-memory-operations */
  uint8 * ifdentries = NULL;
  ifdentries = malloc ( sizeof(uint8) * 12 * count);
  /*
  if (read(fd, ifdentries, 12 * count) != 12*count) {
    perror ("TIFF Header read error4");
    exit(EXIT_FAILURE);
  }
  */
  if ( readproc( client, ifdentries, 12 * count) != 12*count ) {
	  perror ("TIFF Header read error4");
	  exit( EXIT_FAILURE );
  }
  uint8 * e = ifdentries;
  for (i = 0; i<count; i++) {
    uint8 lo = *e;
    e++;
    uint8 hi = *e;
    uint16 tagid = (hi << 8) + lo;
    e++;
    if (TIFFIsByteSwapped(tif))
      TIFFSwabShort(&tagid);
    if (i == tagidx) {
      // tag type check
      lo = *e; e++;
      hi = *e; e++;
      uint16 tagtype = (hi << 8) + lo;
      if (TIFFIsByteSwapped(tif))
        TIFFSwabShort(&tagtype);
      
      uint32 count = (*(e++));
      count += (*(e++) << 8);
      count += (*(e++) << 16);
      count += (*(e++) << 24);
      if (TIFFIsByteSwapped(tif))
        TIFFSwabLong( &count);
#ifdef DEBUG
printf("\ncount=%0x\n\n", count);
#endif
      
      /*  is value or offset? */
      /*  TODO */
      ifd_entry.count=count;
      ifd_entry.datatype=tagtype;
      uint8 data[4];
      data[0] = *(e++);
      data[1] = *(e++);
      data[2] = *(e++);
      data[3] = *(e++);
      uint32 value_or_offset = (data[0]);
            value_or_offset += (data[1] << 8);
            value_or_offset += (data[2] << 16);
            value_or_offset += (data[3] << 24);
            if (TIFFIsByteSwapped(tif))
              TIFFSwabLong( &value_or_offset);
      switch( tagtype) {
        case 1: /* 8-bit unsigned integer */
        case 2: /* 8-bit bytes w/ last byte null */
        case 6: /* !8-bit signed integer */
        case 7: /* !8-bit untyped data */
          if (count > 4) { /* offset */ 
            ifd_entry.value_or_offset=is_offset;
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */ 
            ifd_entry.value_or_offset=is_value;
            ifd_entry.data8[0] = data[0];
            ifd_entry.data8[1] = data[1];
            ifd_entry.data8[2] = data[2];
            ifd_entry.data8[3] = data[3];
#ifdef DEBUG
            printf("data8[0]=%u\n", data[0]);
            printf("data8[1]=%u\n", data[1]);
            printf("data8[2]=%u\n", data[2]);
            printf("data8[3]=%u\n", data[3]);
#endif
          }; break;
        case 3: /* 16-bit unsigned integer */
        case 8: /* !16-bit signed integer */
          if (count > 2) { /* offset */  
            ifd_entry.value_or_offset=is_offset; 
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */  
            ifd_entry.value_or_offset=is_value;
            uint16 w0 = (data[0]) + (data[1]<<8);
            uint16 w1 = (data[2]) + (data[3]<<8);
            if (TIFFIsByteSwapped(tif)) {
              TIFFSwabShort( &w0 ); 
              TIFFSwabShort( &w1 );
            }
            ifd_entry.data16[0] = w0;
            ifd_entry.data16[1] = w1;
#ifdef DEBUG
            printf("data16[0]=%u\n", w0);
            printf("data16[1]=%u\n", w1);
#endif
          }; break;
        case 4: /* 32-bit unsigned integer */
        case 9: /* !32-bit signed integer */
          if (count > 1) { /* offset */ 
            ifd_entry.value_or_offset=is_offset;
            ifd_entry.data32offset=value_or_offset;
          } else { /*  values */   
            ifd_entry.value_or_offset=is_value;
            ifd_entry.data32=value_or_offset;
#ifdef DEBUG
            printf("data32[0]=%u\n", value_or_offset);
#endif
          }; break;
        case 5: /* 64-bit unsigned fraction */
        case 10: /* !64-bit signed fraction */
        case 11: /* !32-bit IEEE floating point */
        case 12: /* !64-bit IEEE floating point */
	case 13: /* %32-bit unsigned integer (offset) */
	case 16: /* BigTIFF 64-bit unsigned integer */
	case 17: /* BigTIFF 64-bit signed integer */
	case 18: /* BigTIFF 64-bit unsigned integer (offset) */
          ifd_entry.value_or_offset=is_offset; 
          ifd_entry.data32offset=value_or_offset;

      }
      free( ifdentries );
#ifdef DEBUG
      printf("tag idx=%i, tag=%u (0x%04x) tagtype=0x%04x is_offset=%s count=%d value_or_offset=0x%08x\n", i, tagid, tagid, tagtype, (ifd_entry.value_or_offset==is_offset ? "true" : "false"), count, value_or_offset);
#endif
      return ifd_entry;
    }
    e+=10;
  }
  /* loop each tag until end or given tag found */
  free( ifdentries );
  return ifd_entry;
}

/*  TODO */
ifd_entry_t TIFFGetRawIFDEntry( TIFF * tif, tag_t tag) {
  int tagidx = -1;
  int i;
  for (i= 0; i < TIFFGetRawTagListCount( tif ); i++) {
    if (tag == TIFFGetRawTagListEntry( tif, i ) && tag > 253) {
      tagidx= i;
      break;
    };
  };
  ifd_entry_t ifd_entry;
  if (tagidx >= 0) {
    ifd_entry =  TIFFGetRawTagIFDListEntry( tif, tagidx );
  } else { /* tag not defined */ 
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tif, tag));
	  ifd_entry.value_or_offset = is_error;
	  ifd_entry.count = 0;
  }
  return ifd_entry;
}
/* scans first IDF and returns the type of the n-th tag */
uint32 TIFFGetRawTagTypeListEntry( TIFF  * tif, int tagidx ) {
if (tagidx >= 0) {
    ifd_entry_t ifd_entry;
    ifd_entry =  TIFFGetRawTagIFDListEntry( tif, tagidx );
    return ifd_entry.datatype;
  } else { /* tag not defined */ 
    fprintf(stderr, "tagidx should be greater equal 0");
    exit(EXIT_FAILURE);
  }
}

/* reads the datatype of given tag on specified TIFF,
 * because FieldType of libtiff does not return the true value (because it maps
 * its own datastructure), we need to use this function instead
 * @param tif pointer to TIFF structure
 * @param tag tag
 * @return datatype of given tag
 * if tag does not exists the function aborts with an error 
 */
TIFFDataType TIFFGetRawTagType(TIFF * tif, tag_t tag) {
  int tagidx = -1;
  int i;
  for (i= 0; i < TIFFGetRawTagListCount( tif ); i++) {
    if (tag == TIFFGetRawTagListEntry( tif, i ) && tag > 253) {
      tagidx= i;
      break;
    };
  };
  if (tagidx >= 0) {
    TIFFDataType datatype =  TIFFGetRawTagTypeListEntry( tif, tagidx );
#ifdef DEBUG
    printf("### datatype=%i \n", datatype);
#endif
    return datatype;
  } else { /* tag not defined */ 
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tif, tag));
          return -1;
  }
}


ret_t check_tagorder(TIFF* tif) {
  printf("check if tags are in ascending order\n");
  int count = TIFFGetRawTagListCount( tif);
  //int fd = TIFFFileno( tif);
  thandle_t client = TIFFClientdata(tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(tif);
  if (! seekproc) {
	  perror ("could not get TIFFGetSeekProc");
  }
  if (! readproc) {
	perror ("could not get TIFFGetReadProc");
  }


  //printf("count %i\n", count);
  /* read count of tags (2 Bytes) */
  int i;
  /* replace i/o operatrions with in-memory-operations */
  uint8 * ifdentries = NULL;
  ifdentries = malloc ( sizeof(uint8) * 12 * count);
  /* 
  if (read(fd, ifdentries, 12 * count) != 12*count) {
    perror ("TIFF Header read error5");
    exit(EXIT_FAILURE);
  }
  */
  if ( readproc( client, ifdentries, 12 * count) != 12*count ) {
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
    if (TIFFIsByteSwapped(tif))
      TIFFSwabShort(&tag);
    if (i>0 && lasttag >= tag) {
      // printf("tag idx=%i, tag=%u (0x%04x) (0x%02x) (0x%02x)\n", i, tag, tag, hi, lo);
      free( ifdentries );
      tif_fails("Invalid TIFF directory; tags are not sorted in ascending order, previous tag:%u (%s) , actual tag:%u (%s) at pos %i of %i\n", lasttag,  TIFFTagName(tif, lasttag),  tag,  TIFFTagName(tif, tag), i, count);
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



