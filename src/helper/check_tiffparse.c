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
#include "check_helper.h"
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


const char * TIFFTagName( tag_t tag ) {
  switch (tag) {
    /*  next lines from tiffdump.c */
    /*  254 - 255, 0xfe - 0xff */
    /* 254 */ case TIFFTAG_SUBFILETYPE: return ("SubFileType" );
    /* 255 */ case TIFFTAG_OSUBFILETYPE: return ("OldSubFileType" );
    /*  256 - 511, 0x100 - 0x1ff */
    /* 256 */ case TIFFTAG_IMAGEWIDTH: return ("ImageWidth" );
    /* 257 */ case TIFFTAG_IMAGELENGTH: return ("ImageLength" );
    /* 258 */ case TIFFTAG_BITSPERSAMPLE: return ("BitsPerSample" );
    /* 259 */ case TIFFTAG_COMPRESSION: return ("Compression" );
    /* 262 */ case TIFFTAG_PHOTOMETRIC: return ("Photometric" );
    /* 263 */ case TIFFTAG_THRESHHOLDING: return ("Threshholding" );
    /* 264 */ case TIFFTAG_CELLWIDTH: return ("CellWidth" );
    /* 265 */ case TIFFTAG_CELLLENGTH: return ("CellLength" );
    /* 266 */ case TIFFTAG_FILLORDER: return ("FillOrder" );
    /* 269 */ case TIFFTAG_DOCUMENTNAME: return ("DocumentName" );
    /* 270 */ case TIFFTAG_IMAGEDESCRIPTION: return ("ImageDescription" );
    /* 271 */ case TIFFTAG_MAKE: return ("Make" );
    /* 272 */ case TIFFTAG_MODEL: return ("Model" );
    /* 273 */ case TIFFTAG_STRIPOFFSETS: return ("StripOffsets" );
    /* 274 */ case TIFFTAG_ORIENTATION: return ("Orientation" );
    /* 277 */ case TIFFTAG_SAMPLESPERPIXEL: return ("SamplesPerPixel" );
    /* 278 */ case TIFFTAG_ROWSPERSTRIP: return ("RowsPerStrip" );
    /* 279 */ case TIFFTAG_STRIPBYTECOUNTS: return ("StripByteCounts" );
    /* 280 */ case TIFFTAG_MINSAMPLEVALUE: return ("MinSampleValue" );
    /* 281 */ case TIFFTAG_MAXSAMPLEVALUE: return ("MaxSampleValue" );
    /* 282 */ case TIFFTAG_XRESOLUTION: return ("XResolution" );
    /* 283 */ case TIFFTAG_YRESOLUTION: return ("YResolution" );
    /* 284 */ case TIFFTAG_PLANARCONFIG: return ("PlanarConfig" );
    /* 285 */ case TIFFTAG_PAGENAME: return ("PageName" );
    /* 286 */ case TIFFTAG_XPOSITION: return ("XPosition" );
    /* 287 */ case TIFFTAG_YPOSITION: return ("YPosition" );
    /* 288 */ case TIFFTAG_FREEOFFSETS: return ("FreeOffsets" );
    /* 289 */ case TIFFTAG_FREEBYTECOUNTS: return ("FreeByteCounts" );
    /* 290 */ case TIFFTAG_GRAYRESPONSEUNIT: return ("GrayResponseUnit" );
    /* 291 */ case TIFFTAG_GRAYRESPONSECURVE: return ("GrayResponseCurve" );
    /* 292 */ case TIFFTAG_GROUP3OPTIONS: return ("Group3Options" );
    /* 293 */ case TIFFTAG_GROUP4OPTIONS: return ("Group4Options" );
    /* 296 */ case TIFFTAG_RESOLUTIONUNIT: return ("ResolutionUnit" );
    /* 297 */ case TIFFTAG_PAGENUMBER: return ("PageNumber" );
    /* 300 */ case TIFFTAG_COLORRESPONSEUNIT: return ("ColorResponseUnit" );
    /* 301 */ case TIFFTAG_TRANSFERFUNCTION: return ("TransferFunction" );
    /* 305 */ case TIFFTAG_SOFTWARE: return ("Software" );
    /* 306 */ case TIFFTAG_DATETIME: return ("DateTime" );
    /* 315 */ case TIFFTAG_ARTIST: return ("Artist" );
    /* 316 */ case TIFFTAG_HOSTCOMPUTER: return ("HostComputer" );
    /* 317 */ case TIFFTAG_PREDICTOR: return ("Predictor" );
    /* 318 */ case TIFFTAG_WHITEPOINT: return ("Whitepoint" );
    /* 319 */ case TIFFTAG_PRIMARYCHROMATICITIES: return ("PrimaryChromaticities" );
    /* 320 */ case TIFFTAG_COLORMAP: return ("Colormap" );
    /* 321 */ case TIFFTAG_HALFTONEHINTS: return ("HalftoneHints" );
    /* 322 */ case TIFFTAG_TILEWIDTH: return ("TileWidth" );
    /* 323 */ case TIFFTAG_TILELENGTH: return ("TileLength" );
    /* 324 */ case TIFFTAG_TILEOFFSETS: return ("TileOffsets" );
    /* 325 */ case TIFFTAG_TILEBYTECOUNTS: return ("TileByteCounts" );
    /* 326 */ case TIFFTAG_BADFAXLINES: return ("BadFaxLines" );
    /* 327 */ case TIFFTAG_CLEANFAXDATA: return ("CleanFaxData" );
    /* 328 */ case TIFFTAG_CONSECUTIVEBADFAXLINES: return ("ConsecutiveBadFaxLines" );
    /* 330 */ case TIFFTAG_SUBIFD: return ("SubIFD" );
    /* 332 */ case TIFFTAG_INKSET: return ("InkSet" );
    /* 333 */ case TIFFTAG_INKNAMES: return ("InkNames" );
    /* 334 */ case TIFFTAG_NUMBEROFINKS: return ("NumberOfInks" );
    /* 336 */ case TIFFTAG_DOTRANGE: return ("DotRange" );
    /* 337 */ case TIFFTAG_TARGETPRINTER: return ("TargetPrinter" );
    /* 338 */ case TIFFTAG_EXTRASAMPLES: return ("ExtraSamples" );
    /* 339 */ case TIFFTAG_SAMPLEFORMAT: return ("SampleFormat" );
    /* 340 */ case TIFFTAG_SMINSAMPLEVALUE: return ("SMinSampleValue" );
    /* 341 */ case TIFFTAG_SMAXSAMPLEVALUE: return ("SMaxSampleValue" );
    /* 512 - 767, 0x200 - 0x2ff */
    /* 512 */ case TIFFTAG_JPEGPROC: return ("JPEGProcessingMode" );
    /* 513 */ case TIFFTAG_JPEGIFOFFSET: return ("JPEGInterchangeFormat" );
    /* 514 */ case TIFFTAG_JPEGIFBYTECOUNT: return ("JPEGInterchangeFormatLength" );
    /* 515 */ case TIFFTAG_JPEGRESTARTINTERVAL: return ("JPEGRestartInterval" );
    /* 517 */ case TIFFTAG_JPEGLOSSLESSPREDICTORS: return ("JPEGLosslessPredictors" );
    /* 518 */ case TIFFTAG_JPEGPOINTTRANSFORM: return ("JPEGPointTransform" );
    /* 347 */ case TIFFTAG_JPEGTABLES: return ("JPEGTables" );
    /* 519 */ case TIFFTAG_JPEGQTABLES: return ("JPEGQTables" );
    /* 520 */ case TIFFTAG_JPEGDCTABLES: return ("JPEGDCTables" );
    /* 521 */ case TIFFTAG_JPEGACTABLES: return ("JPEGACTables" );
    /* 529 */ case TIFFTAG_YCBCRCOEFFICIENTS: return ("YCbCrCoefficients" );
    /* 530 */ case TIFFTAG_YCBCRSUBSAMPLING: return ("YCbCrSubsampling" );
    /* 531 */ case TIFFTAG_YCBCRPOSITIONING: return ("YCbCrPositioning" );
    /* 532 */ case TIFFTAG_REFERENCEBLACKWHITE: return ("ReferenceBlackWhite" );
    /* 700 */ case TIFFTAG_XMLPACKET: return ("XMP");
    /* 32768 -,  0x8000 -  */
    /* 32768 */ case 32768: return ("OLD BOGUS Matteing tag" );
    /* 32953 */ case TIFFTAG_REFPTS: return ("IgReferencePoints (Island Graphics)" );
    /* 32954 */ case TIFFTAG_REGIONTACKPOINT: return ("IgRegionTackPoint (Island Graphics)" );
    /* 32955 */ case TIFFTAG_REGIONWARPCORNERS: return ("IgRegionWarpCorners (Island Graphics)" );
    /* 32956 */ case TIFFTAG_REGIONAFFINE: return ("IgRegionAffine (Island Graphics)" );
    /* 32995 */ case TIFFTAG_MATTEING: return ("OBSOLETE Matteing (Silicon Graphics)" );
    /* 32996 */ case TIFFTAG_DATATYPE: return ("OBSOLETE DataType (Silicon Graphics)" );
    /* 32997 */ case TIFFTAG_IMAGEDEPTH: return ("ImageDepth (Silicon Graphics)" );
    /* 32998 */ case TIFFTAG_TILEDEPTH: return ("TileDepth (Silicon Graphics)" );
    /* 33432 */ case TIFFTAG_COPYRIGHT: return ("Copyright" );
    /* 33723 */ case TIFFTAG_RICHTIFFIPTC: return ("RichTIFFIPTC"); /*  from libtiff */
    /* 34665 */ case TIFFTAG_EXIFIFD: return ("EXIFIFDOffset");
    /* 34675 */ case TIFFTAG_ICCPROFILE: return ("ICC Profile" );
    /* 34750 */ case TIFFTAG_JBIGOPTIONS: return ("JBIG Options" );
    /* 37439 */ case TIFFTAG_STONITS: return ("StoNits" );
                                      /* next 4 lines  WANG TIFF  */
    /* 32932 */ case 32932: return ("TIFF annotation data / Wang TIFF"); /* http://web.archive.org/web/20050309141348/http://www.kofile.com/support%20pro/faqs/annospec.htm, for full explanation, see http://web.archive.org/web/20030124024322/http://www5.eistream.com/support_pro/faqs/annospec.htm */
    /* 32933 */ case 32933: return ("unknown Wang TIFF");
    /* 32934 */ case 32934: return ("Offset to Page Control Structure / Wang TIFF"); /* https://groups.yahoo.com/neo/groups/tiff/conversations/messages/1968 */
    /* 32935 */ case 32935: return ("unknown Wang TIFF");
    /* 32970 */ case 32970: return ("temperature (COMPIX)"); /* http://www.compix.com/fileformattif.htm */

    /* 32971 */ case 32971: return ("Zoombox (COMPIX)"); /* http://www.compix.com/fileformattif.htm */
    /* 32972 */ case 32972: return ("WinTES (COMPIX)"); /* http://www.compix.com/fileformattif.htm */
    /* 33424 */ case 33424: return ("Kodak IFD");  /* http://perldoc.net/Image/ExifTool/TagNames.pod#Unknown%20Tags  */

    /* 33445 */ case 33445: return ("MD_Filetag"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33446 */ case 33446: return ("MD_ScalePixel"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33447 */ case 33447: return ("MD_Colortable"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33448 */ case 33448: return ("MD_LabName"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33449 */ case 33449: return ("MD_SampleInfo"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33450 */ case 33450: return ("MD_PrepDate"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33451 */ case 33451: return ("MD_PrepTime"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33452 */ case 33452: return ("MD_FileUnits"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33918 */ case 33918: return ("INGR Packet Data Tag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 33919 */ case 33919: return ("INGR Flag Registers"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 33920 */ case 33920: return ("IrasB Transormation Matrix"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 33922 */ case 33922: return ("ModelTiepointTag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34264 */ case 34264: return ("ModelTransformationTag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34377 */ case TIFFTAG_PHOTOSHOP: return ("Photoshop Image Ressources"); /*  see http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577413_pgfId-1039502 */
    /* 34732 */ case TIFFTAG_IMAGELAYER: return ("ImageLayer"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag, see RFC2301*/
    /* 34820 */ case 34820: return ("EMC/PixTool SpecificTag"); /* thgere is more informatioon about meaning needed. If tag exists, there was string "Untitled" encoded (as binary) */

    /* 34908 */ case TIFFTAG_FAXRECVPARAMS: return ("HylaFax FaxRecvParams"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34909 */ case TIFFTAG_FAXSUBADDRESS: return ("HylaFax FaxSubAdress"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34910 */ case TIFFTAG_FAXRECVTIME: return ("HylaFax FaxRecvTime"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 37724 */ case 37724: return ("ImageSourceData"); /* http://justsolve.archiveteam.org/wiki/PSD, http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/ */
    /* 42112 */ case 42112: return ("GDAL_Metadata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 42113 */ case 42113: return ("GDAL_nodata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50215 */ case 50215: return ("Oce Scanjob Description"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50216 */ case 50216: return ("Oce Application Selector"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50217 */ case 50217: return ("Oce Identification Number"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50218 */ case 50218: return ("Oce ImageLogic Characteristics"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50255 */ case 50255: return ("Photoshop Annotations"); /* http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577409_17312  */
    /* 50784 */ case 50784: return ("Alias Layer Metadata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50933 */ case 50933: return ("ExtraCameraProfiles"); /* http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_spec_1.4.0.0.pdf */
    /* 59932 */ case 59932: return ("Padding/ Exif MS HDPhoto"); /* see exiftool, http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/EXIF.html, 
                                                                    also check https://commons.apache.org/proper/commons-imaging/xref/org/apache/commons/imaging/formats/tiff/constants/MicrosoftHdPhotoTagConstants.html 
                                                                    it is around 2kB and used for Windows Vista */
    default: return ("undefined tag");
  }
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
      return tif_returns( tag, float2str(value), float2str(val));
    }

  } else {
    // FIXME: tif_fails?
    char array[40];
    snprintf(array, sizeof(array), "tag %u should exist, because defined\n", tag);
    return tif_fails(array);
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
      return tif_returns( tag, int2str(value), int2str(val));
    }

  } else {
    // FIXME: tif_fails?
    char array[40];
    snprintf(array, sizeof(array), "tag %u should exist, because defined\n", tag);
    return tif_fails(array);
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
      return tif_returns( tag, int2str(value), int2str(val));
    }

  } else {
    // FIXME: tif_fails?
    char array[40];
    snprintf(array, sizeof(array), "tag %u should exist, because defined\n", tag);
    return tif_fails(array);
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
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tag));
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
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tag));
          return -1;
  }
}




