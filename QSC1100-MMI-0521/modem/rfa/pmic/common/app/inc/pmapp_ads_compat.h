#ifndef PMAPP_ADS_COMPAT_H
#define PMAPP_ADS_COMPAT_H

/* ADS 1.2 C++ compiler does not support the C++ cast operators.
   To eliminate warnings, change the casts to a special template
   function custom_cast that implements a C-style cast. */
#ifdef __cplusplus
#if (__ARMCC_VERSION/10000 <= 12)
#define static_cast custom_cast
#define dynamic_cast custom_cast
#define reinterpret_cast custom_cast
template <typename T, typename X> T custom_cast(const X& data) { return (T)data; }
#endif /* (__ARMCC_VERSION <= 120000) */
#endif

#endif /* ADS_COMPAT_H */


