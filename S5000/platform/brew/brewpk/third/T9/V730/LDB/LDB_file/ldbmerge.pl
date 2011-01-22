#*******************************************************************************
#*******************************************************************************
#**                                                                           **
#**                    COPYRIGHT 1997-2005 TEGIC COMMUNICATIONS               **
#**                                                                           **
#**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **
#**                                                                           **
#**     This software is supplied under the terms of a license agreement      **
#**     or non-disclosure agreement with Tegic Communications and may not     **
#**     be copied or disclosed except in accordance with the terms of that    **
#**     agreement.                                                            **
#**                                                                           **
#*******************************************************************************
#**                                                                           **
#**     FileName:   LDBMERGE.PL                                               **
#**                                                                           **
#**  Description:   Perl script to split a .LDB file into code modules        **
#**                 for building directly into T9 release package.            **
#**                                                                           **
#**                 Also generates file t9gen.c which contains data access    **
#**                 routine T9ReadLdbData().                                  **
#**                                                                           **
#*******************************************************************************
#******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/

#----------------------------------------------------------------------------
# Global variables
#----------------------------------------------------------------------------
$pagesize = 1024;
$pagecount = 0;
$bytecount = 0;
$ifdefs = 0;
$outpath = "";
$offsetsize;
$addifdefs = 0;
$gen_ldb_ver_file = 0;

$LANGUAGEID_OFFSET = 0x36;  # First data field for Ver1 format LDB
$LANGUAGEID_OFFSET_PTI = 0x28;
$T9PTIDBTYPE = 9;

#----------------------------------------------------------------------------
# Set up hash for language IDs to language names
#----------------------------------------------------------------------------
$langnames{0x0001} = "Arabic";
$langnames{0x0002} = "Bulgarian";
$langnames{0x0003} = "Catalan";
$langnames{0x0004} = "Chinese";
$langnames{0x0005} = "Czech";
$langnames{0x0006} = "Danish";
$langnames{0x0007} = "German";
$langnames{0x0008} = "Greek";
$langnames{0x0009} = "English";
$langnames{0x000A} = "Spanish";
$langnames{0x000b} = "Finnish";
$langnames{0x000c} = "French";
$langnames{0x000D} = "Hebrew";
$langnames{0x000E} = "Hungarian";
$langnames{0x000F} = "Icelandic";
$langnames{0x0010} = "Italian";
$langnames{0x0011} = "Japanese";
$langnames{0x0012} = "Korean";
$langnames{0x0013} = "Dutch";
$langnames{0x0014} = "Norwegian";
$langnames{0x0015} = "Polish";
$langnames{0x0016} = "Portuguese";
$langnames{0x0017} = "RhaetoRoman";
$langnames{0x0018} = "Romanian";
$langnames{0x0019} = "Russian";
$langnames{0x001A} = "SerboCroat";
$langnames{0x001B} = "Slovak";
$langnames{0x001C} = "Albanian";
$langnames{0x001D} = "Swedish";
$langnames{0x001E} = "Thai";
$langnames{0x001F} = "Turkish";
$langnames{0x0020} = "Urdu";
$langnames{0x0021} = "Indonesian";
$langnames{0x0022} = "Ukrainian";
$langnames{0x0023} = "Byelorussian";
$langnames{0x0024} = "Slovenian";
$langnames{0x0025} = "Estonian";
$langnames{0x0026} = "Latvian";
$langnames{0x0027} = "Lithuanian";
$langnames{0x0028} = "Maori";
$langnames{0x0029} = "Farsi";
$langnames{0x002A} = "Vietnamese";
$langnames{0x002b} = "Laotian";
$langnames{0x002C} = "Kampuchean";
$langnames{0x002D} = "Basque";
$langnames{0x002E} = "Sorbian";
$langnames{0x002F} = "Macedonian";
$langnames{0x0030} = "Sutu";
$langnames{0x0031} = "Tsonga";
$langnames{0x0032} = "Tswana";
$langnames{0x0033} = "Venda";
$langnames{0x0034} = "Xhosa";
$langnames{0x0035} = "Zulu";
$langnames{0x0036} = "Afrikaans";
$langnames{0x0038} = "Faeroese";
$langnames{0x0039} = "Hindi";
$langnames{0x003A} = "Maltese";
$langnames{0x003b} = "Sami";
$langnames{0x003c} = "ScotsGaelic";
$langnames{0x003e} = "Malay";
$langnames{0x0041} = "Swahili";
$langnames{0x0042} = "Afar";
$langnames{0x0043} = "Abkhazian";
$langnames{0x0044} = "Amharic";
$langnames{0x0045} = "Assamese";
$langnames{0x0046} = "Aymara";
$langnames{0x0047} = "Azerbaijani";
$langnames{0x0048} = "Bashkir";
$langnames{0x0049} = "Bihari";
$langnames{0x004a} = "Bislama";
$langnames{0x004b} = "Bengali";
$langnames{0x004c} = "Tibetan";
$langnames{0x004d} = "Breton";
$langnames{0x004e} = "Corsican";
$langnames{0x004f} = "Welsh";
$langnames{0x0050} = "Bhutani";
$langnames{0x0051} = "Esperanto";
$langnames{0x0052} = "Fiji";
$langnames{0x0053} = "Frisian";
$langnames{0x0054} = "Irish";
$langnames{0x0055} = "Galician";
$langnames{0x0056} = "Guarani";
$langnames{0x0057} = "Gujarati";
$langnames{0x0058} = "Hausa";
$langnames{0x0059} = "Croatian";
$langnames{0x005a} = "Armenian";
$langnames{0x005b} = "Interlingua";
$langnames{0x005c} = "Interlingue";
$langnames{0x005d} = "Inupiak";
$langnames{0x005e} = "Inuktitut";
$langnames{0x005f} = "Javanese";
$langnames{0x0060} = "Georgian";
$langnames{0x0061} = "Kazakh";
$langnames{0x0062} = "Greenlandic";
$langnames{0x0063} = "Kannada";
$langnames{0x0064} = "Kashmiri";
$langnames{0x0065} = "Kurdish";
$langnames{0x0066} = "Kirghiz";
$langnames{0x0067} = "Latin";
$langnames{0x0068} = "Lingala";
$langnames{0x0069} = "Malagasy";
$langnames{0x006a} = "Malayalam";
$langnames{0x006b} = "Mongolian";
$langnames{0x006c} = "Moldavian";
$langnames{0x006d} = "Marathi";
$langnames{0x006e} = "Burmese";
$langnames{0x006f} = "Nauru";
$langnames{0x0070} = "Nepali";
$langnames{0x0071} = "Occitan";
$langnames{0x0072} = "Oromo";
$langnames{0x0073} = "Oriya";
$langnames{0x0074} = "Punjabi";
$langnames{0x0075} = "Pashto";
$langnames{0x0076} = "Quechua";
$langnames{0x0077} = "Kirundi";
$langnames{0x0078} = "Kiyarwanda";
$langnames{0x0079} = "Sanskrit";
$langnames{0x007a} = "Sindhi";
$langnames{0x007b} = "Sangho";
$langnames{0x007c} = "Singhalese";
$langnames{0x007d} = "Samoan";
$langnames{0x007e} = "Shona";
$langnames{0x007f} = "Somali";
$langnames{0x0080} = "Serbian";
$langnames{0x0081} = "Siswati";
$langnames{0x0082} = "Sesotho";
$langnames{0x0083} = "Sudanese";
$langnames{0x0084} = "Tamil";
$langnames{0x0085} = "Telugu";
$langnames{0x0086} = "Tajik";
$langnames{0x0087} = "Tigrinya";
$langnames{0x0088} = "Turkmen";
$langnames{0x0089} = "Tagalog";
$langnames{0x008a} = "Setswana";
$langnames{0x008b} = "Tonga";
$langnames{0x008c} = "Tatar";
$langnames{0x008d} = "Twi";
$langnames{0x008e} = "Uigur";
$langnames{0x008f} = "Uzbek";
$langnames{0x0090} = "Volapuk";
$langnames{0x0091} = "Wolof";
$langnames{0x0092} = "Yiddish";
$langnames{0x0093} = "Yorouba";
$langnames{0x0094} = "Zhuang";
$langnames{0x00ff} = "Null";
$secondarynames{0x01} = "Traditional";
$secondarynames{0x02} = "Simplified";
$secondarynames{0x11} = "TraditionalPTI";
$secondarynames{0x12} = "SimplifiedPTI";

#----------------------------------------------------------------------------
# Define the various text fragments ouput to the c source files
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Common header text at start of all Tegic source files
#----------------------------------------------------------------------------
my @Time = localtime();
my $Year = $Time[5] + 1900;
@commonheader = (
"/*******************************************************************************",
";*******************************************************************************",
";**                                                                           **",
";**                    COPYRIGHT $Year TEGIC COMMUNICATIONS                    **",
";**                                                                           **",
";**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **",
";**                                                                           **",
";**     This software is supplied under the terms of a license agreement      **",
";**     or non-disclosure agreement with Tegic Communications and may not     **",
";**     be copied or disclosed except in accordance with the terms of that    **",
";**     agreement.                                                            **",
";**                                                                           **",
";*******************************************************************************",
);


#----------------------------------------------------------------------------
# Remainder of header for t9gen.c source file -- gets appended with external
# function definition for each of the LDB access functions.
#----------------------------------------------------------------------------
@t9genheader = (
";**                                                                           **",
";**     FileName: t9gen.c                                                     **",
";**                                                                           **",
";**  Description: Access to internal language data base files.                **",
";**                                                                           **",
";*******************************************************************************",
";******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/",
"",
"#define PAGESIZE    1024",
"",
"#include \"t9api.h\"",
"",
);

@t9gencode = (
"",
"/*---------------------------------------------------------------------------",
" *",
" *  Function: T9ReadLdbData",
" *                                 ",
" *  Synopsis: Callback from T9Core to access a non-Chinese language database",
" *                                           ",
" *     Input: T9FieldInfo *pFieldInfo -- Field info struct that holds database number",
" *            T9U32 dwOffset - byte offset from LDB start to read from",
" *            T9FARPOINTER *ppbDst - Ptr to a ptr to the data cache.",
" *            T9U32 *pdwStart - Ptr to offset of start of data.",
" *            T9NUM nSize    - number of bytes desired (but only 1 is required)",
" *            T9NUM mCacheIndex - cache number (igored for compiled in LDBs)",
" * ",
" *    Return: Number of bytes available at *pdwStart",
" *                                                                            ",
" *---------------------------------------------------------------------------*/",
"T9U32 T9FARCALL T9ReadLdbData(T9FieldInfo *pFieldInfo, T9U32 dwOffset, T9FARPOINTER *ppbDst, T9U32 *pdwStart, T9UINT nSize, T9MINOR mCacheIndex)",
"{",
);

#----------------------------------------------------------------------------
# Ending text for t9gen.c source file
#----------------------------------------------------------------------------
@t9gentail = (
"",
"   default :",
"       return 0;",
"   }",
"",
"    return nSize;",
"}",
"",
);

#----------------------------------------------------------------------------
# Print out eof
#----------------------------------------------------------------------------
@t9geneof = (
"/*-----------------------------------< eof >----------------------------------*/",
);

#----------------------------------------------------------------------------
# Remainder of header text for each LDB segment source file -- program will
# replace 'lxxbxx' text with segment's actual language number and block number
#----------------------------------------------------------------------------
@t9datheader = (
";**                                                                           **",
";**     FileName: lxxbxx.c                                                    **",
";**                                                                           **",
";**  Description: Data and access function for internal LDB data              **",
";**                                                                           **",
";*******************************************************************************",
";******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/",
"#include \"t9.h\"",
"",
"const T9U8 T9FARDATA lxxbxx[] = {",
"",
);

#----------------------------------------------------------------------------
# Remainder of header for t9lang.c source file -- gets appended with external
# function definition for each of the LDB access functions.
#----------------------------------------------------------------------------
@t9langheader = (
";**                                                                           **",
";**     FileName: t9lang.c                                                    **",
";**                                                                           **",
";**  Description: File to include T9 language database files and access       **",
";**               function                                                    **",
";**                                                                           **",
";*******************************************************************************",
";******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/",
"",
"",
"#include \"t9.h\"",
"",
"#include \"t9gen.c\"       /* Pull in main access function module */",
"CHINESEGENPLACEHOLDER",
"",
"CHINESEIFDEFSPLACEHOLDER",
);

#----------------------------------------------------------------------------
# Ending text for each t9lang.c source file
#----------------------------------------------------------------------------
@t9langtail = @t9geneof;

#----------------------------------------------------------------------------
# Common header text at start of all Tegic source files
#----------------------------------------------------------------------------
@chinesecommonheader = (
"/*******************************************************************************",
";*******************************************************************************",
";**                                                                           **",
";**                    COPYRIGHT $Year TEGIC COMMUNICATIONS                    **",
";**                                                                           **",
";**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **",
";**                                                                           **",
";**     This software is supplied under the terms of a license agreement      **",
";**     or non-disclosure agreement with Tegic Communications and may not     **",
";**     be copied or disclosed except in accordance with the terms of that    **",
";**     agreement.                                                            **",
";**                                                                           **",
";*******************************************************************************",
);

#----------------------------------------------------------------------------
# Remainder of header for main Chinese data block
#----------------------------------------------------------------------------
@chinesemainheader = (
";**                                                                           **",
";**     FileName: FILENAME                                                    **",
";**                                                                           **",
";**  Description: Define Chinese data pointers structure                      **",
";**                                                                           **",
";*******************************************************************************",
";******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/",
"",
"#include \"t9api.h\"",
"",
);

#----------------------------------------------------------------------------
# Body of main Chinese data block file
#----------------------------------------------------------------------------
@chinesemainbody = (
"",
);

#----------------------------------------------------------------------------
# Ending text for each Chinese source file
#----------------------------------------------------------------------------
@chinesetail = @t9geneof;

#----------------------------------------------------------------------------
# Body of main Chinese data block file
#----------------------------------------------------------------------------
@chinesemainbody = (
"",
);

#----------------------------------------------------------------------------
# Body of Chinese LDB load function
#----------------------------------------------------------------------------
@t9chnloadfunchead = (
"/*---------------------------------------------------------------------------",
" *",
" *  Function: T9CCLoadLdb",
" *",
" *  Synopsis: Callback from T9Core to load the Chinese language database",
" *",
" *     Input: pCCFieldInfo   -- T9 Chinese Fieldinfo structure",
" *            pT9CCLdbHeader -- Pointer to the buffer that the callback",
" *                              function loads the LDB data to.",
" *",
" *    Return: T9STATNONE if the loading succeeds;",
" *            T9STATLOADLDBFAIL if the language ID in the T9 FieldInfo",
" *            structure is not valid.",
" *",
" *---------------------------------------------------------------------------*/",
"T9STATUS T9FARCALL T9CCLoadLdb(T9CCFieldInfo *pCCFieldInfo, T9ChineseData T9FARDATA *pT9CCLdbHeader)",
"{",
"    T9ChineseData const T9FARDATA *pLdbHeader;",
"    T9UINT i;",
"",
"    switch (pCCFieldInfo->G.nLdbNum) {",
"",
);

@t9chnloadbody = (
"",
);

@t9chnloadfunctail = (
"        default :",
"            return T9STATLOADLDBFAIL;",
"    }",
"",
"    /* Copy the header part */",
"    for (i = 0; i < T9CCLdbHeaderSize; i++)",
"        pT9CCLdbHeader->bHeader[i] = pLdbHeader->bHeader[i];",
"",
"    /* Copy block pointers */",
"    for (i = 0; i < T9CCLDBBLOCKCOUNT; i++)",
"        pT9CCLdbHeader->pBlockPointers[i] = pLdbHeader->pBlockPointers[i];",
"",
"    return T9STATNONE;",
"}",
);

#----------------------------------------------------------------------------
# Remainder of header for each Chinese data block file
#----------------------------------------------------------------------------
@chineseblockheader = (
";**                                                                           **",
";**     FileName: lxxbxx.c                                                    **",
";**                                                                           **",
";**  Description: Data for Chinese LDB                                        **",
";**                                                                           **",
";*******************************************************************************",
";******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/",
"#include \"t9.h\"",
"",
"const T9U8 T9FARDATA lxxbxx[] = {",
"",
);

#----------------------------------------------------------------------------
# Chinese Globals
#----------------------------------------------------------------------------

$sizeof_dword = 4;
$sizeof_word = 2;
$sizeof_byte = 1;

# ********************************Warning********************************
# This number no longer as to agree with the value of T9CCLDBBLOCKCOUNT
# in t9.h.  It just needs to be bigger than the highest number of blocks
# that might appear in the database
# ***********************************************************************
$T9CCLDBBLOCKCOUNT  = 66;    # this number must be updated if number of blocks changes
# ***********************************************************************

$LENGTH_COPYRIGHT     = 0x20;
$skip_chinese = $LENGTH_COPYRIGHT + 2;  # skip the copyright and 2 DB format bytes
$T9CHINESEDATAVALID   = 0x1234;
$CHINESE_PRIMARY_LANG_ID = 0x0004;
$offset_T9ChineseData_BlockSize = $LENGTH_COPYRIGHT + 28;
$offsetof_T9ChineseData_uDwordOrPointer = $offset_T9ChineseData_BlockSize + $sizeof_word * $T9CCLDBBLOCKCOUNT;
$offset_T9ChineseData = $offsetof_T9ChineseData_uDwordOrPointer + $sizeof_dword * $T9CCLDBBLOCKCOUNT;
$offsetof_Primary_Language_ID = $skip_chinese + 2;
$offsetof_Secondary_Language_ID = $offsetof_Primary_Language_ID + $sizeof_byte;
$sizeof_T9ChineseData = $offsetof_T9ChineseData_uDwordOrPointer + ($sizeof_dword * $T9CCLDBBLOCKCOUNT);

#----------------------------------------------------------------------------
# Chinese Database handling
#----------------------------------------------------------------------------
sub chinesemerge {

    #
    #
    #  Set up variables.
    #
    #

    (my $dictref, my $dict_len) = @_;
    my $out_file_name  = "";
    my $out_ext = "c";

    my $binary_mode = 0;       # do binary split
    my $out_number  = 0;        # start the output file names at zero
    my $out_chunks  = 0;      # How many data chunks are output to files

    my $lang_id = 0;
    my $lang_sec_id = 0;
    my $Chinese_main_name = "";

    $dict = $$dictref;    #The raw data is passed by reference

    if (   (   (ord(substr($dict, $skip_chinese, 1)) == ($T9CHINESEDATAVALID & 0xff))
               && (ord(substr($dict, $skip_chinese + 1, 1)) == (($T9CHINESEDATAVALID >> 8) & 0xff))
           ) ||
           (   (ord(substr($dict, $skip_chinese + 1, 1)) == ($T9CHINESEDATAVALID & 0xff))
           && (ord(substr($dict, $skip_chinese, 1)) == (($T9CHINESEDATAVALID >> 8) & 0xff))
       ))
    {
        $big_endian = 0;
    }
    else
    {
        die "Database does not have a valid Chinese dictionary signature!\n";
    }

    # Check for Chinese Database
    $lang_id = my_number_from_dict($offsetof_Primary_Language_ID, $sizeof_byte);
    if ($CHINESE_PRIMARY_LANG_ID != $lang_id)
    {
        die "Database does not have a valid Chinese primary language ID!\n";
    }

    $lang_sec_id = my_number_from_dict($offsetof_Secondary_Language_ID, $sizeof_byte);

    printf ("Language ID = %02d%02d\n", $lang_sec_id, $lang_id);

    # Output case statement
    push (@t9genheader, "");
    if ($addifdefs) {
        push (@t9genheader, "#ifdef T9LANG_$langnames{$lang_id}$secondarynames{$lang_sec_id}");
    }

    # And verify no duplicate language ID's used
    if ($langs{$lang_id + 256 * $lang_sec_id} eq "") {
        $langs{$lang_id + 256 * $lang_sec_id} = "taken";
    }
    else {
        die "\nError -- Language ID $lang_sec_id,$lang_id used by two database files\n";
    }

    #Create T9ChineseData Name based on language IDs
    $Chinese_main_name = sprintf("l%02d%02d", $lang_sec_id, $CHINESE_PRIMARY_LANG_ID);

    push(@t9langheader, "");
    push (@chinesemainbody, "");
    push (@chinesemainheader, "");

    if ($addifdefs) {
        push (@t9langheader, "#ifdef   T9LANG_$langnames{$lang_id}$secondarynames{$lang_sec_id}");
        push (@chinesemainheader, "#ifdef   T9LANG_$langnames{$lang_id}$secondarynames{$lang_sec_id}");
        push (@chinesemainbody, "#ifdef   T9LANG_$langnames{$lang_id}$secondarynames{$lang_sec_id}");
    }

    push (@chinesemainbody, "/* Data pointers for $secondarynames{$lang_sec_id} $langnames{$lang_id} */");
    push (@chinesemainbody, "T9ChineseData const T9FARDATA $Chinese_main_name = {");

    #Initial Structure Data

    for ($i = 0; $i < $offsetof_T9ChineseData_uDwordOrPointer; $i += $sizeof_byte) {
        push (@chinesemainbody, sprintf("    0x%02X,", my_number_from_dict($i, $sizeof_byte)));
    }

    # The first pointer in the database points to the first data block in the database which is just
    # after the pointer section.  Save the address of this.

    $currentData = $offset_T9ChineseData;


    # The Chinese database consists of 3 sections.
    #   Section 1 = Data valid, Version and language information
    #   Section 2 = Pointers to valid data blocks in the database
    #   Section 3 = Data for each block


    # Go through each pointer in the database printing the data associated with each one
    for ($i = 0; $i < $T9CCLDBBLOCKCOUNT; ++$i, ++$out_number)
    {
        $out_base_file_name = sprintf("l%02d%02db%02d", $lang_sec_id, $CHINESE_PRIMARY_LANG_ID, $i);

        $chunk_name = $out_base_file_name.".".$out_ext;


        # read data block size into $n

        $from = $offset_T9ChineseData_BlockSize + ($i * $sizeof_word);
        $n = my_number_from_dict($from, $sizeof_word);

        #       Debug info if Necessary
        #       print "From = " . $from . " FirstData = " . $FirstData . " n1 = " . $n1 . "  n = " . $n . "\n";

        if ($n)
        {
            my $filepath = $outpath.$chunk_name;
            unlink ("$filepath");

            open (FILE, ">$filepath") || die "Can't open $filepath for output!\n";

            if ($out_chunks == 0) {
                push (@t9langheader, "#include \"$chunk_name\"     /* $secondarynames{$lang_sec_id} $langnames{$lang_id}  */");
            }
            else {
                push (@t9langheader, "#include \"$chunk_name\"");
            }

            $out_chunks++;

            push (@chinesemainheader, "extern const T9U8 T9FARDATA $out_base_file_name\[\];");
            push (@chinesemainbody, "    (T9U8 const T9FARDATA *)$out_base_file_name,");
            # Print out the data file header
            foreach (@commonheader) {
                print FILE "$_\n";
            }
            foreach (@chineseblockheader) {
                $line = $_;
                $line =~ s/lxxbxx/$out_base_file_name/;
                print FILE "$line\n";
            }

            # Now print the actual data array
            $rowlength = 16;
            for ($k = 0; $k < $n; $k += $rowlength) {
                if ($k + $rowlength > $n) {
                    $rowlength = $n - $k;
                }
                for ($j = 0; $j < $rowlength; $j += $sizeof_byte) {
                    printf FILE ("0x%02X,", my_number_from_dict($currentData, $sizeof_byte));
                    $currentData += $sizeof_byte;
                }
                print FILE "\n";
            }
            print FILE "};\n";
            foreach (@chinesetail) {
                print FILE "$_\n";
            }
            close(FILE);
        }
        else
        {
            push (@chinesemainbody, "    0,");
        }
    }

    push (@chinesemainbody, "};");

    push (@t9genheader, "extern const T9ChineseData T9FARDATA $Chinese_main_name;");

    push (@$t9chnloadbody, "        /* Load $secondarynames{$lang_sec_id} $langnames{$lang_id} LDB */");
    push (@$t9chnloadbody, "        case ($lang_sec_id * 256 + $lang_id):");
    push (@$t9chnloadbody, sprintf("            pLdbHeader = &l%02d%02d;", $lang_sec_id, $lang_id));
    push (@$t9chnloadbody, "            break;");
    push (@$t9chnloadbody, "");

    if ($addifdefs) {
        push (@t9langheader, "#endif");
        push (@t9genheader, "#endif");
        push (@chinesemainheader, "#endif");
        push (@chinesemainbody, "#endif");
    }
}

#
#
#   Get a 32-bit value from global $dict (depends on global $big_endian)
#
#
sub my_number_from_dict
{
    my $idx = shift;
    my $len = shift;

    my @bytes = unpack("C$len", substr($dict, $idx, $len));

    if ($len == 1) {
        return $bytes[0];
    }
    elsif ($len == 2) {
        return $big_endian ? ($bytes[0] << 8) + $bytes[1] :
                             ($bytes[1] << 8) + $bytes[0];
    }
    elsif ($len == 4) {
        return $big_endian ? ($bytes[0] << 24) + ($bytes[1] << 16) + ($bytes[2] << 8) + $bytes[3] :
                             ($bytes[3] << 24) + ($bytes[2] << 16) + ($bytes[1] << 8) + $bytes[0];
    }
    else {
        die "Invalid data item length: $len";
    }
}


@ldb_ver_code_heading = (
"/*******************************************************************************",
";*******************************************************************************",
";**                                                                           **",
";**                    COPYRIGHT $Year TEGIC COMMUNICATIONS                   **",
";**                                                                           **",
";**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **",
";**                                                                           **",
";**     This software is supplied under the terms of a license agreement      **",
";**     or non-disclosure agreement with Tegic Communications and may not     **",
";**     be copied or disclosed except in accordance with the terms of that    **",
";**     agreement.                                                            **",
";**                                                                           **",
";**     FileName: lxxdbver.c                                                  **",
";**                                                                           **",
";**     Allowing ldb version string to be extracted when build as an DLL.     **",
";**                                                                           **",
";*******************************************************************************",
";******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/",
" ",
"/* format: mj.mm.layout.type lsecid.langid.encoding */",
);

@ldb_ver_code_function = (
"/*---------------------------------------------------------------------------",
" *                                                                           ",
" *  Function: T9GetVersionString                                               ",
" *                                                                           ",
" *  Synopsis: Allowing the plaform intergatrion LDB DLL to get the ldb       ",
" *            version string.                                                ",
" *                                                                           ",
" *     Input/Out: char* buf    - To hold the version string                  ",
" *                int   maxLen - The maximum number of char can be copied    ",
" *                                                                           ",
" *    Return: Number of chars copied.                                        ",
" *                                                                           ",
" *-------------------------------------------------------------------------*/",
"int T9GetVersionString(char* buf, int maxLen)",
"{",
"	 int count = 0;",
" ",
"	 for (count = 0; count < versionLen && count < maxLen; count++) {",
"		 buf[count] = szLdbVersion[count];",
"	 }",
" ",
"	 buf[count] = 0;",
"	 return (count);",
"}",
);

sub one_ldb_ver_file_chinese {

    (my $chinese_ldbdata) = @_;

    $dict = $$chinese_ldbdata;

    $offset_ldb_ver = 46;
    $offset_ldb_type = 33;
    $offset_ldb_layout = 32;
    $offset_ldb_encoding = 47;

    $langid = my_number_from_dict($offsetof_Primary_Language_ID, $sizeof_byte);
    $secid  = my_number_from_dict($offsetof_Secondary_Language_ID, $sizeof_byte);

    $ldb_ver       = my_number_from_dict($offset_ldb_ver, $sizeof_byte);
    $ldb_type      = my_number_from_dict($offset_ldb_type, $sizeof_byte);   
    $ldb_layout    = my_number_from_dict($offset_ldb_layout, $sizeof_byte);
    $ldb_encoding  = my_number_from_dict($offset_ldb_encoding, $sizeof_byte);

    $ldb_ver_string = 
         sprintf("T9 LDB: v%02d.%02d.%02d.%02d l%02d%02d.%02d", 
                ($ldb_ver >> 4),
                ($ldb_ver & 0x0f),
                $ldb_layout,
                $ldb_type,
                $secid,
                $langid,
                ($ldb_encoding >> 4)
                );

    push(@ldb_ver_code_heading, sprintf("static const char* szLdbVersion = \"%s\";", $ldb_ver_string));    
    push(@ldb_ver_code_heading, sprintf("static const int versionLen = %d;", length($ldb_ver_string)));
    push(@ldb_ver_code_heading, "\n");

    $ldb_ver_file_name = sprintf("l%02d%02ddbver", $secid, $langid);
    $ldb_ver_file_path = "$outpath$ldb_ver_file_name.c";

    unlink ("$ldb_ver_file_path");
    open (DATFILE, ">$ldb_ver_file_path") || die "Unable to create file: $ldb_ver_file_path";

    foreach (@ldb_ver_code_heading) {
        $line = $_;
        $line =~ s/lxxdbver/$ldb_ver_file_name/;
        print DATFILE "$line\n";
    }

    foreach (@ldb_ver_code_function) {
        print DATFILE "$_\n";
    }

    close (DATFILE);

    print "created ldb_ver_file $ldb_ver_file_name for: ". sprintf("l%02d%02d\n", $secid, $langid);

}

sub one_ldb_ver_file {
    

    my ($ldbname, $total);
    ($ldbname) = @_;

    # Read in the .LDB file data
    open (LDBFILE, $ldbname) || die "Unable to open file: $ldbname";
    binmode LDBFILE;
    $total = (stat($ldbname))[7];
    
    $ldbdata = "";
    
    # print "File $ldbname is $total bytes, ";
    if (read(LDBFILE, $ldbdata, $total) != $total) {
        die "Failed to read entire data file: $ldbname";
    }
    close (LDBFILE);

    # Check for Chinese Database
    if (   ((ord(substr($ldbdata, $skip_chinese, 1)) == ($T9CHINESEDATAVALID & 0xff))
        && (ord(substr($ldbdata, $skip_chinese + 1, 1)) == (($T9CHINESEDATAVALID >> 8) & 0xff))) ||
           ((ord(substr($ldbdata, $skip_chinese + 1, 1)) == ($T9CHINESEDATAVALID & 0xff))
        && (ord(substr($ldbdata, $skip_chinese, 1)) == (($T9CHINESEDATAVALID >> 8) & 0xff)))
       )
    {
        #This is a Chinese Database so pass it to the Chinese subroutine.
        one_ldb_ver_file_chinese(\$ldbdata);

    }
    else {
        $dbtype_offset = 0x21;
        if (ord(substr($ldbdata, $dbtype_offset, 1)) == $T9PTIDBTYPE) {
            ($langid) = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET_PTI, 1));
            ($secid)  = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET_PTI + 1, 1));

            $version_offset         = 0x20;
            $version_content_offset = 0x2A;
            $symbol_class_offset    = 0x2F;

            ($bLDBVerLayout) = unpack("C", substr($ldbdata, $version_offset, 1));
            ($bLDBType)    = unpack("C", substr($ldbdata, $version_offset + 1, 1)); 
            ($bContMajVer) = unpack("C", substr($ldbdata, $version_content_offset, 1));
            ($bContMinVer) = unpack("C", substr($ldbdata, $version_content_offset + 1, 1));
            ($bSymClass)   = unpack("C", substr($ldbdata, $symbol_class_offset, 1));
        }
        else {
            ($langid) = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET, 1));
            ($secid)  = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET + 1, 1));

            $version_offset         = 0x20;
            $version_content_offset = 0x32;
            $symbol_class_offset    = 0x39;

            ($bLDBVerLayout) = unpack("C", substr($ldbdata, $version_offset, 1));
            ($bLDBType)    = unpack("C", substr($ldbdata, $version_offset + 1, 1)); 
            ($bContMajVer) = unpack("C", substr($ldbdata, $version_content_offset, 1));
            ($bContMinVer) = unpack("C", substr($ldbdata, $version_content_offset + 1, 1));
            ($bSymClass)   = unpack("C", substr($ldbdata, $symbol_class_offset, 1));
        }

        $ldb_ver_string =
                 sprintf("T9 LDB: v%02d.%02d.%02d.%02d l%02d%02d.%02X", 
                        $bContMajVer,
                        $bContMinVer,
                        $bLDBVerLayout,
                        $bLDBType,
                        $secid,
                        $langid,
                        $bSymClass);

        push(@ldb_ver_code_heading, sprintf("static const char* szLdbVersion = \"%s\";", $ldb_ver_string));    
        push(@ldb_ver_code_heading, sprintf("static const int versionLen = %d;", length($ldb_ver_string)));
        push(@ldb_ver_code_heading, "\n");

        $ldb_ver_file_name = sprintf("l%02d%02ddbver", $secid, $langid);
        $ldb_ver_file_path = "$outpath$ldb_ver_file_name.c";

        unlink ("$ldb_ver_file_path");
        open (DATFILE, ">$ldb_ver_file_path") || die "Unable to create file: $ldb_ver_file_path";

        foreach (@ldb_ver_code_heading) {
            $line = $_;
            $line =~ s/lxxdbver/$ldb_ver_file_name/;
            print DATFILE "$line\n";
        }

        foreach (@ldb_ver_code_function) {
            print DATFILE "$_\n";
        }

        close (DATFILE);

        print "created ldb_ver_file $ldb_ver_file_name for: ". sprintf("l%02d%02d\n", $secid, $langid);

    }
}


#----------------------------------------------------------------------------
# Routine to process one language file
#----------------------------------------------------------------------------
sub onefile {

    my ($ldbname, $datname, $datpath, $total, $done, $block, $line);
    my ($langid, $secid);
    my ($i, $rowlength);
    ($ldbname) = @_;

    # Read in the .LDB file data
    open (LDBFILE, $ldbname) || die "Unable to open file: $ldbname";
    binmode LDBFILE;
    $total = (stat($ldbname))[7];
    $ldbdata = "";
    print "File $ldbname is $total bytes, ";
    if (read(LDBFILE, $ldbdata, $total) != $total) {
        die "Failed to read entire data file: $ldbname";
    }
    close (LDBFILE);

    # Check for Chinese Database
    if (   ((ord(substr($ldbdata, $skip_chinese, 1)) == ($T9CHINESEDATAVALID & 0xff))
        && (ord(substr($ldbdata, $skip_chinese + 1, 1)) == (($T9CHINESEDATAVALID >> 8) & 0xff))) ||
           ((ord(substr($ldbdata, $skip_chinese + 1, 1)) == ($T9CHINESEDATAVALID & 0xff))
        && (ord(substr($ldbdata, $skip_chinese, 1)) == (($T9CHINESEDATAVALID >> 8) & 0xff)))
       )
    {
        #This is a Chinese Database so pass it to the Chinese subroutine.
        chinesemerge(\$ldbdata, $total);

        $chinesedata = 1;
    }
    else
    {
        #Latin or Japanese Database

        # Dump the .LDB file data in blocks to separate C files
        $block = 0;

        # Get language ID and emit CASE statement
        $dbtype_offset = 0x21;
        if (ord(substr($ldbdata, $dbtype_offset, 1)) == $T9PTIDBTYPE) {
            ($langid) = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET_PTI, 1));
            ($secid)  = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET_PTI + 1, 1));
        }
        else {
           ($langid) = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET, 1));
           ($secid) = unpack("C", substr($ldbdata, $LANGUAGEID_OFFSET + 1, 1));
        }
        
        printf ("Language ID = %02d%02d\n", $secid, $langid);
        push (@t9genheader, "");
        if ($addifdefs) {
            push (@t9genheader, "#ifdef T9LANG_$langnames{$langid}");
            push (@t9gencode, "#ifdef  T9LANG_$langnames{$langid}");
        }
        push (@t9gencode, sprintf("    case (%2d * 256) + %2d : /* $langnames{$langid} */", $secid, $langid));

        # And verify no duplicate language ID's used
        if ($langs{$langid + 256 * $secid} eq "") {
            $langs{$langid + 256 * $secid} = $ldbname;
        }
        else {
            die "\nError -- Language ID %secid,$langid used by $langs{$langid + 256 * $secid} and $ldbname\n";
        }


        for ($done = 0; $done < $total; $done += $bytecount) {
            my $thiscount = $bytecount;

            # Compute number of pages to do this time
            if ($done + $bytecount > $total) {
                $thiscount = $total - $done;
            }

            # Create data file for this block
            $datname = sprintf("l%02d%02db%02d", $secid, $langid, $block++);
            $datpath = "$outpath$datname.c";
            unlink ("$datpath");
            open (DATFILE, ">$datpath") || die "Unable to create file: $datpath";

            # Add this include to t9lang.c file
            if ($block == 1) {
                push(@t9langheader, "");
                if ($addifdefs) {
                    if ($langid == 4) {
                        push (@t9langheader, "#ifdef   T9LANG_$langnames{$langid}$secondarynames{$secid}");
                    }
                    else {
                        push (@t9langheader, "#ifdef   T9LANG_$langnames{$langid}");
                    }
                }
                push (@t9langheader, "#include \"$datname\.c\"     /* $langnames{$langid} */");
            }
            else {
                push (@t9langheader, "#include \"$datname\.c\"");
            }

            # Print out the data file header
            foreach (@commonheader) {
                print DATFILE "$_\n";
            }
            foreach (@t9datheader) {
                $line = $_;
                $line =~ s/lxxbxx/$datname/;
                print DATFILE "$line\n";
            }

            # Now print the actual data array
            $rowlength = 16;
            for ($i = $done; $i != $done + $thiscount; $i += $rowlength) {
                if ($i + $rowlength > $done + $thiscount) {
                    $rowlength = $done + $thiscount - $i;
                }
                printf DATFILE ("0x%02X," x $rowlength . "\n", unpack("C$rowlength", substr($ldbdata, $i, $rowlength)));
            }

            print DATFILE "};\n";
            # Done with data file for this block
            close (DATFILE);

            # Now create the code to call this data file
            push (@t9genheader, "extern const T9U8 T9FARDATA ".$datname."[];");

            if ($done == 0) { # first page
                if ($thiscount == $total) { # but the only page
                    push (@t9gencode, sprintf("          if (dwOffset < %6d) {*ppbDst = (T9FARPOINTER) $datname; nSize = %6d;}", $total, $thiscount));
                }
                else {
                    push (@t9gencode, sprintf("             if (dwOffset < %6d) *ppbDst = (T9FARPOINTER) $datname;", $thiscount));
                }
            }
            elsif ($done + $thiscount != $total) { # pages in between
                push (@t9gencode, sprintf("        else if (dwOffset < %6d) *ppbDst = (T9FARPOINTER) $datname;", $done + $thiscount));
            }
            else { # last page
                push (@t9gencode, sprintf("        else if (dwOffset < %6d) {*ppbDst = (T9FARPOINTER) $datname; nSize = %6d;}", $total, $thiscount));
            }
        }

        # Close out the CASE statement for this .ldb file
        push (@t9gencode, "        else                        nSize = 0; /* Invalid offset into database! */");
        push (@t9gencode, "        break;");

        if ($addifdefs) {
            push (@t9langheader, "#endif");
            push (@t9gencode, "#endif");
            push (@t9genheader, "#endif");
        }
    }
}

#----------------------------------------------------------------------------
# Help routine -- print possible error text and program help screen
#----------------------------------------------------------------------------
sub help {

    my ($text) = @_;

    if ($text ne "") {
        print "\n";
        print "Error: $text\n";
    }

    print "\n";
    print "Usage: ".__FILE__." -k <Segment size> [-i] [-o <Output path] <Input Files>\n";
    print "\n";
    print "  Program will split one or more .ldb files into separate .c source\n";
    print "  modules for building directly into the T9 release package.  Also\n";
    print "  created is t9gen.c module containing T9ReadLdbData() routine to\n";
    print "  access the appropriate code module.\n";
    print "\n";
    print "  -h or no arguments prints this text and exits\n";
    print "  -i adds #ifdefs around each language in t9lang.c, t9gen.c, t9cgen.c files\n";
    print "  -k <Segment size> specifies number of 1k data blocks in each source segment\n";
    print "     for latin databases.  \n";
    exit;
}

#----------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------

# If no arguments, just print help text
if (@ARGV == 0) {
    help("");
}

#Set chinese boolean to false
$chinesedata  = 0;

# Process arguments
while (@ARGV != 0) {
    $arg = shift(@ARGV);
    if ($arg !~ /^-/) {
        push (@ldbfiles, $arg);
    }
    else {
        $arg =~ tr/A-Z/a-z/;
        if ($arg eq "-k") {
            $pagecount = shift(@ARGV);
            if ($pagecount < 1 || $pagecount > 1024) {
                help ("Segment size must be from 1 to 1024, not $pagecount");
            }
        }
        elsif ($arg eq "-i") {
            $addifdefs = 1;
        }
        elsif ($arg eq "-o") {
            $outpath = shift (@ARGV);
            if (!(-e $outpath)) {
                help ("Output path $outpath does not exist");
            }
            if (!(-d $outpath)) {
                help ("Output path $outpath is not a directory");
            }

            $outpath .= '/'
                unless $outpath =~ /[\/\\]$/;
        }
        elsif ($arg eq "-ldbver") {
            $gen_ldb_ver_file = 1;
        }
        elsif ($arg eq "-h" || $arg eq "-?") {
            help ("");
        }
        else {
            help ("Option \'$arg\' invalid; Use \'ldbmerge\' for usage text");
        }
    }
}

# Verify user gave us a segment size
if ($pagecount == 0) {
    help("You MUST specify the segment size with the -k option");
}
$bytecount = $pagecount * $pagesize;

# Verify we have at least one input file to process
if (@ldbfiles == 0) {
    help("No input files given");
}

# Put some lines out that need variables.
push (@t9gencode, "    nSize = $bytecount;");
push (@t9gencode, "    *pdwStart = dwOffset - (dwOffset % $bytecount);");
push (@t9gencode, "");
push (@t9gencode, "    switch (pFieldInfo->nLdbNum) {");

# Process each .ldb file
foreach (@ldbfiles) {
    onefile($_);
}

#Initialize name to blank (assume no chinese)
$chininclude = "";

if ($chinesedata)
{
    #If there is chinese then output t9cgen.c

    # Create main Chinese file
    my $chinname = "t9cgen.c";
    my $chinpath = $outpath.$chinname;
    unlink ("$chinpath");

    open (CHINFILE, ">$chinpath") || die "Unable to create file: $chinpath";

    # Print out the gen file data
    foreach (@commonheader) {
        print CHINFILE "$_\n";
    }
    foreach (@chinesemainheader) {
        $line = $_;
        $line =~ s/FILENAME/$chinname/;
        print CHINFILE "$line\n";
    }
    foreach (@chinesemainbody) {
        print CHINFILE "$_\n";
    }
    foreach (@chinesetail) {
        print CHINFILE "$_\n";
    }

    close (CHINFILE);

    $chininclude = "#include \"".$chinname."\"";
}


# Create t9gen.c file
$genname = "${outpath}t9gen.c";
unlink ($genname);
open (GENFILE, ">$genname") || die "Unable to create file: $genname";

# Print out the gen file data
foreach (@commonheader) {
    print GENFILE "$_\n";
}
foreach (@t9genheader) {
    print GENFILE "$_\n";
}
foreach (@t9gencode) {
    print GENFILE "$_\n";
}

foreach (@t9gentail) {
    print GENFILE "$_\n";
}


# Print Chinese LDB loading callback function in the gen file
if ($chinesedata) {
    if ($addifdefs) {
    print GENFILE sprintf("#if (defined(T9LANG_%s%s) || defined(T9LANG_%s%s))\n", $langnames{$CHINESE_PRIMARY_LANG_ID}, $secondarynames{1}, $langnames{$CHINESE_PRIMARY_LANG_ID}, $secondarynames{2});
    }

    foreach (@t9chnloadfunchead) {
        print GENFILE "$_\n";
    }
    foreach (@$t9chnloadbody) {
        print GENFILE "$_\n";
    }
    foreach (@t9chnloadfunctail) {
        print GENFILE "$_\n";
    }
    if ($addifdefs) {
        print GENFILE "#endif\n";
    }
}

foreach (@t9langtail) {
        print GENFILE "$_\n";
}

close (GENFILE);


# Create t9lang.c file
$langname = "${outpath}t9lang.c";
unlink ($langname);
open (LANGFILE, ">$langname") || die "Unable to create file: $langname";

# Print out the gen file data
foreach (@commonheader) {
    print LANGFILE "$_\n";
}
foreach (@t9langheader) {
    $line = $_;
    #Replace with t9cgen.c if Chinese else replace with blank
    $line =~ s/CHINESEGENPLACEHOLDER/$chininclude/;

    # Add compatability ifdefs for Chinese, if appropriate
    if ($line eq "CHINESEIFDEFSPLACEHOLDER") {
        if ($chinesedata) {
            if ($addifdefs) {
                print LANGFILE "#if (defined(T9LANG_Chinese) && !defined(T9LANG_ChineseTraditional) && !defined(T9LANG_ChineseSimplified))\n";
                print LANGFILE "#define T9LANG_ChineseTraditional\n";
                print LANGFILE "#define T9LANG_ChineseSimplified\n";
                print LANGFILE "#endif\n";
                print LANGFILE "\n";
            }
        }
    }
    else {
        print LANGFILE "$line\n";
    }
}
foreach (@t9langtail) {
    print LANGFILE "$_\n";
}
close (LANGFILE);

# create ldb version file for each ldb
if ($gen_ldb_ver_file) {
    foreach (@ldbfiles) {
        one_ldb_ver_file($_);
    }
}
