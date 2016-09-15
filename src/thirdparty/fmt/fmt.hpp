#ifndef FMT_TPL_H_
#define FMT_TPL_H_

// Note (KW 5/2016): 
//      This file customizes the format library with some compiler defines 
//      for use with the ASC toolkit project.
//
//      Include this file in ASC toolkit code rather than 
//      the original format.h file in the fmt/ subdirectory


// Don't use exceptions
#undef MUST_UNDEF_FMT_EXCEPTIONS
#ifndef FMT_EXCEPTIONS
  #define FMT_EXCEPTIONS 0
  #define MUST_UNDEF_FMT_EXCEPTIONS
#endif

// Use the library as header-only
#undef MUST_UNDEF_FMT_HEADER_ONLY
#ifndef FMT_HEADER_ONLY
  #define FMT_HEADER_ONLY 1
  #define MUST_UNDEF_FMT_HEADER_ONLY
#endif

// Clang has a linker error with fmt's extern templates
#undef MUST_UNDEF_FMT_EXTERN_TEMPLATES
#if !defined(FMT_USE_EXTERN_TEMPLATES) && defined(__clang__)
  #define FMT_USE_EXTERN_TEMPLATES 0
  #define MUST_UNDEF_FMT_EXTERN_TEMPLATES
#endif



#include "fmt/format.h"
#include "fmt/ostream.h"


// Undef the defines above
#ifdef MUST_UNDEF_FMT_EXCEPTIONS
  #undef MUST_UNDEF_FMT_EXCEPTIONS
  #undef FMT_EXCEPTIONS
#endif

#ifdef FMT_HEADER_ONLY
  #undef FMT_HEADER_ONLY
  #undef MUST_UNDEF_FMT_HEADER_ONLY
#endif

#ifdef MUST_UNDEF_FMT_EXTERN_TEMPLATES
  #undef FMT_USE_EXTERN_TEMPLATES
  #undef MUST_UNDEF_FMT_EXTERN_TEMPLATES
#endif


#endif  // FMT_TPL_H_
