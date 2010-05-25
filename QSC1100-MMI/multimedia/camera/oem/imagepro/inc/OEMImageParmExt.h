#ifndef OEMIMAGEPARMEXT_H
#define OEMIMAGEPARMEXT_H

#define AEECLSID_EXIF_GET_INFO         0x01032603
#define AEECLSID_EXIF_GET_VALUE        0x01032604
#define AEECLSID_EXIF_SET_ASCII_VALUE  0x01032605
#define AEECLSID_EXTRACT_THUMBNAIL     0x01032606

typedef struct
{
  uint32  uID;   // used for exif tag id
  void   *pData;
  uint32  uLen;
} OEMImageParmExt;

#endif // OEMIMAGEPARMEXT_H
