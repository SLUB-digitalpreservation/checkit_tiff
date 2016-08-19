cit_tiff_baseline_minimal.cfg
	Defines rules for checking files against the Baseline TIFF 6 specification.

cit_tiff6_baseline_SLUB.cfg
	Like "cit_tiff_baseline_minimal.cfg", but with the extensions as defined in
	http://www.slub-dresden.de/ueber-uns/slubarchiv/technische-standards-fuer-die-ablieferung-von-digitalen-dokumenten/handreichung-tiff/

cit_tiff6_baseline_SLUBrelaxed.cfg
	Like "cit_tiff6_baseline_SLUB.cfg", but with following tags allowed:
	33723	83BB	IPTC		IPTC (International Press Telecommunications Council) metadata. 
	34377	8649	Photoshop	Collection of Photoshop 'Image Resource Blocks'.

cit_tiff6_legacy_SLUB.cfg
	Like "cit_tiff6_baseline_SLUB.cfg", but with optionally allowed tag
	315		013B	Artist		Person who created the image. Some older TIFF files used this tag for storing Copyright information.
	..., but only if it contains the exact string "Business Graphics Datentechnik GmbH".

cit_tiff-a_whitepaper.cfg
	Defines rules for checking files against the TI-A's preliminary specification as in
	http://www.ti-a.org/TI-A%20Whitepaper.pdf
