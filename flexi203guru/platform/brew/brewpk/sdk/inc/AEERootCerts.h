/*======================================================================  
    Brew-related root certs. NOT for use with SSL
   
    Author: Laurence Lundblade <lgl@qualcomm.com>
    File: AEERootCerts.h
    Started: June 2001
     
    Copyright (c) 2001 - 2003 QUALCOMM Incorporated USA    
    
  ====================================================================== */


#define AEECLSID_BREW_ROOTCERT          0x01015c2a
#define AEECLSID_TS_ROOTCERT            0x01015c2b




/***********************************************************************

  These two classes have an IWebOpts interface. That is you'll get an 
  IWebOpts instance back when you do a create instance with these class
  IDs

  The IWebOpts instances are read-only singletons. IWEBOPTS_Add and
  IWEBOPTS_Delete will fail. Increments and decrements to the reference 
  count does nothing.

  These interface can be passed in to IX509Chain directly to represent
  the root cert sets. It is usually passed as an option of type 
  WEBOPT_DEFAULTS.

  AEECLSID_BREW_ROOTCERT returns the Brew root certs. These are the 
  certs for app signing, ADS authentication and other.

  AEECLSID_TS_ROOTCERT returns the time stamping / counter signing 
  certs used with Brew app signing. 

  See AEECLSID_SSLROOTCERTS for roots to use with SSL.

  */
