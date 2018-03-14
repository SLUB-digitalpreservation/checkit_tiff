#!/usr/bin/perl -w
use strict;
use warnings;
use File::Path;
use File::Slurp;
use Testcall;
use Test::More tests => 181;

my $testdir=prepare();


ok( call_checkit_check_config('') , "empty grammar");
#ok(!call_checkit_check_config('#') , "empty comment");
ok( call_checkit_check_config("#mode( test )\n") , "single comment (1)");
ok( call_checkit_check_config("# mode( test )\n") , "single comment (2)");
ok( call_checkit_check_config("\n") , "empty grammar with newline only");
ok( call_checkit_check_config("#\n") , "empty comment with newline");
ok( call_checkit_check_config("# mode( test )\n# modal( test )\n") , "double comment");

ok( call_checkit_check_config("mode(baseline)\n") , "mode(baseline)");
#ok(!call_checkit_check_config("mode(extended)\n") , "mode(extended)"); # mode "extended" is not implemented yet
#ok(!call_checkit_check_config("mode(nonexist)\n") , "mode(nonexist)");
ok( call_checkit_check_config("mode(enable_type_checks)\n") , "mode(enable_type_checks)");
ok( call_checkit_check_config("mode(enable_offset_checks)\n") , "mode(enable_offset_checks)");
ok( call_checkit_check_config("mode(enable_ifd_checks)\n") , "mode(enable_ifd_checks)");

ok( call_checkit_check_config("256; mandatory; any\n") , "simple tag mandatory any");
ok(!call_checkit_check_config("0; mandatory; any\n") , "simple non-tag mandatory any");
ok( call_checkit_check_config("65535; mandatory; any\n") , "simple 65535 mandatory any");
ok( call_checkit_check_config("2^8; mandatory; any\n") , "simple 2^8 mandatory any");
ok( call_checkit_check_config("256; mandatory; any\n") , "simple tag mandatory any");
ok( call_checkit_check_config("256; mandatory; only(1)\n") , "simple tag mandatory only(1)");
ok( call_checkit_check_config("256; mandatory; ntupel(1,1,1)\n") , "simple tag mandatory ntupel(1,1,1)");

# for supported regexes, see http://www.pcre.org/current/doc/html/pcre2syntax.html
ok( call_checkit_check_config("256; mandatory; regex(\"string\")\n") , "simple tag mandatory regex(\"string\")");
ok( call_checkit_check_config("256; mandatory; regex(\"^string\")\n") , "simple tag mandatory regex(\"^string\")");
ok( call_checkit_check_config("256; mandatory; regex(\"string\$\")\n") , "simple tag mandatory regex(\"string\$\")");
ok( call_checkit_check_config("256; mandatory; regex(\"^string\$\")\n") , "simple tag mandatory regex(\"^string\$\")");
ok( call_checkit_check_config("256; mandatory; regex(\"s*\")\n") , "simple tag mandatory regex(\"s*\")");
ok( call_checkit_check_config("256; mandatory; regex(\"s?\")\n") , "simple tag mandatory regex(\"s?\")");
ok( call_checkit_check_config("256; mandatory; regex(\"s+\")\n") , "simple tag mandatory regex(\"s+\")");
ok( call_checkit_check_config("256; mandatory; regex(\"[A-Z]\")\n") , "simple tag mandatory regex(\"[A-Z]\")");
ok( call_checkit_check_config("256; mandatory; regex(\"[^A-Z]\")\n") , "simple tag mandatory regex(\"[^A-Z]\")");
ok( call_checkit_check_config("256; mandatory; range(1,100)\n") , "simple tag mandatory range(1,100)");
ok( call_checkit_check_config("256; mandatory; range(100,1)\n") , "simple tag mandatory range(100,1)");
ok( call_checkit_check_config("256; mandatory; logical_or(100,1)\n") , "simple tag mandatory logical_or(100,1)");
ok( call_checkit_check_config("256; mandatory; logical_or(1,100)\n") , "simple tag mandatory logical_or(1,100)");
ok( call_checkit_check_config("256; mandatory; logical_or(only(100),only(1))\n") , "simple tag mandatory logical_or(only(100),only(1))");
ok( call_checkit_check_config("256; mandatory; logical_or(only(1),only(100))\n") , "simple tag mandatory logical_or(only(1),only(100))");
ok( call_checkit_check_config("256; mandatory; logical_or(ntupel(1,1,1),1)\n") , "simple tag mandatory logical_or(ntupel(1,1,1),1)");
ok( call_checkit_check_config("256; mandatory; logical_or(ntupel(1,1,1),1,regex(\"string\"))\n") , "simple tag mandatory logical_or(ntupel(1,1,1),1,regex(\"string\"))");
ok( call_checkit_check_config("256; mandatory; logical_or(ntupel(8,8,8),ntupel(16,16,16))\n") , "simple tag mandatory logical_or(ntupel(8,8,8),ntupel(16,16,16))");
ok( call_checkit_check_config("315; mandatory; printascii\n") , "simple tag mandatory printascii");
ok( call_checkit_check_config("306; mandatory; datetime\n") , "simple tag mandatory datetime");
ok( call_checkit_check_config("34675; mandatory; iccprofile\n") , "simple tag mandatory iccprofile");

#
ok( call_checkit_check_config("256; optional; any\n") , "simple tag optional any");
ok(!call_checkit_check_config("0; optional; any\n") , "simple non-tag optional any");
ok( call_checkit_check_config("256; optional; any\n") , "simple tag optional any");
ok( call_checkit_check_config("256; optional; only(1)\n") , "simple tag optional only(1)");
ok( call_checkit_check_config("256; optional; ntupel(1,1,1)\n") , "simple tag optional ntupel(1,1,1)");
ok( call_checkit_check_config("256; optional; regex(\"string\")\n") , "simple tag optional regex(\"string\")");
ok( call_checkit_check_config("256; optional; range(1,100)\n") , "simple tag optional range(1,100)");
ok( call_checkit_check_config("256; optional; range(100,1)\n") , "simple tag optional range(100,1)");
ok( call_checkit_check_config("256; optional; logical_or(100,1)\n") , "simple tag optional logical_or(100,1)");
ok( call_checkit_check_config("256; optional; logical_or(1,100)\n") , "simple tag optional logical_or(1,100)");
ok( call_checkit_check_config("256; optional; logical_or(only(100),only(1))\n") , "simple tag optional logical_or(only(100,1))");
ok( call_checkit_check_config("256; optional; logical_or(only(1),only(100))\n") , "simple tag optional logical_or(only(1,100))");
ok( call_checkit_check_config("315; optional; printascii\n") , "simple tag optional printascii");
ok( call_checkit_check_config("306; optional; datetime\n") , "simple tag optional datetime");
ok( call_checkit_check_config("34675; optional; iccprofile\n") , "simple tag optional iccprofile");
#
ok( call_checkit_check_config("256; depends(255.1); any\n") , "simple tag depends(255.1) any");
ok(!call_checkit_check_config("0; depends(255.1); any\n") , "simple non-tag depends(255.1) any");
ok( call_checkit_check_config("256; depends(255.1); any\n") , "simple tag depends(255.1) any");
ok( call_checkit_check_config("256; depends(255.1); only(1)\n") , "simple tag depends(255.1) only(1)");
ok( call_checkit_check_config("256; depends(255.1); ntupel(1,1,1)\n") , "simple tag depends(255.1) ntupel(1,1,1)");
ok( call_checkit_check_config("256; depends(255.1); regex(\"string\")\n") , "simple tag depends(255.1) regex(\"string\")");
ok( call_checkit_check_config("256; depends(255.1); range(1,100)\n") , "simple tag depends(255.1) range(1,100)");
ok( call_checkit_check_config("256; depends(255.1); range(100,1)\n") , "simple tag depends(255.1) range(100,1)");
ok( call_checkit_check_config("256; depends(255.1); logical_or(100,1)\n") , "simple tag depends(255.1) logical_or(100,1)");
ok( call_checkit_check_config("256; depends(255.1); logical_or(1,100)\n") , "simple tag depends(255.1) logical_or(1,100)");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); any\n") , "simple tag depends(255.regex(\"test\")) any");
#ok(!call_checkit_check_config("256; depends(255.printascii); any\n") , "simple tag depends(255.printascii) any, should fail");
#ok(!call_checkit_check_config("256; depends(255.datetime); any\n") , "simple tag depends(255.datetime) any, should fail");
#ok(!call_checkit_check_config("256; depends(255.iccprofile); any\n") , "simple tag depends(255.iccprofile) any, should fail");


ok(!call_checkit_check_config("0; depends(255.regex(\"test\")); any\n") , "simple non-tag depends(255.regex(\"test\")) any");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); any\n") , "simple tag depends(255.regex(\"test\")) any");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); only(1)\n") , "simple tag depends(255.regex(\"test\")) only(1)");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); ntupel(1,1,1)\n") , "simple tag depends(255.regex(\"test\")) ntupel(1,1,1)");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); regex(\"string\")\n") , "simple tag depends(255.regex(\"test\")) regex(\"string\")");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); range(1,100)\n") , "simple tag depends(255.regex(\"test\")) range(1,100)");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); range(100,1)\n") , "simple tag depends(255.regex(\"test\")) range(100,1)");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); logical_or(100,1)\n") , "simple tag depends(255.regex(\"test\")) logical_or(100,1)");
ok( call_checkit_check_config("256; depends(255.regex(\"test\")); logical_or(1,100)\n") , "simple tag depends(255.regex(\"test\")) logical_or(1,100)");
ok( call_checkit_check_config("256; depends(255.any); any\n") , "simple tag depends(255.any) any");
ok(!call_checkit_check_config("0; depends(255.any); any\n") , "simple non-tag depends(255.any) any");
ok( call_checkit_check_config("256; depends(255.any); any\n") , "simple tag depends(255.any) any");
ok( call_checkit_check_config("256; depends(255.any); only(1)\n") , "simple tag depends(255.any) only(1)");
ok( call_checkit_check_config("256; depends(255.any); ntupel(1,1,1)\n") , "simple tag depends(255.any) ntupel(1,1,1)");
ok( call_checkit_check_config("256; depends(255.any); regex(\"string\")\n") , "simple tag depends(255.any) regex(\"string\")");
ok( call_checkit_check_config("256; depends(255.any); range(1,100)\n") , "simple tag depends(255.any) range(1,100)");
ok( call_checkit_check_config("256; depends(255.any); range(100,1)\n") , "simple tag depends(255.any) range(100,1)");
ok( call_checkit_check_config("256; depends(255.any); logical_or(100,1)\n") , "simple tag depends(255.any) logical_or(100,1)");
ok( call_checkit_check_config("256; depends(255.any); logical_or(1,100)\n") , "simple tag depends(255.any) logical_or(1,100)");
ok( call_checkit_check_config("256; depends(255.range(1,100)); any\n") , "simple tag depends(255.range(1,100)) any");
ok(!call_checkit_check_config("0; depends(255.range(1,100)); any\n") , "simple non-tag depends(255.range(1,100)) any");
ok( call_checkit_check_config("256; depends(255.range(1,100)); any\n") , "simple tag depends(255.range(1,100)) any");
ok( call_checkit_check_config("256; depends(255.range(1,100)); only(1)\n") , "simple tag depends(255.range(1,100)) only(1)");
ok( call_checkit_check_config("256; depends(255.range(1,100)); ntupel(1,1,1)\n") , "simple tag depends(255.range(1,100)) ntupel(1,1,1)");
ok( call_checkit_check_config("256; depends(255.range(1,100)); regex(\"string\")\n") , "simple tag depends(255.range(1,100)) regex(\"string\")");
ok( call_checkit_check_config("256; depends(255.range(1,100)); range(1,100)\n") , "simple tag depends(255.range(1,100)) range(1,100)");
ok( call_checkit_check_config("256; depends(255.range(1,100)); range(100,1)\n") , "simple tag depends(255.range(1,100)) range(100,1)");
ok( call_checkit_check_config("256; depends(255.range(1,100)); logical_or(100,1)\n") , "simple tag depends(255.range(1,100)) logical_or(100,1)");
ok( call_checkit_check_config("256; depends(255.range(1,100)); logical_or(1,100)\n") , "simple tag depends(255.range(1,100)) logical_or(1,100)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); any\n") , "simple tag depends(255.range(0,1)) any");
ok(!call_checkit_check_config("0; depends(255.range(0,1)); any\n") , "simple non-tag depends(255.range(0,1)) any");
ok( call_checkit_check_config("256; depends(255.range(0,1)); any\n") , "simple tag depends(255.range(0,1)) any");
ok( call_checkit_check_config("256; depends(255.range(0,1)); only(1)\n") , "simple tag depends(255.range(0,1)) only(1)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); ntupel(1,1,1)\n") , "simple tag depends(255.range(0,1)) ntupel(1,1,1)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); regex(\"string\")\n") , "simple tag depends(255.range(0,1)) regex(\"string\")");
ok( call_checkit_check_config("256; depends(255.range(0,1)); range(1,100)\n") , "simple tag depends(255.range(0,1)) range(1,100)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); range(100,1)\n") , "simple tag depends(255.range(0,1)) range(100,1)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); logical_or(100,1)\n") , "simple tag depends(255.range(0,1)) logical_or(100,1)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); logical_or(1,100)\n") , "simple tag depends(255.range(0,1)) logical_or(1,100)");
ok( call_checkit_check_config("256; depends(255.range(0,1)); logical_or(only(100),only(1))\n") , "simple tag depends(255.range(0,1)) logical_or(only(100),only(1)))");
ok( call_checkit_check_config("256; depends(255.range(0,1)); logical_or(only(1),only(100))\n") , "simple tag depends(255.range(0,1)) logical_or(only(1),only(100)))");
ok( call_checkit_check_config("258; depends(262.2); ntupel(8,8,8)\n") , "simple tag depends(262.2); ntupel(8,8,8)");
#
ok( call_checkit_check_config("256; optdepends(255.1); any\n") , "simple tag optdepends(255.1) any");
ok(!call_checkit_check_config("0; optdepends(255.1); any\n") , "simple non-tag optdepends(255.1) any");
ok( call_checkit_check_config("256; optdepends(255.1); any\n") , "simple tag optdepends(255.1) any");
ok( call_checkit_check_config("256; optdepends(255.1); only(1)\n") , "simple tag optdepends(255.1) only(1)");
ok( call_checkit_check_config("256; optdepends(255.1); ntupel(1,1,1)\n") , "simple tag optdepends(255.1) ntupel(1,1,1)");
ok( call_checkit_check_config("256; optdepends(255.1); regex(\"string\")\n") , "simple tag optdepends(255.1) regex(\"string\")");
ok( call_checkit_check_config("256; optdepends(255.1); range(1,100)\n") , "simple tag optdepends(255.1) range(1,100)");
ok( call_checkit_check_config("256; optdepends(255.1); range(100,1)\n") , "simple tag optdepends(255.1) range(100,1)");
ok( call_checkit_check_config("256; optdepends(255.1); logical_or(100,1)\n") , "simple tag optdepends(255.1) logical_or(100,1)");
ok( call_checkit_check_config("256; optdepends(255.1); logical_or(1,100)\n") , "simple tag optdepends(255.1) logical_or(1,100)");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); any\n") , "simple tag optdepends(255.regex(\"test\")) any");
ok(!call_checkit_check_config("0; optdepends(255.regex(\"test\")); any\n") , "simple non-tag optdepends(255.regex(\"test\")) any");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); any\n") , "simple tag optdepends(255.regex(\"test\")) any");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); only(1)\n") , "simple tag optdepends(255.regex(\"test\")) only(1)");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); ntupel(1,1,1)\n") , "simple tag optdepends(255.regex(\"test\")) ntupel(1,1,1)");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); regex(\"string\")\n") , "simple tag optdepends(255.regex(\"test\")) regex(\"string\")");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); range(1,100)\n") , "simple tag optdepends(255.regex(\"test\")) range(1,100)");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); range(100,1)\n") , "simple tag optdepends(255.regex(\"test\")) range(100,1)");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); logical_or(100,1)\n") , "simple tag optdepends(255.regex(\"test\")) logical_or(100,1)");
ok( call_checkit_check_config("256; optdepends(255.regex(\"test\")); logical_or(1,100)\n") , "simple tag optdepends(255.regex(\"test\")) logical_or(1,100)");
ok( call_checkit_check_config("256; optdepends(255.any); any\n") , "simple tag optdepends(255.any) any");
ok(!call_checkit_check_config("0; optdepends(255.any); any\n") , "simple non-tag optdepends(255.any) any");
ok( call_checkit_check_config("256; optdepends(255.any); any\n") , "simple tag optdepends(255.any) any");
ok( call_checkit_check_config("256; optdepends(255.any); only(1)\n") , "simple tag optdepends(255.any) only(1)");
ok( call_checkit_check_config("256; optdepends(255.any); ntupel(1,1,1)\n") , "simple tag optdepends(255.any) ntupel(1,1,1)");
ok( call_checkit_check_config("256; optdepends(255.any); regex(\"string\")\n") , "simple tag optdepends(255.any) regex(\"string\")");
ok( call_checkit_check_config("256; optdepends(255.any); range(1,100)\n") , "simple tag optdepends(255.any) range(1,100)");
ok( call_checkit_check_config("256; optdepends(255.any); range(100,1)\n") , "simple tag optdepends(255.any) range(100,1)");
ok( call_checkit_check_config("256; optdepends(255.any); logical_or(100,1)\n") , "simple tag optdepends(255.any) logical_or(100,1)");
ok( call_checkit_check_config("256; optdepends(255.any); logical_or(1,100)\n") , "simple tag optdepends(255.any) logical_or(1,100)");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); any\n") , "simple tag optdepends(255.range(1,100)) any");
ok(!call_checkit_check_config("0; optdepends(255.range(1,100)); any\n") , "simple non-tag optdepends(255.range(1,100)) any");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); any\n") , "simple tag optdepends(255.range(1,100)) any");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); only(1)\n") , "simple tag optdepends(255.range(1,100)) only(1)");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); ntupel(1,1,1)\n") , "simple tag optdepends(255.range(1,100)) ntupel(1,1,1)");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); regex(\"string\")\n") , "simple tag optdepends(255.range(1,100)) regex(\"string\")");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); range(1,100)\n") , "simple tag optdepends(255.range(1,100)) range(1,100)");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); range(100,1)\n") , "simple tag optdepends(255.range(1,100)) range(100,1)");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); logical_or(100,1)\n") , "simple tag optdepends(255.range(1,100)) logical_or(100,1)");
ok( call_checkit_check_config("256; optdepends(255.range(1,100)); logical_or(1,100)\n") , "simple tag optdepends(255.range(1,100)) logical_or(1,100)");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); any\n") , "simple tag optdepends(255.range(0,1)) any");
ok(!call_checkit_check_config("0; optdepends(255.range(0,1)); any\n") , "simple non-tag optdepends(255.range(0,1)) any");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); any\n") , "simple tag optdepends(255.range(0,1)) any");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); only(1)\n") , "simple tag optdepends(255.range(0,1)) only(1)");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); ntupel(1,1,1)\n") , "simple tag optdepends(255.range(0,1)) ntupel(1,1,1)");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); regex(\"string\")\n") , "simple tag optdepends(255.range(0,1)) regex(\"string\")");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); range(1,100)\n") , "simple tag optdepends(255.range(0,1)) range(1,100)");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); range(100,1)\n") , "simple tag optdepends(255.range(0,1)) range(100,1)");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); logical_or(only(100),only(1))\n") , "simple tag optdepends(255.range(0,1)) logical_or(only(100),only(1)))");
ok( call_checkit_check_config("256; optdepends(255.range(0,1)); logical_or(only(1),only(100))\n") , "simple tag optdepends(255.range(0,1)) logical_or(only(1),only(100)))");
#
my $grammarfile="$testdir/grammar";
my $grammar=<<"GRAMMAR";
256; mandatory; any

GRAMMAR
write_file($grammarfile, $grammar);
ok( call_checkit_check_config("include($grammarfile)\n"), "include( $grammarfile )");

ok( call_checkit_check_config("256; mandatory; only(2^32)\n") , "simple tag only(2^32)");
ok( call_checkit_check_config("256; mandatory; only(0x0)\n") , "simple tag only(0x0)");
ok( call_checkit_check_config("256; mandatory; only(0x0000)\n") , "simple tag only(0x0000)");
ok( call_checkit_check_config("256; mandatory; only(0xffff)\n") , "simple tag only(0xffff)");
#


### next tests should fail!
ok(!call_checkit_check_config("0; mandatory; any\n") , "simple non-tag (0) mandatory any");
ok(!call_checkit_check_config("253; mandatory; any\n") , "simple non-tag (253) mandatory any");
ok(!call_checkit_check_config("65536; mandatory; any\n") , "simple non-tag (65536) mandatory any");
ok(!call_checkit_check_config("2^16; mandatory; any\n") , "simple non-tag (2^16) mandatory any");
#ok(!call_checkit_check_config("0x; mandatory; any\n") , "simple non-tag (0x) mandatory any");
ok(!call_checkit_check_config("0x0; mandatory; any\n") , "simple non-tag (0x0) mandatory any");
#ok(!call_checkit_check_config("0xffx; mandatory; any\n") , "simple non-tag (0xffx) mandatory any");
#ok(!call_checkit_check_config("0xff0x; mandatory; any\n") , "simple non-tag (0xff0x) mandatory any");
#ok(!call_checkit_check_config("0xg; mandatory; any\n") , "simple non-tag (0xg) mandatory any");
ok(!call_checkit_check_config("0x10000; mandatory; any\n") , "simple non-tag (0x10000) mandatory any");
#ok(!call_checkit_check_config("256; mandatory; only(1,1,1)\n") , "simple tag mandatory only(1,1,1)");
#ok(!call_checkit_check_config("256; mandatory; ntupel(\"1\",\"1\",\"1\")\n") , "simple tag mandatory ntupel(\"1\",\"1\",\"1\")");
#ok(!call_checkit_check_config("256, mandatory; any\n") , "simple non-tag (256, mandatory)");
#ok(!call_checkit_check_config("256; mandatory, any\n") , "simple tag (mandatory, any)");

### next tests checking cornerstones of parser
my $expected;
####
$expected =<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=001 */ EXEC:    fc_tag tag=259

EXPECT

is( call2_checkit_check_config("259; mandatory; any\n"), $expected, "parsercheck, simple1");
####
$expected =<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=002 */ EXEC:    fc_tag tag=259

/* lineno=001 */ EXEC:    fc_tag tag=260

EXPECT

is( call2_checkit_check_config("260; mandatory; any\n259; mandatory; any\n"), $expected, "parsercheck, simple2");
####
$expected =<<'EXPECT';
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=001 */ EXEC:    fc_tag_has_value_matching_regex tag=262 regex_stack[0]=^[[:print:]]*$

EXPECT

is( call2_checkit_check_config("262; mandatory; printascii\n"), $expected, "parsercheck, printascii");
####
$expected =<<'EXPECT';
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=001 */ EXEC:    fc_tag_has_value tag=256 top i stack=(51966)

EXPECT

is( call2_checkit_check_config("0x100; mandatory; only(0xcafe)\n"), $expected, "parsercheck, hex");

####
$expected =<<'EXPECT';
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=001 */ EXEC:    fc_tag_has_value tag=256 top i stack=(51966)

EXPECT

is( call2_checkit_check_config("2^7; mandatory; only(0xcafe)\n"), $expected, "parsercheck, exponent");

####
$expected =<<'EXPECT';
((( parse config file )))
wrong tag (should be >253) at line 1 (error at '0xffffffff01')
EXPECT

is( call2_checkit_check_config("0xffffffff01; mandatory; only(0xcafe)\n"), $expected, "parsercheck, tag exceeded");

####
$expected =<<'EXPECT';
((( parse config file )))
wrong tag (should be <65536) at line 1 (error at '65536')
EXPECT

is( call2_checkit_check_config("65536; mandatory; only(0xcafe)\n"), $expected, "parsercheck, tag exceeded2");

####
$expected =<<'EXPECT';
((( parse config file )))
wrong tag (should be <65536) at line 1 (error at '0x10000')
EXPECT

is( call2_checkit_check_config("0x10000; mandatory; only(0xcafe)\n"), $expected, "parsercheck, tag exceeded3");



####
$expected=<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=001 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(0)
/* lineno=001 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=001 */ EXEC:    fc_internal_logic_combine_close tag=259

EXPECT
is( call2_checkit_check_config("259; depends(262.0); logical_or(1,2,32773)\n"), $expected, "parsercheck, depends, logical_or");
####
$expected =<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=002 */ EXEC:    fc_tag tag=260

/* lineno=001 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(0)
/* lineno=001 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(2)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=001 */ EXEC:    fc_internal_logic_combine_close tag=259

EXPECT

is( call2_checkit_check_config("259; depends(262.0); logical_or(2,32773)\n260; mandatory; any\n"), $expected, "parsercheck, depends, logical_or2");
####
$expected =<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=002 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(0)
/* lineno=002 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(2)

/* lineno=002 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=002 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=002 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=001 */ EXEC:    fc_tag tag=260

EXPECT

is( call2_checkit_check_config("260; mandatory; any\n259; depends(262.0); logical_or(2,32773)\n"), $expected, "parsercheck, depends, logical_or3");
####
my $rules =<<RULES;
# 259   0103    Compression     Compression scheme used on the image data 
### (1 means no compression, 2 CCIT Group 3, 32773 PackBits)
### black/white (BitsPerSample is optional for b/w)
259; depends(262.0); logical_or(1,2,32773)
259; depends(262.1); logical_or(1,2,32773)
### grayscale (BitsPerSample must exist)
259; depends(258.4); logical_or(1,32773)
259; depends(258.8); logical_or(1,32773)
### colormap
259; depends(262.3); logical_or(1,32773)
### RGB
259; depends(262.2); logical_or(1,32773)
RULES
$expected =<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=012 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(2)
/* lineno=012 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=012 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=012 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=012 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=010 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(3)
/* lineno=010 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=010 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=010 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=010 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=008 */ PRECOND: fc_tag_has_value_quiet tag=258 top i stack=(8)
/* lineno=008 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=008 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=008 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=008 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=007 */ PRECOND: fc_tag_has_value_quiet tag=258 top i stack=(4)
/* lineno=007 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=007 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=007 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=007 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=005 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(1)
/* lineno=005 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=005 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=005 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=005 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=005 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=004 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(0)
/* lineno=004 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=004 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=004 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=004 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=004 */ EXEC:    fc_internal_logic_combine_close tag=259

EXPECT

is( call2_checkit_check_config($rules), $expected, "parsercheck, depends, complex logical_or");
####
$expected=<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=005 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(0)
/* lineno=005 */ PRECOND: fc_tag_has_valid_type tag=259 top i stack=(1)
/* lineno=005 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=005 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=005 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=005 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=005 */ EXEC:    fc_internal_logic_combine_close tag=259

/* lineno=004 */ EXEC:    fc_tagorder tag=0

/* lineno=004 */ EXEC:    fc_all_IFDs_are_word_aligned tag=0

/* lineno=003 */ EXEC:    fc_all_offsets_are_not_zero tag=0

/* lineno=003 */ EXEC:    fc_all_offsets_are_used_once_only tag=0

/* lineno=003 */ EXEC:    fc_all_offsets_are_word_aligned tag=0

/* lineno=001 */ EXEC:    fc_has_only_one_ifd tag=0

EXPECT
is( call2_checkit_check_config("mode(baseline)\nmode(enable_type_checks)\nmode(enable_offset_checks)\nmode(enable_ifd_checks)\n259; depends(262.0); logical_or(1,2,32773)\n"), $expected, "parsercheck, modes, depends, logical_or");
####
$rules=<<RULES;
259; depends(262.0); logical_or(1,2,32773)
262; mandatory; logical_or(0,1,2)
RULES
$expected=<<EXPECT;
((( parse config file )))
((( execute execution plan )))
((( clean execution plan )))
((( print internal execution plan )))

/* the rules are in stack order, the top comes first */

/* lineno=002 */ EXEC:    fc_internal_logic_combine_open tag=262 top i stack=(0)

/* lineno=002 */ EXEC:    fc_tag_has_value tag=262 top i stack=(0)

/* lineno=002 */ EXEC:    fc_tag_has_value tag=262 top i stack=(1)

/* lineno=002 */ EXEC:    fc_tag_has_value tag=262 top i stack=(2)

/* lineno=002 */ EXEC:    fc_internal_logic_combine_close tag=262

/* lineno=001 */ PRECOND: fc_tag_has_value_quiet tag=262 top i stack=(0)
/* lineno=001 */ EXEC:    fc_internal_logic_combine_open tag=259 top i stack=(1)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(1)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(2)

/* lineno=001 */ EXEC:    fc_tag_has_value tag=259 top i stack=(32773)

/* lineno=001 */ EXEC:    fc_internal_logic_combine_close tag=259

EXPECT
is( call2_checkit_check_config($rules), $expected, "parsercheck, regression_bug, depends, logical_or");



#########

cleanup();



