This file contains descriptions of the files included in your T9 delivery.

Copyright 2007 Tegic Communications

TEGIC COMMUNICATIONS PROPRIETARY INFORMATION

This software is supplied under the terms of a license agreement or non-disclosure agreement
with Tegic Communications and may not be copied or disclosed except in accordance with the
terms of that agreement.



Windows compiler used: VC6
Windows compiler options: 
	/nologo /MD /W3 /GX /Od /GZ /c 



Target compiler used: ADS1.2
Target compiler options:
	-c  -j-  -cpu ARM926EJ-S  -apcs  /noswst/interwork -littleend -zo -fd -fa -Wb -Ospace -O2  


The objects included in this build are:
T9_v730_VC6_uni_Alpha.obj		VC6, Alphabetic
T9_v730_VC6_uni_Chin.obj		VC6, Chinese Only
T9_v730_VC6_uni_AlCh.obj		VC6, Alphabetic and Chinese
T9_v730_ADS1.2_uni_Alpha.o		ADS1.2, Alphabetic Only
T9_v730_ADS1.2_uni_Chin.o		ADS1.2, Chinese Only
T9_v730_ADS1.2_uni_AlCh.o		ADS1.2, Alphabetic and Chinese

T9_Core_730_VC__t9rel_A_2.obj   VC6, Alphabetic Support Hebrew and Arabic           
T9_Core_730_VC__t9rel_AC_2.obj  VC6, Chinese and Alphabetic Support Hebrew and Arabic          
T9_Core_730_VC__t9rel_C_2.obj   VC6, Chinese Only
T9_Core_730_ADS__t9rel_A_1.o    ADS1.2, Alphabetic Support Hebrew and Arabic
T9_Core_730_ADS__t9rel_AC_1.o   ADS1.2, Chinese and Alphabetic Support Hebrew and Arabic 
T9_Core_730_ADS__t9rel_C_1.o    ADS1.2, Chinese Only

Following is a list of LDBs included in this delivery with descriptions of each file.

Langauge         Filename                                      Encoding    Char Set  Comments
---------------- --------------------------------------------- ----------- --------- --------------------
English          ENlbUN.ldb                                    UNICODE     base       
Arabic           ARlsUN.ldb                                    UNICODE     standard   Standard Layout
French           FRlsUN.ldb                                    UNICODE     standard   
Hindi            HIlsUNAlternate.ldb                           UNICODE     standard   Alternate Layout
Indonesian       IDlbUN.ldb                                    UNICODE     base       
Italian          ITlsUN.ldb                                    UNICODE     standard   
Malay            MSlbUN.ldb                                    UNICODE     base       
Portuguese       PTlsUN.ldb                                    UNICODE     standard   
Romanian         ROlsUN.ldb                                    UNICODE     standard   
Russian          RUlsUN.ldb                                    UNICODE     standard   Standard Layout
Spanish          ESlsUN.ldb                                    UNICODE     standard   
Thai             THlsUN.ldb                                    UNICODE     standard   Standard Layout
Thai             THlsUNHutchison.ldb                           UNICODE     standard   Alternate Layout for Hutchison
Vietnamese       VIlsUN.ldb                                    UNICODE     standard
Hebrew           HElsUN.ldb                                    UNICODE     standard
Chinese          ENlbUNZH.ldb                                  UNICODE     base       English for Chinese phones (Pinyin syllables added)
Chinese          B5L1_Trad_Unicode_0601_SP40K.bin              UNICODE     --         Reduced Traditional
Chinese          B5L1_Trad_Unicode_0601_SPC20K.bin             UNICODE     --         Reduced Traditional
Chinese          B5L1_Trad_Unicode_0601_SPC40K.bin             UNICODE     --         Reduced Traditional
Chinese          Big5_Trad_Unicode_0601_SP20K.bin              UNICODE     --         Full Traditional
Chinese          Big5_Trad_Unicode_0601_SP40K.bin              UNICODE     --         Full Traditional
Chinese          Big5_Trad_Unicode_0601_SPC20K.bin             UNICODE     --         Full Traditional
Chinese          Big5_Trad_Unicode_0601_SPC40K.bin             UNICODE     --         Full Traditional
Chinese          GB2312_Simp_Unicode_0601_SP20K.bin            UNICODE     --         Simplified Chinese: GB2312
Chinese          GB2312_Simp_Unicode_0601_SP40K.bin            UNICODE     --         Simplified Chinese: GB2312
Chinese          GB2312_Simp_Unicode_0601_SPC20K.bin           UNICODE     --         Simplified Chinese: GB2312
Chinese          GB2312_Simp_Unicode_0601_SPC40K.bin           UNICODE     --         Simplified Chinese: GB2312


 - ldbmerge.pl - perl script used to convert LDB files from .ldb format to .c format

 The recommended command line to use with this script is as follows:
   ldbmerge.pl -k 15 [-o <Output path>] <Input Files>

