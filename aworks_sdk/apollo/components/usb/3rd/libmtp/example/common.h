//#include <libmtp.h>
#include "libmtp.h"
#ifndef _MSC_VER
//#include <config.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#include <unistd.h>
#endif
#else
// Only if using MSVC...
#include "..\windows\getopt.h"
#endif
