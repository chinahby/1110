#ifndef AEELNGCODE_H
#define AEELNGCODE_H
/*======================================================
FILE:  AEELngCode.h

SERVICES: Language Code Definitions

GENERAL DESCRIPTION:
   ISO 639-1 and ISO 3166 A2 definitions for BREW

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*=============================================================================
   DATA STRUCTURES DOCUMENTATION
===============================================================================
AEE Language Type

Description:
    BREW language type returned by ISHELL_GetDeviceInfo() is a uint32 variable
    defined in AEELngCode.h. This type of variables comprises two components:

    - ISO 639-1 language code (2 characters)
    - ISO 3166 A2 country code (2 characters)

    The country code is optional. In the case this optional code is not present, two
    spaces are inserted in its place.

    For example:
    - Korean (LNG_KOREAN) is defined as "ko  " (0x20206f6b) without the optional
      country code ("ko" for language code and two spaces for country code).
    - Portuguese used in Brazil (LNG_PORTUGUESE_BRAZIL) is defined as "ptbr"
      (0x72627470) with the country code ("pt" for language code and "br" for country
      code).
    

Definition:
Language code starting with 'A'
  LNG_AFAR              0x20206161  // 'aa  '
  LNG_ABKHAZIAN         0x20206261  // 'ab  '
  LNG_AFRIKAANS         0x20206661  // 'af  '
  LNG_AMHARIC           0x20206d61  // 'am  '
  LNG_ARABIC            0x20207261  // 'ar  '
  LNG_ASSAMESE          0x20207361  // 'as  '
  LNG_AYMARA            0x20207961  // 'ay  '
  LNG_AZERBAIJANI       0x20207a61  // 'az  '

Language code starting with 'B'
  LNG_BASHKIR           0x20206162  // 'ba  '
  LNG_BYELORUSSIAN      0x20206562  // 'be  '
  LNG_BULGARIAN         0x20206762  // 'bg  '
  LNG_BIHARI            0x20206862  // 'bh  '
  LNG_BISLAMA           0x20206962  // 'bi  '
  LNG_BENGALI           0x20206e62  // 'bn  '
  LNG_BANGLA            LNG_BENGALI // 'bn  '
  LNG_TIBETAN           0x20206f62  // 'bo  '
  LNG_BRETON            0x20207262  // 'br  '

Language code starting with 'C'
  LNG_CATALAN           0x20206163  // 'ca  '
  LNG_CORSICAN          0x20206f63  // 'co  '
  LNG_CZECH             0x20207363  // 'cs  '
  LNG_WELSH             0x20207963  // 'cy  '

Language code starting with 'D'
  LNG_DANISH            0x20206164  // 'da  '
  LNG_GERMAN            0x20206564  // 'de  '
  LNG_BHUTANI           0x20207a64  // 'dz  '

Language code starting with 'E'
  LNG_GREEK             0x20206c65  // 'el  '
  LNG_ENGLISH           0x20206e65  // 'en  '
  LNG_ESPERANTO         0x20206f65  // 'eo  '
  LNG_SPANISH           0x20207365  // 'es  '
  LNG_SPANISH_MEXICO    0x786d7365  // 'esmx'
  LNG_ESTONIAN          0x20207465  // 'et  '
  LNG_BASQUE            0x20207565  // 'eu  '

Language code starting with 'F'
  LNG_PERSIAN           0x20206166  // 'fa  '
  LNG_FINNISH           0x20206966  // 'fi  '
  LNG_FIJI              0x20206a66  // 'fj  '
  LNG_FAROESE           0x20206f66  // 'fo  '
  LNG_FRENCH            0x20207266  // 'fr  '
  LNG_FRISIAN           0x20207966  // 'fy  '

Language code starting with 'G'
  LNG_IRISH             0x20206167  // 'ga  '
  LNG_SCOTS_GAELIC      0x20206467  // 'gd  '
  LNG_GALICIAN          0x20206c67  // 'gl  '
  LNG_GUARANI           0x20206e67  // 'gn  '
  LNG_GUJARATI          0x20207567  // 'gu  '

Language code starting with 'H'
  LNG_HAUSA             0x20206168  // 'ha  '
  LNG_HEBREW            0x20206568  // 'he  '
  LNG_HEBREW_OLD        0x20207769  // 'iw  '  Old ISO 639-1 code for 'he  '
  LNG_HINDI             0x20206968  // 'hi  '
  LNG_CROATIAN          0x20207268  // 'hr  '
  LNG_HUNGARIAN         0x20207568  // 'hu  '
  LNG_ARMENIAN          0x20207968  // 'hy  '

Language code starting with 'I'
  LNG_INTERLINGUA       0x20206169  // 'ia  '
  LNG_INDONESIAN        0x20206469  // 'id  '
  LNG_INDONESIAN_OLD    0x20206e69  // 'in  '  Old ISO 639-1 code for 'id  '
  LNG_INTERLINGUE       0x20206569  // 'ie  '
  LNG_INUPIAK           0x20206b69  // 'ik  '
  LNG_ICELANDIC         0x20207369  // 'is  '
  LNG_ITALIAN           0x20207469  // 'it  '
  LNG_INUKTITUT         0x20207569  // 'iu  '

Language code starting with 'J'
  LNG_JAPANESE          0x2020616a  // 'ja  '
  LNG_JAVANESE          0x2020776a  // 'jw  '

Language code starting with 'K'
  LNG_GEORGIAN          0x2020616b  // 'ka  '
  LNG_Kazakh            0x20206b6b  // 'kk  '
  LNG_GREENLANDIC       0x20206c6b  // 'kl  '
  LNG_CAMBODIAN         0x20206d6b  // 'km  '
  LNG_KANNADA           0x20206e6b  // 'kn  '
  LNG_KOREAN            0x20206f6b  // 'ko  '
  LNG_KASHMIRI          0x2020736b  // 'ks  '
  LNG_KURDISH           0x2020756b  // 'ku  '
  LNG_KIRGHIZ           0x2020796b  // 'ky  '

Language code starting with 'L'
  LNG_LATIN             0x2020616c  // 'la  '
  LNG_LINGALA           0x20206e6c  // 'ln  '
  LNG_LAOTHIAN          0x20206f6c  // 'lo  '
  LNG_LITHUANIAN        0x2020746c  // 'lt  '
  LNG_LATVIAN           0x2020766c  // 'lv  '
  LNG_LETTISH           LNG_LATVIAN

Language code starting with 'M'
  LNG_MALAGASY          0x2020676d  // 'mg  '
  LNG_MAORI             0x2020696d  // 'mi  '
  LNG_MACEDONIAN        0x20206b6d  // 'mk  '
  LNG_MALAYALAM         0x20206c6d  // 'ml  '
  LNG_MONGOLIAN         0x20206e6d  // 'mn  '
  LNG_MOLDAVIAN         0x20206f6d  // 'mo  '
  LNG_MARATHI           0x2020726d  // 'mr  '
  LNG_MALAY             0x2020736d  // 'ms  '
  LNG_MALTESE           0x2020746d  // 'mt  '
  LNG_BURMESE           0x2020796d  // 'my  '

Language code starting with 'N'
  LNG_NAURU             0x2020616e  // 'na  '
  LNG_NEPALI            0x2020656e  // 'ne  '
  LNG_DUTCH             0x20206c6e  // 'nl  '
  LNG_NORWEGIAN         0x20206f6e  // 'no  '

Language code starting with 'O'
  LNG_OCCITAN           0x2020636f  // 'oc  '
  LNG_OROMO_AFAN        0x20206d6f  // 'om  '
  LNG_ORIYA             0x2020726f  // 'or  '

Language code starting with 'P'
  LNG_PUNJABI           0x20206170  // 'pa  '
  LNG_POLISH            0x20206c70  // 'pl  '
  LNG_PASHTO            0x20207370  // 'ps  '
  LNG_PUSHTO            LNG_PASHTO 
  LNG_PORTUGUESE        0x20207470  // 'pt  '
  LNG_PORTUGUESE_BRAZIL 0x72627470  // 'ptbr'  Portuguese used in Brazil

Language code starting with 'Q'
  LNG_QUECHUA           0x20207571  // 'qu  '

Language code starting with 'R'
  LNG_RHAETO_ROMANCE    0x20206d72  // 'rm  '
  LNG_KIRUNDI           0x20206e72  // 'rn  '
  LNG_ROMANIAN          0x20206f72  // 'ro  '
  LNG_RUSSIAN           0x20207572  // 'ru  '
  LNG_KINYARWANDA       0x20207772  // 'rw  '

Language code starting with 'S'
  LNG_SANSKRIT          0x20206173  // 'sa  '
  LNG_SINDHI            0x20206473  // 'sd  '
  LNG_SANGHO            0x20206773  // 'sg  '
  LNG_SERBO_CROATIAN    0x20206873  // 'sh  '
  LNG_SINHALESE         0x20206973  // 'si  '
  LNG_SLOVAK            0x20206b73  // 'sk  '
  LNG_SLOVENIAN         0x20206c73  // 'sl  '
  LNG_SAMOAN            0x20206d73  // 'sm  '
  LNG_SHONA             0x20206e73  // 'sn  '
  LNG_SOMALI            0x20206f73  // 'so  '
  LNG_ALBANIAN          0x20207173  // 'sq  '
  LNG_SERBIAN           0x20207273  // 'sr  '
  LNG_SISWATI           0x20207373  // 'ss  '  
  LNG_SESOTHO           0x20207473  // 'st  '
  LNG_SUNDANESE         0x20207573  // 'su  '
  LNG_SWEDISH           0x20207673  // 'sv  '
  LNG_SWAHILI           0x20207773  // 'sw  '

Language code starting with 'T'
  LNG_TAMIL             0x20206174  // 'ta  '
  LNG_TELUGU            0x20206574  // 'te  '
  LNG_TAJIK             0x20206774  // 'tg  '
  LNG_THAI              0x20206874  // 'th  '
  LNG_TIGRINYA          0x20206974  // 'ti  '
  LNG_TURKMEN           0x20206B74  // 'tk  '
  LNG_TAGALOG           0x20206c74  // 'tl  '
  LNG_SETSWANA          0x20206e74  // 'tn  '
  LNG_TONGA             0x20206f74  // 'to  '
  LNG_TURKISH           0x20207274  // 'tr  '
  LNG_TSONGA            0x20207374  // 'ts  '
  LNG_TATAR             0x20207474  // 'tt  '
  LNG_TWI               0x20207774  // 'tw  '

Language code starting with 'U'
  LNG_UIGHUR            0x20206775  // 'ug  '
  LNG_UKRAINIAN         0x20206b75  // 'uk  '
  LNG_URDU              0x20207275  // 'ur  '
  LNG_UZBEK             0x20207a75  // 'uz  '

Language code starting with 'V'
  LNG_VIETNAMESE        0x20206976  // 'vi  '
  LNG_VOLAPUK           0x20206f76  // 'vo  '

Language code starting with 'W'
  LNG_WOLOF             0x20206f77  // 'wo  '

Language code starting with 'X'
  LNG_XHOSA             0x20206878  // 'xh  '

Language code starting with 'Y'
  LNG_YIDDISH           0x20206979  // 'yi  '
  LNG_YIDDISH_OLD       0x2020696a  // 'ji  '  Old ISO 639-1 code for 'yi  '
  LNG_YORUBA            0x20206f79  // 'yo  '

Language code starting with 'Z'
  LNG_ZHUANG            0x2020617a  // 'za  '
  LNG_CHINESE           0x2020687a  // 'zh  '  
  LNG_SCHINESE          0x6e63687a  // 'zhcn'  Simplified Chinese used in China
  LNG_TCHINESE          0x7774687a  // 'zhtw'  Traditional Chinese used in Taiwan
  LNG_ZULU              0x2020757a  // 'zu  '


Members:
    None

Comments:
    None

See Also:
    ISHELL_GetDeviceInfo()
    message options 
=============================================================================*/


// LNG code starting with 'A'
#define LNG_AFAR              0x20206161  // 'aa  '
#define LNG_ABKHAZIAN         0x20206261  // 'ab  '
#define LNG_AFRIKAANS         0x20206661  // 'af  '
#define LNG_AMHARIC           0x20206d61  // 'am  '
#define LNG_ARABIC            0x20207261  // 'ar  '
#define LNG_ASSAMESE          0x20207361  // 'as  '
#define LNG_AYMARA            0x20207961  // 'ay  '
#define LNG_AZERBAIJANI       0x20207a61  // 'az  '

// LNG code starting with 'B'
#define LNG_BASHKIR           0x20206162  // 'ba  '
#define LNG_BYELORUSSIAN      0x20206562  // 'be  '
#define LNG_BULGARIAN         0x20206762  // 'bg  '
#define LNG_BIHARI            0x20206862  // 'bh  '
#define LNG_BISLAMA           0x20206962  // 'bi  '
#define LNG_BENGALI           0x20206e62  // 'bn  '
#define LNG_BANGLA            LNG_BENGALI // 'bn  '
#define LNG_TIBETAN           0x20206f62  // 'bo  '
#define LNG_BRETON            0x20207262  // 'br  '

// LNG code starting with 'C'
#define LNG_CATALAN           0x20206163  // 'ca  '
#define LNG_CORSICAN          0x20206f63  // 'co  '
#define LNG_CZECH             0x20207363  // 'cs  '
#define LNG_WELSH             0x20207963  // 'cy  '

// LNG code starting with 'D'
#define LNG_DANISH            0x20206164  // 'da  '
#define LNG_GERMAN            0x20206564  // 'de  '
#define LNG_BHUTANI           0x20207a64  // 'dz  '

// LNG code starting with 'E'
#define LNG_GREEK             0x20206c65  // 'el  '
#define LNG_ENGLISH           0x20206e65  // 'en  '
#define LNG_ESPERANTO         0x20206f65  // 'eo  '
#define LNG_SPANISH           0x20207365  // 'es  '
#define LNG_SPANISH_MEXICO    0x786d7365  // 'esmx'
#define LNG_ESTONIAN          0x20207465  // 'et  '
#define LNG_BASQUE            0x20207565  // 'eu  '

// LNG code starting with 'F'
#define LNG_PERSIAN           0x20206166  // 'fa  '
#define LNG_FINNISH           0x20206966  // 'fi  '
#define LNG_FIJI              0x20206a66  // 'fj  '
#define LNG_FAROESE           0x20206f66  // 'fo  '
#define LNG_FRENCH            0x20207266  // 'fr  '
#define LNG_FRISIAN           0x20207966  // 'fy  '

// LNG code starting with 'G'
#define LNG_IRISH             0x20206167  // 'ga  '
#define LNG_SCOTS_GAELIC      0x20206467  // 'gd  '
#define LNG_GALICIAN          0x20206c67  // 'gl  '
#define LNG_GUARANI           0x20206e67  // 'gn  '
#define LNG_GUJARATI          0x20207567  // 'gu  '

// LNG code starting with 'H'
#define LNG_HAUSA             0x20206168  // 'ha  '
#define LNG_HEBREW            0x20206568  // 'he  '
#define LNG_HEBREW_OLD        0x20207769  // 'iw  '  Old ISO 639-1 code for 'he  '
#define LNG_HINDI             0x20206968  // 'hi  '
#define LNG_CROATIAN          0x20207268  // 'hr  '
#define LNG_HUNGARIAN         0x20207568  // 'hu  '
#define LNG_ARMENIAN          0x20207968  // 'hy  '

// LNG code starting with 'I'
#define LNG_INTERLINGUA       0x20206169  // 'ia  '
#define LNG_INDONESIAN        0x20206469  // 'id  '
#define LNG_INDONESIAN_OLD    0x20206e69  // 'in  '  Old ISO 639-1 code for 'id  '
#define LNG_INTERLINGUE       0x20206569  // 'ie  '
#define LNG_INUPIAK           0x20206b69  // 'ik  '
#define LNG_ICELANDIC         0x20207369  // 'is  '
#define LNG_ITALIAN           0x20207469  // 'it  '
#define LNG_INUKTITUT         0x20207569  // 'iu  '

// LNG code starting with 'J'
#define LNG_JAPANESE          0x2020616a  // 'ja  '
#define LNG_JAVANESE          0x2020776a  // 'jw  '

// LNG code starting with 'K'
#define LNG_GEORGIAN          0x2020616b  // 'ka  '
#define LNG_Kazakh            0x20206b6b  // 'kk  '
#define LNG_GREENLANDIC       0x20206c6b  // 'kl  '
#define LNG_CAMBODIAN         0x20206d6b  // 'km  '
#define LNG_KANNADA           0x20206e6b  // 'kn  '
#define LNG_KOREAN            0x20206f6b  // 'ko  '
#define LNG_KASHMIRI          0x2020736b  // 'ks  '
#define LNG_KURDISH           0x2020756b  // 'ku  '
#define LNG_KIRGHIZ           0x2020796b  // 'ky  '

// LNG code starting with 'L'
#define LNG_LATIN             0x2020616c  // 'la  '
#define LNG_LINGALA           0x20206e6c  // 'ln  '
#define LNG_LAOTHIAN          0x20206f6c  // 'lo  '
#define LNG_LITHUANIAN        0x2020746c  // 'lt  '
#define LNG_LATVIAN           0x2020766c  // 'lv  '
#define LNG_LETTISH           LNG_LATVIAN

// LNG code starting with 'M'
#define LNG_MALAGASY          0x2020676d  // 'mg  '
#define LNG_MAORI             0x2020696d  // 'mi  '
#define LNG_MACEDONIAN        0x20206b6d  // 'mk  '
#define LNG_MALAYALAM         0x20206c6d  // 'ml  '
#define LNG_MONGOLIAN         0x20206e6d  // 'mn  '
#define LNG_MOLDAVIAN         0x20206f6d  // 'mo  '
#define LNG_MARATHI           0x2020726d  // 'mr  '
#define LNG_MALAY             0x2020736d  // 'ms  '
#define LNG_MALTESE           0x2020746d  // 'mt  '
#define LNG_BURMESE           0x2020796d  // 'my  '
#define LNG_MYANMAR           0x2061796d  // 'mya '   //add by yangdecai 20101224

// LNG code starting with 'N'
#define LNG_NAURU             0x2020616e  // 'na  '
#define LNG_NEPALI            0x2020656e  // 'ne  '
#define LNG_DUTCH             0x20206c6e  // 'nl  '
#define LNG_NORWEGIAN         0x20206f6e  // 'no  '

// LNG code starting with 'O'
#define LNG_OCCITAN           0x2020636f  // 'oc  '
#define LNG_OROMO_AFAN        0x20206d6f  // 'om  '
#define LNG_ORIYA             0x2020726f  // 'or  '

// LNG code starting with 'P'
#define LNG_PUNJABI           0x20206170  // 'pa  '
#define LNG_POLISH            0x20206c70  // 'pl  '
#define LNG_PASHTO            0x20207370  // 'ps  '
#define LNG_PUSHTO            LNG_PASHTO 
#define LNG_PORTUGUESE        0x20207470  // 'pt  '
#define LNG_PORTUGUESE_BRAZIL 0x72627470  // 'ptbr'  Portuguese used in Brazil

// LNG code starting with 'Q'
#define LNG_QUECHUA           0x20207571  // 'qu  '

// LNG code starting with 'R'
#define LNG_RHAETO_ROMANCE    0x20206d72  // 'rm  '
#define LNG_KIRUNDI           0x20206e72  // 'rn  '
#define LNG_ROMANIAN          0x20206f72  // 'ro  '
#define LNG_RUSSIAN           0x20207572  // 'ru  '
#define LNG_KINYARWANDA       0x20207772  // 'rw  '

// LNG code starting with 'S'
#define LNG_SANSKRIT          0x20206173  // 'sa  '
#define LNG_SINDHI            0x20206473  // 'sd  '
#define LNG_SANGHO            0x20206773  // 'sg  '
#define LNG_SERBO_CROATIAN    0x20206873  // 'sh  '
#define LNG_SINHALESE         0x20206973  // 'si  '
#define LNG_SLOVAK            0x20206b73  // 'sk  '
#define LNG_SLOVENIAN         0x20206c73  // 'sl  '
#define LNG_SAMOAN            0x20206d73  // 'sm  '
#define LNG_SHONA             0x20206e73  // 'sn  '
#define LNG_SOMALI            0x20206f73  // 'so  '
#define LNG_ALBANIAN          0x20207173  // 'sq  '
#define LNG_SERBIAN           0x20207273  // 'sr  '
#define LNG_SISWATI           0x20207373  // 'ss  '  
#define LNG_SESOTHO           0x20207473  // 'st  '
#define LNG_SUNDANESE         0x20207573  // 'su  '
#define LNG_SWEDISH           0x20207673  // 'sv  '
#define LNG_SWAHILI           0x20207773  // 'sw  '

// LNG code starting with 'T'
#define LNG_TAMIL             0x20206174  // 'ta  '
#define LNG_TELUGU            0x20206574  // 'te  '
#define LNG_TAJIK             0x20206774  // 'tg  '
#define LNG_THAI              0x20206874  // 'th  '
#define LNG_TIGRINYA          0x20206974  // 'ti  '
#define LNG_TURKMEN           0x20206B74  // 'tk  '
#define LNG_TAGALOG           0x20206c74  // 'tl  '
#define LNG_SETSWANA          0x20206e74  // 'tn  '
#define LNG_TONGA             0x20206f74  // 'to  '
#define LNG_TURKISH           0x20207274  // 'tr  '
#define LNG_TSONGA            0x20207374  // 'ts  '
#define LNG_TATAR             0x20207474  // 'tt  '
#define LNG_TWI               0x20207774  // 'tw  '

// LNG code starting with 'U'
#define LNG_UIGHUR            0x20206775  // 'ug  '
#define LNG_UKRAINIAN         0x20206b75  // 'uk  '
#define LNG_URDU              0x20207275  // 'ur  '
#define LNG_UZBEK             0x20207a75  // 'uz  '

// LNG code starting with 'V'
#define LNG_VIETNAMESE        0x20206976  // 'vi  '
#define LNG_VOLAPUK           0x20206f76  // 'vo  '

// LNG code starting with 'W'
#define LNG_WOLOF             0x20206f77  // 'wo  '

// LNG code starting with 'X'
#define LNG_XHOSA             0x20206878  // 'xh  '

// LNG code starting with 'Y'
#define LNG_YIDDISH           0x20206979  // 'yi  '
#define LNG_YIDDISH_OLD       0x2020696a  // 'ji  '  Old ISO 639-1 code for 'yi  '
#define LNG_YORUBA            0x20206f79  // 'yo  '

// LNG code starting with 'Z'
#define LNG_ZHUANG            0x2020617a  // 'za  '
#define LNG_CHINESE           0x2020687a  // 'zh  '  
#define LNG_SCHINESE          0x6e63687a  // 'zhcn'  Simplified Chinese used in China
#define LNG_TCHINESE          0x7774687a  // 'zhtw'  Traditional Chinese used in Taiwan
#define LNG_ZULU              0x2020757a  // 'zu  '

#endif // AEELNGCODE_H

