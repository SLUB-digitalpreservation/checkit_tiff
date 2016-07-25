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


/* TODO:
 * initialize_ctif()
 * free_ctif()
 *
 * get_ifd0_pos()
 * get_ifd0_count()
 * get_ifd0_stream()
 * ...
 */




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
    /* 347 */ case TIFFTAG_JPEGTABLES: return ("JPEGTables" );
    /* 400 */ case TIFFTAG_GLOBALPARAMETERSIFD: return ("GLOBALPARAMETERSIFD");
    /* 401 */ case TIFFTAG_PROFILETYPE: return ("PROFILETYPE");
    /* 402 */ case TIFFTAG_FAXPROFILE: return ("FAXPROFILE");
    /* 403 */ case TIFFTAG_CODINGMETHODS: return ("CODINGMETHODS");
    /* 404 */ case TIFFTAG_VERSIONYEAR: return ("VERSIONYEAR");
    /* 405 */ case TIFFTAG_MODENUMBER: return ("MODENUMBER");
    /* 433 */ case TIFFTAG_DECODE: return ("DECODE");
    /* 434 */ case TIFFTAG_IMAGEBASECOLOR: return ("IMAGEBASECOLOR");
    /* 435 */ case TIFFTAG_T82OPTIONS: return ("T82OPTIONS");
    /* 512 - 767, 0x200 - 0x2ff */
    /* 512 */ case TIFFTAG_JPEGPROC: return ("JPEGProcessingMode" );
    /* 513 */ case TIFFTAG_JPEGIFOFFSET: return ("JPEGInterchangeFormat" );
    /* 514 */ case TIFFTAG_JPEGIFBYTECOUNT: return ("JPEGInterchangeFormatLength" );
    /* 515 */ case TIFFTAG_JPEGRESTARTINTERVAL: return ("JPEGRestartInterval" );
    /* 517 */ case TIFFTAG_JPEGLOSSLESSPREDICTORS: return ("JPEGLosslessPredictors" );
    /* 518 */ case TIFFTAG_JPEGPOINTTRANSFORM: return ("JPEGPointTransform" );
    /* 519 */ case TIFFTAG_JPEGQTABLES: return ("JPEGQTables" );
    /* 520 */ case TIFFTAG_JPEGDCTABLES: return ("JPEGDCTables" );
    /* 521 */ case TIFFTAG_JPEGACTABLES: return ("JPEGACTables" );
    /* 529 */ case TIFFTAG_YCBCRCOEFFICIENTS: return ("YCbCrCoefficients" );
    /* 530 */ case TIFFTAG_YCBCRSUBSAMPLING: return ("YCbCrSubsampling" );
    /* 531 */ case TIFFTAG_YCBCRPOSITIONING: return ("YCbCrPositioning" );
    /* 532 */ case TIFFTAG_REFERENCEBLACKWHITE: return ("ReferenceBlackWhite" );
    /* 559 */ case TIFFTAG_STRIPROWCOUNTS: return ("StripRowCounts");
    /* 700 */ case TIFFTAG_XMLPACKET: return ("XMP");

    /* 32768 -,  0x8000 -  */
    /* 32768 */ case 32768: return ("OLD BOGUS Matteing tag" );
    /* 32781 */ case TIFFTAG_OPIIMAGEID: return ("OpiImageId");
    /* next 4 lines  WANG TIFF  */
    /* 32932 */ case 32932: return ("TIFF annotation data / Wang TIFF"); /* http://web.archive.org/web/20050309141348/http://www.kofile.com/support%20pro/faqs/annospec.htm, for full explanation, see http://web.archive.org/web/20030124024322/http://www5.eistream.com/support_pro/faqs/annospec.htm */
    /* 32933 */ case 32933: return ("unknown Wang TIFF");
    /* 32934 */ case 32934: return ("Offset to Page Control Structure / Wang TIFF"); /* https://groups.yahoo.com/neo/groups/tiff/conversations/messages/1968 */
    /* 32935 */ case 32935: return ("unknown Wang TIFF");
    /* 32952 */ case 32952: return ("unknown Ig (Island Graphics)" ); /* http://fileformats.archiveteam.org/wiki/Island_Graphics_TIFF */
    /* 32953 */ case TIFFTAG_REFPTS: return ("IgReferencePoints (Island Graphics)" );
    /* 32954 */ case TIFFTAG_REGIONTACKPOINT: return ("IgRegionTackPoint (Island Graphics)" );
    /* 32955 */ case TIFFTAG_REGIONWARPCORNERS: return ("IgRegionWarpCorners (Island Graphics)" );
    /* 32956 */ case TIFFTAG_REGIONAFFINE: return ("IgRegionAffine (Island Graphics)" );
    /* 32970 */ case 32970: return ("temperature (COMPIX)"); /* http://www.compix.com/fileformattif.htm */
    /* 32971 */ case 32971: return ("Zoombox (COMPIX)"); /* http://www.compix.com/fileformattif.htm */
    /* 32972 */ case 32972: return ("WinTES (COMPIX)"); /* http://www.compix.com/fileformattif.htm */
    /* 32995 */ case TIFFTAG_MATTEING: return ("OBSOLETE Matteing (Silicon Graphics)" );
    /* 32996 */ case TIFFTAG_DATATYPE: return ("OBSOLETE DataType (Silicon Graphics)" );
    /* 32997 */ case TIFFTAG_IMAGEDEPTH: return ("ImageDepth (Silicon Graphics)" );
    /* 32998 */ case TIFFTAG_TILEDEPTH: return ("TileDepth (Silicon Graphics)" );
    /* 33300 */ case TIFFTAG_PIXAR_IMAGEFULLWIDTH: return ("PIXAR_IMAGEFULLWIDTH");
    /* 33301 */ case TIFFTAG_PIXAR_IMAGEFULLLENGTH: return ("PIXAR_IMAGEFULLLENGTH");
    /* 33302 */ case TIFFTAG_PIXAR_TEXTUREFORMAT: return ("PIXAR_TEXTUREFORMAT");
    /* 33303 */ case TIFFTAG_PIXAR_WRAPMODES: return ("PIXAR_WRAPMODES");
    /* 33304 */ case TIFFTAG_PIXAR_FOVCOT: return ("PIXAR_FOVCOT");
    /* 33305 */ case TIFFTAG_PIXAR_MATRIX_WORLDTOSCREEN: return ("PIXAR_MATRIX_WORLDTOSCREEN");
    /* 33306 */ case TIFFTAG_PIXAR_MATRIX_WORLDTOCAMERA: return ("PIXAR_MATRIX_WORLDTOCAMERA");
    /* 33405 */ case TIFFTAG_WRITERSERIALNUMBER: return ("Kodak serial number");
    /* 33421 */ case 33421: return ("CFAREPEATPATTERNDIM");
    /* 33422 */ case 33422: return ("CFAPATTERN");
    /* 33424 */ case 33424: return ("Kodak IFD");  /* http://perldoc.net/Image/ExifTool/TagNames.pod#Unknown%20Tags  */
    /* 33432 */ case TIFFTAG_COPYRIGHT: return ("Copyright" );
    /* 33434 */ case 33434: return ("EXIF ExposureTime"); /* EXIF, Exposure time, given in seconds. */
    /* 33437 */ case 33437: return ("EXIF FNumber"); /* EXIF, The F number. */
    /* 33445 */ case 33445: return ("MD_Filetag"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33446 */ case 33446: return ("MD_ScalePixel"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33447 */ case 33447: return ("MD_Colortable"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33448 */ case 33448: return ("MD_LabName"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33449 */ case 33449: return ("MD_SampleInfo"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33450 */ case 33450: return ("MD_PrepDate"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33451 */ case 33451: return ("MD_PrepTime"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33452 */ case 33452: return ("MD_FileUnits"); /* http://research.stowers-institute.org/mcm/efg/ScientificSoftware/Utility/TiffTags/GEL-FileFormat.pdf */
    /* 33628 */ case 33628: return ("MetaMorph Stack Image UIC1"); /* see: ftp://ftp.meta.moleculardevices.com/support/stack/STK.doc */
    /* 33629 */ case 33629: return ("MetaMorph Stack Image UIC2"); /* see: ftp://ftp.meta.moleculardevices.com/support/stack/STK.doc */
    /* 33630 */ case 33630: return ("MetaMorph Stack Image UIC3"); /* see: ftp://ftp.meta.moleculardevices.com/support/stack/STK.doc */
    /* 33631 */ case 33631: return ("MetaMorph Stack Image UIC4"); /* see: ftp://ftp.meta.moleculardevices.com/support/stack/STK.doc */
    /* 33723 */ case TIFFTAG_RICHTIFFIPTC: return ("RichTIFFIPTC"); /*  from libtiff */
    /* 33918 */ case 33918: return ("INGR Packet Data Tag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 33919 */ case 33919: return ("INGR Flag Registers"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 33920 */ case 33920: return ("IrasB Transormation Matrix"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 33922 */ case 33922: return ("ModelTiepointTag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34016 */ case TIFFTAG_IT8SITE: return ("IT8SITE");
    /* 34017 */ case TIFFTAG_IT8COLORSEQUENCE: return ("IT8COLORSEQUENCE");
    /* 34018 */ case TIFFTAG_IT8HEADER: return ("IT8HEADER");
    /* 34019 */ case TIFFTAG_IT8RASTERPADDING: return ("IT8RASTERPADDING");
    /* 34020 */ case TIFFTAG_IT8BITSPERRUNLENGTH: return ("IT8BITSPERRUNLENGTH");
    /* 34021 */ case TIFFTAG_IT8BITSPEREXTENDEDRUNLENGTH: return ("IT8BITSPEREXTENDEDRUNLENGTH");
    /* 34022 */ case TIFFTAG_IT8COLORTABLE: return ("IT8COLORTABLE");
    /* 34023 */ case TIFFTAG_IT8IMAGECOLORINDICATOR: return ("IT8IMAGECOLORINDICATOR");
    /* 34024 */ case TIFFTAG_IT8BKGCOLORINDICATOR: return ("IT8BKGCOLORINDICATOR");
    /* 34025 */ case TIFFTAG_IT8IMAGECOLORVALUE: return ("IT8IMAGECOLORVALUE");
    /* 34026 */ case TIFFTAG_IT8BKGCOLORVALUE: return ("IT8BKGCOLORVALUE");
    /* 34027 */ case TIFFTAG_IT8PIXELINTENSITYRANGE: return ("IT8PIXELINTENSITYRANGE");
    /* 34028 */ case TIFFTAG_IT8TRANSPARENCYINDICATOR: return ("IT8TRANSPARENCYINDICATOR");
    /* 34029 */ case TIFFTAG_IT8COLORCHARACTERIZATION: return ("IT8COLORCHARACTERIZATION");
    /* 34030 */ case TIFFTAG_IT8HCUSAGE: return ("IT8HCUSAGE");
    /* 34031 */ case TIFFTAG_IT8TRAPINDICATOR: return ("IT8TRAPINDICATOR");
    /* 34032 */ case TIFFTAG_IT8CMYKEQUIVALENT: return ("IT8CMYKEQUIVALENT");
    /* 34232 */ case TIFFTAG_FRAMECOUNT: return ("FRAMECOUNT");
    /* 34264 */ case 34264: return ("ModelTransformationTag"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34310 */ case 34310: return ("Leaf MOS PKTS tag (Raw camera)"); /*  see http://fileformats.archiveteam.org/wiki/Leaf_MOS */
    /* 34377 */ case TIFFTAG_PHOTOSHOP: return ("Photoshop Image Ressources"); /*  see http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577413_pgfId-1039502 */
    /* 34665 */ case TIFFTAG_EXIFIFD: return ("EXIFIFDOffset");
    /* 34675 */ case TIFFTAG_ICCPROFILE: return ("ICC Profile" );
    /* 34732 */ case TIFFTAG_IMAGELAYER: return ("ImageLayer"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag, see RFC2301*/
    /* 34750 */ case TIFFTAG_JBIGOPTIONS: return ("JBIG Options" );
    /* 34820 */ case 34820: return ("EMC/PixTool SpecificTag"); /* thgere is more informatioon about meaning needed. If tag exists, there was string "Untitled" encoded (as binary) */
    /* 34850 */ case 34850: return ("EXIF ExposureProgram"); /* EXIF, The class of the program used by the camera to set exposure when the picture is taken. */
    /* 34852 */ case 34852: return ("EXIF SpectralSensitivity"); /* EXIF, Indicates the spectral sensitivity of each channel of the camera used. */
    /* 34853 */ case TIFFTAG_GPSIFD: return ("GPSIFD");
    /* 34855 */ case 34855: return ("EXIF ISOSpeedRatings"); /* EXIF, Indicates the ISO Speed and ISO Latitude of the camera or input device as specified in ISO 12232. */
    /* 34856 */ case 34856: return ("EXIF OECF"); /* EXIF, Indicates the Opto-Electric Conversion Function (OECF) specified in ISO 14524. */
    /* 34908 */ case TIFFTAG_FAXRECVPARAMS: return ("HylaFax FaxRecvParams"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34909 */ case TIFFTAG_FAXSUBADDRESS: return ("HylaFax FaxSubAdress"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 34910 */ case TIFFTAG_FAXRECVTIME: return ("HylaFax FaxRecvTime"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 36864 */ case 36864: return ("EXIF ExifVersion"); /* EXIF, The version of the supported Exif standard. */
    /* 36867 */ case 36867: return ("EXIF DateTimeOriginal"); /* EXIF, The date and time when the original image data was generated. */
    /* 36868 */ case 36868: return ("EXIF DateTimeDigitized"); /* EXIF, The date and time when the image was stored as digital data. */
    /* 37121 */ case 37121: return ("EXIF ComponentsConfiguration"); /* EXIF, Specific to compressed data; specifies the channels and complements PhotometricInterpretation */
    /* 37122 */ case 37122: return ("EXIF CompressedBitsPerPixel"); /* EXIF, Specific to compressed data; states the compressed bits per pixel. */
    /* 37377 */ case 37377: return ("EXIF ShutterSpeedValue"); /* EXIF, Shutter speed. */
    /* 37378 */ case 37378: return ("EXIF ApertureValue"); /* EXIF, The lens aperture. */
    /* 37379 */ case 37379: return ("EXIF BrightnessValue"); /* EXIF, The value of brightness. */
    /* 37380 */ case 37380: return ("EXIF ExposureBiasValue"); /* EXIF, The exposure bias. */
    /* 37381 */ case 37381: return ("EXIF MaxApertureValue"); /* EXIF, The smallest F number of the lens. */
    /* 37382 */ case 37382: return ("EXIF SubjectDistance"); /* EXIF, The distance to the subject, given in meters. */
    /* 37383 */ case 37383: return ("EXIF MeteringMode"); /* EXIF, The metering mode. */
    /* 37384 */ case 37384: return ("EXIF LightSource"); /* EXIF, The kind of light source. */
    /* 37385 */ case 37385: return ("EXIF Flash"); /* EXIF, Indicates the status of flash when the image was shot. */
    /* 37386 */ case 37386: return ("EXIF FocalLength"); /* EXIF, The actual focal length of the lens, in mm. */
    /* 37396 */ case 37396: return ("EXIF SubjectArea"); /* EXIF, Indicates the location and area of the main subject in the overall scene. */
    /* 37398 */ case 37398: return ("TIFF/EPStandardID"); /* TIFF/EP, see http://fileformats.archiveteam.org/wiki/TIFF/EP */
    /* 37400 */ case 37400: return ("old Kodak KDC"); /*  see <http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */
    /* 37439 */ case TIFFTAG_STONITS: return ("StoNits" );
    /* 37500 */ case 37500: return ("EXIF MakerNote"); /* EXIF, Manufacturer specific information. */
    /* 37510 */ case 37510: return ("EXIF UserComment"); /* EXIF, Keywords or comments on the image; complements ImageDescription. */
    /* 37520 */ case 37520: return ("EXIF SubsecTime"); /* EXIF, A tag used to record fractions of seconds for the DateTime tag. */
    /* 37521 */ case 37521: return ("EXIF SubsecTimeOriginal"); /* EXIF, A tag used to record fractions of seconds for the DateTimeOriginal tag. */
    /* 37522 */ case 37522: return ("EXIF SubsecTimeDigitized"); /* EXIF, A tag used to record fractions of seconds for the DateTimeDigitized tag. */
    /* 37724 */ case 37724: return ("ImageSourceData"); /* http://justsolve.archiveteam.org/wiki/PSD, http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/ */

    /* 40960 */ case 40960: return ("EXIF FlashpixVersion"); /* EXIF, The Flashpix format version supported by a FPXR file. */
    /* 40961 */ case 40961: return ("EXIF ColorSpace"); /* EXIF, The color space information tag is always recorded as the color space specifier. */
    /* 40962 */ case 40962: return ("EXIF PixelXDimension"); /* EXIF, Specific to compressed data; the valid width of the meaningful image. */
    /* 40963 */ case 40963: return ("EXIF PixelYDimension"); /* EXIF, Specific to compressed data; the valid height of the meaningful image. */
    /* 40964 */ case 40964: return ("EXIF RelatedSoundFile"); /* EXIF, Used to record the name of an audio file related to the image data. */
    /* 40965 */ case TIFFTAG_INTEROPERABILITYIFD: return ("INTEROPERABILITYIFD");
    /* 41483 */ case 41483: return ("EXIF FlashEnergy"); /* EXIF, Indicates the strobe energy at the time the image is captured, as measured in Beam Candle Power Seconds */
    /* 41484 */ case 41484: return ("EXIF SpatialFrequencyResponse"); /* EXIF, Records the camera or input device spatial frequency table and SFR values in the direction of image width, image height, and diagonal direction, as specified in ISO 12233. */
    /* 41486 */ case 41486: return ("EXIF FocalPlaneXResolution"); /* EXIF, Indicates the number of pixels in the image width (X) direction per FocalPlaneResolutionUnit on the camera focal plane. */
    /* 41487 */ case 41487: return ("EXIF FocalPlaneYResolution"); /* EXIF, Indicates the number of pixels in the image height (Y) direction per FocalPlaneResolutionUnit on the camera focal plane. */
    /* 41488 */ case 41488: return ("EXIF FocalPlaneResolutionUnit"); /* EXIF, Indicates the unit for measuring FocalPlaneXResolution and FocalPlaneYResolution. */
    /* 41492 */ case 41492: return ("EXIF SubjectLocation"); /* EXIF, Indicates the location of the main subject in the scene. */
    /* 41493 */ case 41493: return ("EXIF ExposureIndex"); /* EXIF, Indicates the exposure index selected on the camera or input device at the time the image is captured. */
    /* 41495 */ case 41495: return ("EXIF SensingMethod"); /* EXIF, Indicates the image sensor type on the camera or input device. */
    /* 41728 */ case 41728: return ("EXIF FileSource"); /* EXIF, Indicates the image source. */
    /* 41729 */ case 41729: return ("EXIF SceneType"); /* EXIF, Indicates the type of scene. */
    /* 41730 */ case 41730: return ("EXIF CFAPattern"); /* EXIF, Indicates the color filter array (CFA) geometric pattern of the image sensor when a one-chip color area sensor is used. */
    /* 41985 */ case 41985: return ("EXIF CustomRendered"); /* EXIF, Indicates the use of special processing on image data, such as rendering geared to output. */
    /* 41986 */ case 41986: return ("EXIF ExposureMode"); /* EXIF, Indicates the exposure mode set when the image was shot. */
    /* 41987 */ case 41987: return ("EXIF WhiteBalance"); /* EXIF, Indicates the white balance mode set when the image was shot. */
    /* 41988 */ case 41988: return ("EXIF DigitalZoomRatio"); /* EXIF, Indicates the digital zoom ratio when the image was shot. */
    /* 41989 */ case 41989: return ("EXIF FocalLengthIn35mmFilm"); /* EXIF, Indicates the equivalent focal length assuming a 35mm film camera, in mm. */
    /* 41990 */ case 41990: return ("EXIF SceneCaptureType"); /* EXIF, Indicates the type of scene that was shot. */
    /* 41991 */ case 41991: return ("EXIF GainControl"); /* EXIF, Indicates the degree of overall image gain adjustment. */
    /* 41992 */ case 41992: return ("EXIF Contrast"); /* EXIF, Indicates the direction of contrast processing applied by the camera when the image was shot. */
    /* 41993 */ case 41993: return ("EXIF Saturation"); /* EXIF, Indicates the direction of saturation processing applied by the camera when the image was shot. */
    /* 41994 */ case 41994: return ("EXIF Sharpness"); /* EXIF, Indicates the direction of sharpness processing applied by the camera when the image was shot. */
    /* 41995 */ case 41995: return ("EXIF DeviceSettingDescription"); /* EXIF, This tag indicates information on the picture-taking conditions of a particular camera model. */
    /* 41996 */ case 41996: return ("EXIF SubjectDistanceRange"); /* EXIF, Indicates the distance to the subject. */
    /* 42016 */ case 42016: return ("EXIF ImageUniqueID"); /* EXIF, Indicates an identifier assigned uniquely to each image. */
    /* 42112 */ case 42112: return ("GDAL Metadata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 42113 */ case 42113: return ("GDAL nodata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 46275 */ case 46275: return ("Imacon 3F"); /*  see http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */
    /* 46279 */ case 46279: return ("Imacon 3F raw dim"); /*  see http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */

    /* 50215 */ case 50215: return ("Oce Scanjob Description"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50216 */ case 50216: return ("Oce Application Selector"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50217 */ case 50217: return ("Oce Identification Number"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50218 */ case 50218: return ("Oce ImageLogic Characteristics"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50255 */ case 50255: return ("Photoshop Annotations"); /* http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/#50577409_17312  */
    /* 50454 */ case 50454: return ("unknown Sinar tag"); /* http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */
    /* 50455 */ case 50455: return ("unknown Sinar tag"); /* http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */
    /* 50459 */ case 50459: return ("Hasselblad IFD"); /* http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */
    /* 50706 */ case TIFFTAG_DNGVERSION: return ("DNGVERSION");
    /* 50707 */ case TIFFTAG_DNGBACKWARDVERSION: return ("DNGBACKWARDVERSION");
    /* 50708 */ case TIFFTAG_UNIQUECAMERAMODEL: return ("UNIQUECAMERAMODEL");
    /* 50709 */ case TIFFTAG_LOCALIZEDCAMERAMODEL: return ("LOCALIZEDCAMERAMODEL");
    /* 50710 */ case TIFFTAG_CFAPLANECOLOR: return ("CFAPLANECOLOR");
    /* 50711 */ case TIFFTAG_CFALAYOUT: return ("CFALAYOUT");
    /* 50712 */ case TIFFTAG_LINEARIZATIONTABLE: return ("LINEARIZATIONTABLE");
    /* 50713 */ case TIFFTAG_BLACKLEVELREPEATDIM: return ("BLACKLEVELREPEATDIM");
    /* 50714 */ case TIFFTAG_BLACKLEVEL: return ("BLACKLEVEL");
    /* 50715 */ case TIFFTAG_BLACKLEVELDELTAH: return ("BLACKLEVELDELTAH");
    /* 50716 */ case TIFFTAG_BLACKLEVELDELTAV: return ("BLACKLEVELDELTAV");
    /* 50717 */ case TIFFTAG_WHITELEVEL: return ("WHITELEVEL");
    /* 50718 */ case TIFFTAG_DEFAULTSCALE: return ("DEFAULTSCALE");
    /* 50719 */ case TIFFTAG_DEFAULTCROPORIGIN: return ("DEFAULTCROPORIGIN");
    /* 50720 */ case TIFFTAG_DEFAULTCROPSIZE: return ("DEFAULTCROPSIZE");
    /* 50721 */ case TIFFTAG_COLORMATRIX1: return ("COLORMATRIX1");
    /* 50722 */ case TIFFTAG_COLORMATRIX2: return ("COLORMATRIX2");
    /* 50723 */ case TIFFTAG_CAMERACALIBRATION1: return ("CAMERACALIBRATION1");
    /* 50724 */ case TIFFTAG_CAMERACALIBRATION2: return ("CAMERACALIBRATION2");
    /* 50725 */ case TIFFTAG_REDUCTIONMATRIX1: return ("REDUCTIONMATRIX1");
    /* 50726 */ case TIFFTAG_REDUCTIONMATRIX2: return ("REDUCTIONMATRIX2");
    /* 50727 */ case TIFFTAG_ANALOGBALANCE: return ("ANALOGBALANCE");
    /* 50728 */ case TIFFTAG_ASSHOTNEUTRAL: return ("ASSHOTNEUTRAL");
    /* 50729 */ case TIFFTAG_ASSHOTWHITEXY: return ("ASSHOTWHITEXY");
    /* 50730 */ case TIFFTAG_BASELINEEXPOSURE: return ("BASELINEEXPOSURE");
    /* 50731 */ case TIFFTAG_BASELINENOISE: return ("BASELINENOISE");
    /* 50732 */ case TIFFTAG_BASELINESHARPNESS: return ("BASELINESHARPNESS");
    /* 50733 */ case TIFFTAG_BAYERGREENSPLIT: return ("BAYERGREENSPLIT");
    /* 50734 */ case TIFFTAG_LINEARRESPONSELIMIT: return ("LINEARRESPONSELIMIT");
    /* 50735 */ case TIFFTAG_CAMERASERIALNUMBER: return ("CAMERASERIALNUMBER");
    /* 50736 */ case TIFFTAG_LENSINFO: return ("LENSINFO");
    /* 50737 */ case TIFFTAG_CHROMABLURRADIUS: return ("CHROMABLURRADIUS");
    /* 50738 */ case TIFFTAG_ANTIALIASSTRENGTH: return ("ANTIALIASSTRENGTH");
    /* 50739 */ case TIFFTAG_SHADOWSCALE: return ("SHADOWSCALE");
    /* 50740 */ case TIFFTAG_DNGPRIVATEDATA: return ("DNGPRIVATEDATA");
    /* 50741 */ case TIFFTAG_MAKERNOTESAFETY: return ("MAKERNOTESAFETY");
    /* 50779 */ case TIFFTAG_CALIBRATIONILLUMINANT2: return ("CALIBRATIONILLUMINANT2");
    /* 50780 */ case TIFFTAG_BESTQUALITYSCALE: return ("BESTQUALITYSCALE");
    /* 50781 */ case TIFFTAG_RAWDATAUNIQUEID: return ("RAWDATAUNIQUEID");
    /* 50784 */ case 50784: return ("Alias Layer Metadata"); /* see http://www.rastermaster.com/RasterMaster%20DLL%20manual/WebHelp/Content/aptifftagswide.htm for explanation of tag*/
    /* 50827 */ case TIFFTAG_ORIGINALRAWFILENAME: return ("ORIGINALRAWFILENAME");
    /* 50828 */ case TIFFTAG_ORIGINALRAWFILEDATA: return ("ORIGINALRAWFILEDATA");
    /* 50829 */ case TIFFTAG_ACTIVEAREA: return ("ACTIVEAREA");
    /* 50830 */ case TIFFTAG_MASKEDAREAS: return ("MASKEDAREAS");
    /* 50831 */ case TIFFTAG_ASSHOTICCPROFILE: return ("ASSHOTICCPROFILE");
    /* 50832 */ case TIFFTAG_ASSHOTPREPROFILEMATRIX: return ("ASSHOTPREPROFILEMATRIX");
    /* 50833 */ case TIFFTAG_CURRENTICCPROFILE: return ("CURRENTICCPROFILE");
    /* 50834 */ case TIFFTAG_CURRENTPREPROFILEMATRIX: return ("CURRENTPREPROFILEMATRIX");
    /* 50933 */ case 50933: return ("ExtraCameraProfiles"); /* http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/products/photoshop/pdfs/dng_spec_1.4.0.0.pdf */
    /* 59932 */ case 59932: return ("Padding/ Exif MS HDPhoto"); /* see exiftool, http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/EXIF.html,
                                                                    also check https://commons.apache.org/proper/commons-imaging/xref/org/apache/commons/imaging/formats/tiff/constants/MicrosoftHdPhotoTagConstants.html
                                                                    it is around 2kB and used for Windows Vista */
    /* 64772 */ case 64772: return ("Kodak P-Series"); /* see http://hg.corpus-callosum.com/X3F_Utilities/X3F_qlgenerator/rev/177b9a6b3b14?revcount=30 */

    default: return ("undefined tag");
  }
}
//------------------------------------------------------------------------------
ret_t check_tag_has_fvalue(ctiff_t * ctif, tag_t tag, float value)
{
  float val;
  int found=TIFFGetField(ctif->tif, tag, &val);
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
    char array[EXPECTSTRLEN];
    snprintf(array, sizeof(array), "tag %u should exist, because defined\n", tag);
    return tif_fails(array);
  }
}

//------------------------------------------------------------------------------
ret_t check_tag_has_u16value(ctiff_t * ctif, tag_t tag, uint16 value)
{
  uint16 val;
  int found=TIFFGetField(ctif->tif, tag, &val);
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
    char array[EXPECTSTRLEN];
    snprintf(array, sizeof(array), "tag %u should exist, because defined\n", tag);
    return tif_fails(array);
  }
}


//------------------------------------------------------------------------------
ret_t check_tag_has_u32value(ctiff_t * ctif, tag_t tag, uint32 value)
{
  uint32 val;
  int found=TIFFGetField(ctif->tif, tag, &val);
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
    char array[EXPECTSTRLEN];
    snprintf(array, sizeof(array), "tag %u should exist, because defined\n", tag);
    return tif_fails(array);
  }
}


int parse_header_and_endianess(ctiff_t * ctif) {
  thandle_t client = TIFFClientdata(ctif->tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
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
  ctif->isbyteswapped = ret;
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

uint32 get_first_IFD(ctiff_t * ctif) {
	int isByteSwapped = parse_header_and_endianess(ctif);
	assert( NULL == ctif->ifd0p);
	/* seek the image file directory (bytes 4-7) */
	thandle_t client = TIFFClientdata(ctif->tif);
	TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
	TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
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
	ctif->ifd0pos=offset;
	ctif->ifd0p=malloc( sizeof(char) * offset );
	if (NULL == ctif->ifd0p) {
		fprintf (stderr, "could not allocate memory for ifd0p\n");
		exit( EXIT_FAILURE );
	}
	seekproc(client, offset, SEEK_SET);

	uint16 count;
	if ( readproc( client, &count, 2) != 2 ) {
		perror ("TIFF Header read error2");
		exit( EXIT_FAILURE );
	}

	if (is_byteswapped(ctif))
		TIFFSwabShort(&count);
	ctif->ifd0c = count;
	assert(0 != ctif->ifd0c);

	return offset;
}

/* scans first IDF and returns count of tags
 * Hint: sideeffect, if succeed the seek points to beginning of the first
 * IFD-entry */
int TIFFGetRawTagListCount (ctiff_t * ctif, uint32 ifdpos) {
	/* parse TIF */
	thandle_t client = TIFFClientdata(ctif->tif);
	TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
	TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
	if (! seekproc) {
		perror ("could not get TIFFGetSeekProc");
	}
	if (! readproc) {
		perror ("could not get TIFFGetReadProc");
	}

	//int fd = TIFFFileno( tif);
	/* seek the image file directory (bytes 4-7) */
	uint32 offset = ifdpos;

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

	if (is_byteswapped(ctif))
		TIFFSwabShort(&count);
	return count;
}

/* scans first IDF and returns the n-th tag */
tag_t TIFFGetRawTagListEntry( ctiff_t * ctif, int tagidx ) {
	int byteswapped = is_byteswapped(ctif);
	int count = get_ifd0_count( ctif);
	thandle_t client = TIFFClientdata(ctif->tif);
	TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
	TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
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
	seekproc(client, ctif->ifd0pos+2, SEEK_SET); /* IFD0 plus 2byte to get IFD-entries */
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
		e++;
		if (i == tagidx) {
			uint16 tagid = (hi << 8) + lo;
			if (byteswapped)
				TIFFSwabShort(&tagid);
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
offset_t read_offsetdata( ctiff_t * ctif, uint32 address, uint16 count, uint16 datatype) {
  //int fd = TIFFFileno( tif);
  thandle_t client = TIFFClientdata(ctif->tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
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
ifd_entry_t TIFFGetRawTagIFDListEntry( ctiff_t * ctif, int tagidx ) {
  int tagcount = get_ifd0_count( ctif);
  assert( tagcount > 0);
  int byteswapped = is_byteswapped(ctif);
#ifdef DEBUG
  printf(" count of tags = %i\n", tagcount);
#endif
  // int fd = TIFFFileno( tif);
  thandle_t client = TIFFClientdata(ctif->tif);
  TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
  TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
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
  ifdentries = malloc ( sizeof(uint8) * 12 * tagcount);
  /*
  if (read(fd, ifdentries, 12 * count) != 12*count) {
    perror ("TIFF Header read error4");
    exit(EXIT_FAILURE);
  }
  */
  seekproc(client, ctif->ifd0pos+2, SEEK_SET); /* IFD0 plus 2byte to get IFD-entries */

  if ( readproc( client, ifdentries, 12 * tagcount) != 12*tagcount ) {
	  perror ("TIFF Header read error4");
	  exit( EXIT_FAILURE );
  }
  uint8 * e = ifdentries;
  for (i = 0; i<tagcount; i++) {
    uint8 lo = *e;
    e++;
    uint8 hi = *e;
    uint16 tagid = (hi << 8) + lo;
    e++;
    if (byteswapped)
      TIFFSwabShort(&tagid);
    if (i == tagidx) {
      // tag type check
      lo = *e; e++;
      hi = *e; e++;
      uint16 tagtype = (hi << 8) + lo;
      if (byteswapped)
        TIFFSwabShort(&tagtype);

      uint32 count = (*(e++));
      count += (*(e++) << 8);
      count += (*(e++) << 16);
      count += (*(e++) << 24);
      if (byteswapped)
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
			if (byteswapped)
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
            if (byteswapped) {
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
ifd_entry_t TIFFGetRawIFDEntry( ctiff_t * ctif, tag_t tag) {
  int tagidx = -1;
  for (int i= 0; i < get_ifd0_count( ctif ); i++) {
    if (tag > 253 && tag == TIFFGetRawTagListEntry( ctif, i )) {
      tagidx= i;
      break;
    };
  };
  ifd_entry_t ifd_entry;
  if (tagidx >= 0) {
    ifd_entry =  TIFFGetRawTagIFDListEntry( ctif, tagidx );
  } else { /* tag not defined */
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tag));
	  ifd_entry.value_or_offset = is_error;
	  ifd_entry.count = 0;
  }
  return ifd_entry;
}
/* scans first IDF and returns the type of the n-th tag */
uint32 TIFFGetRawTagTypeListEntry( ctiff_t * ctif, int tagidx ) {
if (tagidx >= 0) {
    ifd_entry_t ifd_entry;
    ifd_entry =  TIFFGetRawTagIFDListEntry( ctif, tagidx );
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
TIFFDataType TIFFGetRawTagType(ctiff_t * ctif, tag_t tag) {
  int tagidx = -1;
  for (int i= 0; i < get_ifd0_count( ctif ); i++) {
    if (tag > 253 && tag == TIFFGetRawTagListEntry( ctif, i )) {
      tagidx= i;
      break;
    };
  };
  if (tagidx >= 0) {
    TIFFDataType datatype =  TIFFGetRawTagTypeListEntry( ctif, tagidx );
#ifdef DEBUG
    printf("### datatype=%i \n", datatype);
#endif
    return datatype;
  } else { /* tag not defined */
	  printf("\ttag %u (%s) was not found, but requested because defined\n", tag, TIFFTagName(tag));
	  return -1;
  }
}

ctiff_t * initialize_ctif(const char * tiff_file) {
	/* load tiff file */
	TIFF* tif = NULL;
	tif = TIFFOpen( tiff_file, "rm");
	if (NULL == tif) {
		fprintf( stderr, "file '%s' could not be opened\n", tiff_file);
		exit (EXIT_FAILURE);
	};
	ctiff_t * ctif = malloc ( sizeof( ctiff_t) );
	if (NULL == ctif) {
		fprintf( stderr, "could not allocate memory for ctiff_t\n");
		exit (EXIT_FAILURE);
	}
	ctif->tif = tif;
	ctif->filename = strdup(tiff_file);
	ctif->ifd0p=NULL;
	ctif->ifd0pos= 0;
	ctif->ifd0c= 0;
	parse_header_and_endianess( ctif );
	get_first_IFD( ctif );
	return ctif;
}

void free_ctif( ctiff_t * ctif) {
	assert( NULL != ctif);
	TIFFClose(ctif->tif);
	free (ctif);
	ctif = NULL;
}

uint32 get_ifd0_pos( ctiff_t * ctif ) {
	assert( NULL != ctif);
	assert( NULL != ctif->ifd0p);
	return ctif->ifd0pos;
}

uint16 get_ifd0_count( ctiff_t * ctif ) {
	assert( NULL != ctif);
	assert( NULL != ctif->ifd0p);
	return ctif->ifd0c;
}

char is_byteswapped( ctiff_t * ctif ) {
	assert( NULL != ctif);
	return ctif->isbyteswapped;
}

uint32 get_next_ifd_pos(ctiff_t * ctif, uint32 actual_pos) {
	assert( NULL != ctif);
	assert( NULL != ctif->ifd0p);
	thandle_t client = TIFFClientdata(ctif->tif);
	TIFFReadWriteProc readproc = TIFFGetReadProc(ctif->tif);
	TIFFSeekProc seekproc = TIFFGetSeekProc(ctif->tif);
	if (! seekproc) {
		perror ("could not get TIFFGetSeekProc");
	}
	if (! readproc) {
		perror ("could not get TIFFGetReadProc");
	}

	seekproc(client, actual_pos, SEEK_SET);
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

	if (ctif->isbyteswapped)
		TIFFSwabShort(&count);
	seekproc(client, 12 * count, SEEK_CUR);
	uint32 offset;
	if ( readproc( client, &offset, 4) != 4 ) {
		perror ("TIFF Header read error3");
		exit( EXIT_FAILURE );
	}
	if (ctif->isbyteswapped)
		TIFFSwabLong(&offset);
	return offset;
}




	

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/


