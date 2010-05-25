#ifndef PLATFORM_LTK

#define EXTERN_RANGE(name)                       \
   extern unsigned long Image__##name##__Base;   \
   extern unsigned long Image__##name##__Length

EXTERN_RANGE(BOOT);
EXTERN_RANGE(APP_RAM);

#define BASE(name) Image__##name##__Base
#define LENGTH(name) Image__##name##__Length

#define BEFORE(address, name) (address < BASE(name))
#define AFTER(address, name)  (address > (BASE(name) + LENGTH(name)))

int HT_SYSTEM_CHECK_DATA_SECTION_POINTER(unsigned long address)
{
	return 1;
}

int HT_SYSTEM_CHECK_TEXT_SECTION_POINTER(unsigned long address)
{
  return address >= 0x20 && BEFORE(address, APP_RAM);
}
#endif//PLATFORM_LTK
