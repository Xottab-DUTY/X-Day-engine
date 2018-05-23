#pragma once

/*!
*   \typedef using cbool = bool const;
*   \brief constant boolean
*   
*   \typedef using pstr = char*
*   \brief pointer to string
*   
*   \typedef using cpstr = char* const;
*   \brief const pointer to string
*   
*   \typedef using pcstr = const char*;
*   \brief pointer to const string
*   
*   \typedef using cpcstr = const char* const;
*   \brief const pointer to const string
*   
*   \typedef using pstr = char*
*   \brief pointer to string
*/

using cbool = bool const;

using pstr = char*;
using cpstr = char* const;
using pcstr = const char*;
using cpcstr = const char* const;

using string8 = char[16];
using string16 = char[16];
using string32 = char[32];
using string64 = char[64];
using string128 = char[128];
using string256 = char[256];
using string512 = char[512];
using string1024 = char[1024];
using string2048 = char[2048];
using string4096 = char[4096];
using string8192 = char[8192];
