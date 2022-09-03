#ifndef __lib_aveth_utils_hpp
#define __lib_aveth_utils_hpp
//  ALIB: A C-Style programming utility header for tedious tasks.
//
//  Any instance of array creation or object creation is handled using the C-style malloc()/free() functions!
//
//



/** - https://github.com/avetharun/avetharun/blob/main/ave_libs.hpp
 @copyright Avetharun 2021-2022 - GNU-GPL3-a
 Let's keep this simple. It follows the GNU GPL3 license, with very few modifications and exceptions.
 @ You can:
  - Use this commercially
  - Distribute modified versions
  - Modify this file and/or projects
  - Patent projects that use this file
  - Use this file and/or project privately, ie. for use in an internal server (See limitations & Exceptions)
  - Use freely under the following conditions
 @ Conditions:
  - You must disclose the source file and/or author when distributing this file
  - A copy of this license must be attached to the file, in verbatim
  - The same (or similar) license must be used if you modify and license your version
  - Changes must be listed. ie. modifying source code must have your name and change in the file.
    - To follow this it must either be
      A: Beside the change (on top or on bottom, in a comment)
      B: In the AUTHORs section of the file/project
      C: in any changelog that references the file's changes
 @ Limitations / What_you_cannot_do
  - The user (you) are not liable for the harmful actions this program or file may cause, if any.
  - Keep code closed source (*See exceptions)
 @b Exceptions
  - If code is closed source, it must be in a private setting. Examples are as follows:
    EXA: A server used to host and/or distribute files
    EXB: Used as a base for porting and/or in microcontrollers
 */


 // Avetharun : 4-14-22 : added byte modification utilities
 // Avetharun : 4-15-22 : added beginswith function, and renamed readFileBytes to alib_file_read
 // Avetharun : 4-17-22 : added "copy sign" function
 // Avetharun : 4-27-22 : added json utilities & fixed  issue, causing things that must be declared seperately to not work.
 // Avetharun : 4-29-22 : modify the way function pointer types are created, now uses a macro. Formatted as follows:
      // d_typedef_func_ty(ret_val, ty_name, ty_arg_types_variadic)
      // Note: semicolon is NOT needed, as if it's put at the end, it will produce an intellisense warning. Apparently it's by design. Ignore it if it happens.
      // Original impl: https://github.com/avetharun/avetharun/blob/bf49a022c7021fb3200231722f7975f167e1cf9f/ave_libs.hpp#L308
                       // Also added assert handling
#include <string>
#define _ALIB_FQUAL static inline
std::string ___nn_alib_error_charp_str;
_ALIB_FQUAL char* alib_strfmt(const char* fmt, ...);
#define alib_get_error() ___nn_alib_error_charp_str.c_str()
#define alib_set_error(...) ___nn_alib_error_charp_str = alib_strfmt(__VA_ARGS__);

// Note: ignore any "function definition for typedef_func_ty" or "Y is not defined" errors. They're temporary.
#define d_typedef_func_ty(return_z, name, ...) typedef return_z (*name)(__VA_ARGS__);

#ifndef ftoa
char* ftoa(float f)
{
    static char        buf[17];
    char* cp = buf;
    unsigned long    l, rem;

    if (f < 0) {
        *cp++ = '-';
        f = -f;
    }
    l = (unsigned long)f;
    f -= (float)l;
    rem = (unsigned long)(f * 1e6);
    sprintf(cp, "%lu.%6.6lu", l, rem);
    return buf;
}
#endif
#ifndef NDEBUG
// Production builds should set NDEBUG=1
#define NDEBUG false
#endif
#define alib_malloca(TYPE, length) (TYPE*)malloc(length)
#define alib_malloct(TYPE) (TYPE*)malloc(sizeof(TYPE))
#ifdef WIN32
bool alib_can_reach_mem(void* ptr) {
    __try {
        char prefix = *(((char*)ptr)); //Get the first byte. If this doesn't cause an error, then we can reach this.
        return true;
    }
    __except (true) { //Catch all unique exceptions (Windows exceptions) 
        return false; //Can't reach this memory
    }
}
#endif
#ifndef ALIB_DEBUG_BUILD
#define ALIB_DEBUG_BUILD !NDEBUG
#endif

#if defined(ALIB_FORCE_BINARY) || (!defined(ALIB_NO_BINARY))
// 
//      Binary & bit manipulation utilities
//    - Avetharun
// 


/* Helper macros. Unless you know what you're doing, don't run these. */

// Sourced from https://bytes.com/topic/c/answers/219656-literal-binary
// Binary decoding to unsigned long v
#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)
// Binary encoding into hex (internal+user-runnable)
#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))

#define B64(dmsb,db2,db3,db4,db5,db6,db7,dlsb) \
(\
	((unsigned long long int)B32(dmsb,db2,db3,db4)<<32  ) \
+	((unsigned long long int)B32(db5,db6,db7,dlsb)	   ) \
)

/** \todo B64(v,v,v,v,v,v,v,v)
 * but that'll be a mess...
 */

 // Internal bit manipulation
#define imp_bitenb(val,n)   ( ( val ) |=  (1<<(n))) // Sets bit n to 1
#define imp_bitdis(val,n)   ( ( val ) &= ~(1<<(n))) // Sets bit n to 0
#define imp_flipbit(val,n)  ( ( val ) ^=  (1<<(n))) // Inverts bit n
#define imp_getbitv(val,n)   ( ( val ) &   (1<<(n))) // Gets bit n from data val





/*---------------------------------------*/
/* User macros */

// Binary number creation to unsigned long


// Bit manipulation 
//
// Sets bit nbit to value bv in variable var
#define setbitv(var,nbit,val) (val > 0) ? imp_bitenb(var,nbit) : imp_bitdis(var,nbit)
#define setbitsv(var,val,amt) for(int nbit=0; nbit < amt; nbit++) { setbitv(var,nbit,val); }
#define setbitssv(var,start,val,amt) for(int nbit=start; nbit < start+amt; nbit++) { setbitv(var,nbit,val); }
// Sets bit nbit to TRUE/1
#define bitenablev(var,nbit)              imp_bitenb(var,nbit)
// Sets bit nbit to FALSE/0
#define bitdisablev(var,nbit)             imp_bitdis(var,nbit)
// Get bit nbit from value var
#define getbitv(var,nbit)                 imp_getbitv(var,nbit)
#define flipbitv(var, nbit)               imp_flipbit(var,nbit)

// Bit creation
// 

/**
 * Re-implementation of line @e 66 because compiler complains if I do the following:
 * #define Binary8(b8v) B8(b8v)
 * > Too many/little values in constant
*/
// Convert 8 bits into an unsigned char
#define Binary8(d) ((unsigned char)B8__(HEX__(d))) 
// Convert 16 bits (Each byte is seperated!) into an unsigned short
#define Binary16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))
// Convert 32 bits (Each byte is seperated!) into an unsigned long
#define Binary32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))
// Convert 64 bits (Each byte is seperated!) into an unsigned long long
#define Binary64(dmsb,db2,db3,db4,db5,db6,db7,dlsb) \
(\
	((unsigned long long int)B32(dmsb,db2,db3,db4)<<32  ) \
+	((unsigned long long int)B32(db5,db6,db7,dlsb)	   ) \
)
// Todo: Binary64 to T<long long>

/**
 * @todo @b Binary_Reading. Effectively reversing the above functions.
 * Planned implementation: Eventually.
 */



#endif // ALIB_NO_BINARY

#if defined(ALIB_FORCE_FILE) || (!defined(ALIB_NO_FILE))

#include <iosfwd>
#include <fstream>
#include <sstream>
#include <string>
 // If size != 0, leave size as is
_ALIB_FQUAL void __alib_internal_reqlen__f_impl(size_t* sz, const char* arr) {
    if ((*sz) == 0) {
        (*sz) = strlen(arr);
    }
}

_ALIB_FQUAL std::string alib_file_read(std::ifstream& file) {
    std::ostringstream buf;
    buf << file.rdbuf();
    return buf.str();
}
_ALIB_FQUAL std::string alib_file_read(const char* fname) {
    std::ostringstream buf;
    std::ifstream file(fname, std::ios::binary);
    buf << file.rdbuf();
    return buf.str();
}


_ALIB_FQUAL void alib_file_read(const char* fname, const char** out_, unsigned long long* size_) {
    std::string _fr = alib_file_read(fname);
    *out_ = _fr.c_str();
    *size_ = _fr.size();
}

_ALIB_FQUAL void alib_file_read(std::ifstream file, const char** out_, unsigned long long* size_) {
    std::string _fr = alib_file_read(file);
    *out_ = _fr.c_str();
    *size_ = _fr.size();
}

_ALIB_FQUAL bool alib_file_exists(const char* name) {
    if (FILE* file = fopen(name, "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

_ALIB_FQUAL void alib_file_write(const char* filen, const char* d, size_t l = 0) {
    std::ofstream file(filen, std::ios::out | std::ios::binary);
    __alib_internal_reqlen__f_impl(&l, d);
    file.write(d, l);
    file.close();
}
// Note: does NOT close file after!
_ALIB_FQUAL void alib_file_write(std::ostream& file, const char* d, size_t l = 0) {
    __alib_internal_reqlen__f_impl(&l, d);
    file.write(d, l);
}

#ifdef _WIN32
#include "direct.h"
#define PATH_SEP '\\'
#define GETCWD _getcwd
#define CHDIR _chdir
#else
#include "unistd.h"
#define PATH_SEP '/'
#define GETCWD getcwd
#define CHDIR chdir
#endif
_ALIB_FQUAL bool alib_scwd(const char* dir) { return CHDIR(dir) == 0; }

_ALIB_FQUAL char* alib_gcwd() {
    return GETCWD(0, 0);
}

#endif // ALIB_NO_FILE_UTILS


#if defined(ALIB_FORCE_CONCAT) || (!defined(ALIB_NO_CONCAT))

#define __concat_internal3(a, b, c) a##b##c
#define __concat_internal2(a, b) a##b
#define __concat4__internal(a,b,c,d) __concat_internal2(a,b)##__concat_internal2(c,d)
#define concat4(a,b,c,d) __concat4__internal(a,b,c,d)
#define concat3(a, b, c) __concat_internal3(a, b, c)
#define concat2(a, b) __concat_internal2(a, b)
#endif // ALIB_NO_CONCAT





#if defined(ALIB_FORCE_NONAMES) || (!defined(ALIB_NO_NONAMES))
// This may require some explaining. I use these for creating "unnamed" functions and variables, at compile time.
// Obviously, since they're unnamed, they're single use.
// The easiest way for me to use this, is for unnamed struct initializing lambda functions, which uses a struct similar to this:
/*
#include <functional>
struct inline_initializer{
    inline_initializer(std::function<void()> i) {
      i();
    }
};
inline_initializer _nn {
    [&] () {
        std::cout << "This runs before main()!";
    }
};
*/
// Since the variable is single use, as is obvious,

// What it should output is obviously based upon compiler, but what it'll look like on GCC is roughly this:
// ____34_unl__noname_line_34_LINEPOS_34_un_
// If you want to get the noname as a variable, do _nng(LINE_NUM)
// This is the first instance of _nn (), on line 23 of any file. At the moment, GCC doesn't have a "remove . " pragma, 
// so file-specific nonames that are on the same line are impossible.

#define ___nn_internal_concat__(a, b, c) a##b##c
#define ___nn_internal_concat(a, b, c) ___nn_internal_concat__(a, b, c)
#define _1_nn_internal__(a) ___nn_internal_concat(a, _LINEPOS_, __LINE__)
#define _1_nn_internal__2__ _1_nn_internal__(___nn_internal_concat(_noname, _line_, __LINE__))
#define _1_nn_internal__2_1_ ___nn_internal_concat(_, ___nn_internal_concat(__, __LINE__, _unl), _)
#define _1_nn_internal__3__ ___nn_internal_concat(_1_nn_internal__2_1_, _1_nn_internal__2__, _un)


#define _g_nn_internal__(a, line) ___nn_internal_concat(a, _LINEPOS_, line)
#define _g_nn_internal__2__(line) _g_nn_internal__(___nn_internal_concat(_noname, _line_, line), line)
#define _g_nn_internal__2_1_(line) ___nn_internal_concat(_, ___nn_internal_concat(__, line, _unl), _)
#define _g_nn_internal__3__(line) ___nn_internal_concat(_g_nn_internal__2_1_(line), _g_nn_internal__2__(line), _un)


#define _nn_impl ___nn_internal_concat(_, _1_nn_internal__3__, _) // Create no-name object
#define _nn _nn_impl
#define _nng(line) ___nn_internal_concat(_, _g_nn_internal__3__(line), _) // Get no-name object at file line (LINE)

#endif // ALIB_NO_NONAMES

#if defined(ALIB_FORCE_INLINE_RUNNERS) || (!defined(ALIB_NO_INLINE_RUNNERS)) 

#include <functional>
// lambda-based runner function. Runs when program initializes all variables.
struct alib_inline_run {
    alib_inline_run(std::function<void()> initFunc) {
        initFunc();
    }
    d_typedef_func_ty(void, void_0a_f_impl);
    template <typename T = void_0a_f_impl>
    alib_inline_run(T initFunc) {
        initFunc();
    }
};


#endif // ALIB_NO_INLINE_RUNNERS


#if (defined(ALIB_FORCE_WIN_UTILS) && defined(_WIN32)) && (!defined(ALIB_NO_WIN_UTILS)) 
#include <Windows.h>
void alib_hide_console()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void alib_show_console()
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool alib_console_visible()
{
    return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}


#endif

#if defined(ALIB_FORCE_FUNCPTR) || (!defined(ALIB_NO_FUNCPTR))
#define noop (void)0
d_typedef_func_ty(int, int_2i_f, int, int)
d_typedef_func_ty(int, int_1i_f, int)
d_typedef_func_ty(int, int_0a_f)
d_typedef_func_ty(void, int_1i_1p_f, int, void*)
d_typedef_func_ty(void, void_0a_f)
d_typedef_func_ty(void, void_1i_f, int)
d_typedef_func_ty(void, void_2i_f, int, int)
d_typedef_func_ty(void, void_1pc_1i32_f, const char*, uint32_t)
#endif // ALIB_NO_FUNCPTR


#if defined(ALIB_FORCE_BYTE_UTILS) || (!defined(ALIB_NO_BYTE_UTILS))
#include <cmath>
_ALIB_FQUAL unsigned long alib_fnull(const char* arr) {
    int n_t_offset = 0;
    while (*arr++ != '\0') { n_t_offset++; }
    return n_t_offset;
}
_ALIB_FQUAL unsigned long alib_n_fnull(const char* arr, int limit) {
    for (int i = 0; i < limit; i++) {
        if (arr[i] == '\0') {
            return i;
        }
    }
    return 0;
}
_ALIB_FQUAL size_t alib_nulpos(const char* arr) {
    return alib_fnull(arr);
}
_ALIB_FQUAL size_t alib_nulposn(const char* arr, int limit) {
    return alib_n_fnull(arr, limit);
}
template <typename T>
_ALIB_FQUAL T alib_max(T first, T second) {
    if (first < second) { return second; }
    return first;
}
template <typename T>
_ALIB_FQUAL T alib_min(T first, T second) {

#ifndef _CMATH_
    if (first > second) { return second; }
    return first;
#else 
    return min(first, second);
#endif
}
#define __alib_haslogm__
_ALIB_FQUAL int alib_log(int base, int n) {
#ifndef _CMATH_
    // 75% as fast as cmath, if using recursion. If we have cmath avalible, use that instead.
    return (n > base - 1)
        ? 1 + alib_log(n / base, base)
        : 0;
#else
    return (int)(log(n) / log(base));
#endif
}
#define __alib_haswrange__
_ALIB_FQUAL bool alib_wrange(int min, int max, int val) {
    return ((val - max) * (val - min) <= 0);
}
// Taken from python's usage of math.isclose()
_ALIB_FQUAL bool alib_fclose(float first, float second, float rel_tol = 1e-09, float abs_tol = 0.0) {
    return abs(first - second) <= alib_max(rel_tol * alib_max(abs(first), abs(second)), abs_tol);
}
_ALIB_FQUAL bool alib_dclose(double first, double second, double rel_tol = 1e-09, double abs_tol = 0.0) {
    return abs(first - second) <= alib_max(rel_tol * alib_max(abs(first), abs(second)), abs_tol);
}


_ALIB_FQUAL int alib_digitsInNum(long n, int base = 10)
{
    if (n == 0) { return 1; }
    if (n < 0) {
        n *= -1;
    }
    // log using base, then
    // taking it to the lowest int then add 1
    return (int)floor(alib_log(base, n)) + 1;
}

// Get digits of [num], formatted as ASCII (by default, pass false to disable)
// arr[0] is the amount of digits in the array
// Note: malloc 
_ALIB_FQUAL char* alib_getDigitsOfNumber(int num, bool ascii = true) {
    int amt_digits = alib_digitsInNum(num, 10);
    if (amt_digits <= 0) {
        amt_digits = 1;
    }
    char* digits = (char*)malloc(amt_digits + 1);
    int i = 1;
    while (num != 0)
    {
        // What even is math?
        const int least_significant_digit = num % 10;
        digits[i] = (least_significant_digit + ((ascii) ? 48 : 0));
        num /= 10;
        i++;
    }

    return digits;
}

static const char* alib_bit_rep[16] = {
    "0000","0001","0010","0011",
    "0100","0101","0110","0111",
    "1000","1001","1010","1011",
    "1100","1101","1110","1111",
};

// If size != 0, leave size as is, otherwise size = strlen(arr)
_ALIB_FQUAL void alib_reqlen(size_t* sz, const char* arr) {
    if ((*sz) == 0) {
        (*sz) = strlen(arr);
    }
}

_ALIB_FQUAL void alib_print_byte(uint8_t byte)
{
    printf("%s%s", alib_bit_rep[byte >> 4], alib_bit_rep[byte & 0x0F]);
}

_ALIB_FQUAL int alib_endswith(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

_ALIB_FQUAL int alib_beginswith(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lenprefix = strlen(prefix);
    if (lenprefix > lenstr)
        return 0;
    return strncmp(str, prefix, lenprefix) == 0;
}
// Get string between two strings
_ALIB_FQUAL int alib_substr2(const char* input, size_t input_len) {

}
// if string begins with X and the character afterward is NOT the last character of the prefix.
_ALIB_FQUAL int alib_beginswith_anddoesntfollow(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lenprefix = strlen(prefix);
    if (lenprefix > lenstr)
        return 0;
    // No character after
    if (lenstr < lenprefix + 1) { return strncmp(str, prefix, lenprefix) == 0; }
    return strncmp(str, prefix, lenprefix) == 0 && str[lenprefix] != prefix[lenprefix - 1];
}
// get position of char 
_ALIB_FQUAL int alib_getchrpos(const char* src, char c, size_t len = 0)
{
    alib_reqlen(&len, src);
    for (size_t i = 0; i < len; i++)
    {
        if (src[i] == c)
            return (int)i;
    }
    return -1;
}
// char array contains
_ALIB_FQUAL bool alib_costr(std::string src, const char* match) {
    if (src.find(match) != std::string::npos) {
        return true;
    }
    return false;
}
_ALIB_FQUAL int alib_chreq(const char* src, const char* match) {
    size_t sl = strlen(src);
    size_t ml = strlen(match);
    if (ml > sl) { return false; }
    return strncmp(src, match, ml);
}
_ALIB_FQUAL int alib_streq(std::string src, const char* match) {
    size_t sl = src.size();
    size_t ml = strlen(match);
    if (ml > sl) { return false; }
    return (src.compare(match) == 0);
}
// Occurances of char `c` in `src`
_ALIB_FQUAL size_t alib_chrocc(const char* src, char c, size_t len = 0) {
    alib_reqlen(&len, src);
    size_t occ = 0;
    for (size_t i = 0; i < len; i++) {
        if (src[i] == c) { occ++; }
    }
    return occ;
}
#include <vector>
#include <cstdarg>
_ALIB_FQUAL const char* alib_rmocc(const char* src, char c, size_t len = 0) {
    alib_reqlen(&len, src);
    std::vector<char> src_copy;
    for (int i = 0; i < len; i++) {
        if (src[i] == c) { continue; }
        src_copy.push_back(src[i]);
    }
    src_copy.push_back('\0');
    return src_copy.data();
}

// get byte at offset of array 
_ALIB_FQUAL char alib_get_byte(void* data, int offset) {
    return ((char*)data)[offset];
}
// get byte at array[0]
_ALIB_FQUAL char alib_get_byte(void* data) {
    return ((char*)data)[0];
}
// set byte at offset of array 
_ALIB_FQUAL void alib_set_byte(void* data, char byte, int offset) {
    reinterpret_cast<char*>(data)[offset] = byte;
}
// set byte at array[0]
_ALIB_FQUAL void alib_set_byte(void* data, char byte) {
    reinterpret_cast<char*>(data)[0] = byte;
}

#ifdef _MSC_VER
#define VSCPRINTF "native (symlinked)"
#define vscprintf _vscprintf
#endif

#ifdef __GNUC__
#define VSCPRINTF "implemented"
_ALIB_FQUAL int vscprintf(const char* format, va_list ap)
{
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int retval = vsnprintf(NULL, 0, format, ap_copy);
    va_end(ap_copy);
    return retval;
}
#endif

/*
 * asprintf, vasprintf:
 * MSVC does not implement these, thus we implement them here
 * GNU-C-compatible compilers implement these with the same names, thus we
 * don't have to do anything
 */
#ifdef _MSC_VER
#define VASPRINTF "implemented"
#define ASPRINTF "implemented"
_ALIB_FQUAL int vasprintf(char** strp, const char* format, va_list ap)
{
    int len = vscprintf(format, ap);
    if (len == -1)
        return -1;
    char* str = (char*)malloc((size_t)len + 1);
    if (!str)
        return -1;
    int retval = vsnprintf(str, len + 1, format, ap);
    if (retval == -1) {
        free(str);
        return -1;
    }
    *strp = str;
    return retval;
}

int asprintf(char** strp, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    int retval = vasprintf(strp, format, ap);
    va_end(ap);
    return retval;
}
#endif

#ifdef __GNUC__
#define VASPRINTF "native"
#define ASPRINTF "native"
#endif


_ALIB_FQUAL size_t alib_2d_ar_pos(size_t pitch, size_t x, size_t y, size_t bytes_per_step = 4) {
    return y * pitch + x * bytes_per_step;
}
_ALIB_FQUAL size_t alib_va_arg_length(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    va_end(args);
    return bufsz;
}
_ALIB_FQUAL size_t alib_va_arg_length(const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    return bufsz;

}

// Note: you're expected to supply an empty pointer & manage memory after this! It uses malloc, not new.
_ALIB_FQUAL void alib_va_arg_parse(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    buf = (char*)malloc(bufsz + 1);
    vsprintf((char*)buf, fmt, args);
    va_end(args);
}
_ALIB_FQUAL void alib_va_arg_parse(char* buf, const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    buf = (char*)malloc(bufsz + 1);
    vsprintf((char*)buf, fmt, args);
}
_ALIB_FQUAL const char* alib_va_arg_parse(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    const char* _buf = (const char*)malloc(bufsz);
    vsprintf((char*)_buf, fmt, args);
    va_end(args);
    return _buf;
}
_ALIB_FQUAL const char* alib_va_arg_parse(const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    const char* _buf = (const char*)malloc(bufsz);
    vsprintf((char*)_buf, fmt, args);
    return _buf;
}
_ALIB_FQUAL const char* alib_strfmtv(const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    const char* _buf = (const char*)malloc(bufsz);
    vsprintf((char*)_buf, fmt, args);
    return _buf;
}
_ALIB_FQUAL char* alib_strfmt(const char* fmt, ...) {
    va_list args;
    std::string out;
    va_start(args, fmt);
    size_t bufsz = 0;
    bufsz = snprintf(NULL, 0, fmt, args);
    char* _buf = new char[bufsz] {'\0'};
    vsprintf(_buf, fmt, args);
    out.append(_buf);
    va_end(args);
    return (char*)out.c_str();
}
_ALIB_FQUAL std::string alib_strfmts(const char* fmt, ...) {
    va_list args;
    std::string out;
    va_start(args, fmt);
    size_t bufsz = 0;
    bufsz = snprintf(NULL, 0, fmt, args);
    char* _buf = new char[bufsz] {'\0'};
    vsprintf(_buf, fmt, args);
    out.append(_buf);
    va_end(args);
    return out;
}
_ALIB_FQUAL const char* alib_chrrepl(const char* in, char match, char repl_value) {
    while (*(in++)) {
        if (*in == match) {
            (*(char*)in) = repl_value;
        }
    }
    return in;
}
// Unlike the character replacement version, this will be re-allocating on the stack if needed
// Array will be trimmed after, make sure to free() it!
_ALIB_FQUAL char* alib_strrepl(char* in, const char* match, const char* repl, size_t in_len = 0, size_t match_len = 0, size_t repl_len = 0) {
    return (char*)"NTIMPL\0"; // NTIMPL
}
// Copies the sign-ed ness of A into B
_ALIB_FQUAL void alib_copy_signed(signed int a, signed int* b) {
    *b = (a < 0) ? -*b : (*b < 0) ? -*b : *b;
}
template <typename T>
_ALIB_FQUAL void alib_clampptr(T* out, T lower, T upper) {
    *out =
        (*out <= lower) ? lower : // out <= lower : return lower
        (*out <= upper) ? *out :  // out <= upper : return out
        upper;                     // out >  upper : return upper
}
template <typename T>
_ALIB_FQUAL T alib_clamp(T n, T lower, T upper) {
    return n <= lower ? lower : n <= upper ? n : upper;
}
#include <codecvt>
#include <locale>
_ALIB_FQUAL std::wstring convert_wstr(std::string str) {
    std::wstring str2(str.length(), L' '); // Make room for characters
    std::copy(str.begin(), str.end(), str2.begin());
    return str2;
}

// Begin std::string specific utilities

#include <string>
#include <regex>		// regex, sregex_token_iterator
#include <algorithm>    // copy
#include <iterator>     // back_inserter
#include <iomanip>

_ALIB_FQUAL void alib_split(std::string arr, std::string del, std::vector<std::string>* out)
{
    size_t start = 0;
    size_t end = arr.find(del);
    while (end != -1) {
        out->push_back(arr.substr(start, end - start));
        start = end + del.size();
        end = arr.find(del, start);
    }
    out->push_back(arr.substr(start, end - start));
}

_ALIB_FQUAL std::vector<std::string> alib_split(const std::string& s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        //elems.push_back(item);
        elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
    }
    return elems;
}
_ALIB_FQUAL void alib_strsplit(std::string& str, char delim, std::vector<std::string>& out)
{
    size_t start;
    size_t end = 0;
    // Doesn't contain 'delim', so return the base string itself.
    if ((str.find_first_not_of(delim, end)) == std::string::npos) {
        out.push_back(str);
        return;
    }
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(str.substr(start, end - start));
    }
}
_ALIB_FQUAL std::string alib_str_hex(std::string s) {

    std::stringstream s1;
    for (int i = 0; i < s.length(); i++) {
        s1 << std::hex << (int)s.at(i);
    }

    return s1.str();
}
_ALIB_FQUAL std::string alib_int_hex(long l) {
    std::stringstream stream;
    stream << std::hex << l;
    return std::string(stream.str());
}

#include <string>
#include <algorithm>

//  Lowercases string
_ALIB_FQUAL std::string alib_lower(const char* s)
{
    std::string s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(), tolower);
    return s2;
}

// Uppercases string
_ALIB_FQUAL std::string alib_upper(const char* s)
{
    std::string s2 = s;
    std::transform(s2.begin(), s2.end(), s2.begin(), toupper);
    return s2;
}

_ALIB_FQUAL double alib_percent(long double num, double percent) {
    long double _n_d100 = (num / 100);
    return lroundl(_n_d100 * percent);
}
_ALIB_FQUAL float alib_percentf(float num, float percent) {
    float _n_d100 = (num * 0.01f);
    return roundf(_n_d100 * percent);
}

_ALIB_FQUAL int alib_percents(int base, std::string percent_str) {
    if ((alib_endswith(percent_str.c_str(), "%"))) percent_str.erase(percent_str.end());
    int percent = atoi(percent_str.c_str());
    return (int)alib_percent(base, percent);
}

#include <cmath>
#define alib_max(a,b) (((a) > (b)) ? (a) : (b))
#define alib_min(a,b) (((a) < (b)) ? (a) : (b))
template<typename T> struct alib_is_shared_ptr : std::false_type {};
template<typename T> struct alib_is_shared_ptr<std::shared_ptr<T>> : std::true_type {};


template <typename T>
_ALIB_FQUAL void alib_remove_any_of(std::vector<T> _v, T vy) {
    for (int i = 0; i < _v.size(); i++) {
        if (_v.at(i) == vy) {
            _v.erase(_v.begin() + i);
        }
    }
    _v.shrink_to_fit();
}

template <typename T>
_ALIB_FQUAL bool alib_contains_any_of(std::vector<T> _v, T vy) {
    for (int i = 0; i < _v.size(); i++) {
        if (_v.at(i) == vy) {
            return true;
        }
    }
    return false;
}

#endif // ALIB_NO_BYTE_UTILS

#if (defined(ALIB_FORCE_ASSERT) && __has_include(<conio.h>)) && (!defined(ALIB_NO_ASSERT))
#include <assert.h>
#include <stdio.h>
#include <conio.h>
#define alib_assert(condition, fmt, ...) if (condition) { fprintf(stderr, fmt, __VA_ARGS__); fprintf(stderr, "\nPress any key to exit.\n"); while (!_getch()) {} exit(-1); }
#define alib_asserti(condition, fmt, ...) if (condition) { \
    fprintf(stderr, fmt, __VA_ARGS__); fprintf(stderr, "\nIgnore? Y/N"); \
    int k = _getch();\
    if (tolower(k)== 'y') {} else if (tolower(k) == 'n') {\
        exit(-1);\
    } else {fprintf(stderr, "Not a valid input. Please type Y/N");}\
}
#if defined (_WIN32)
#include <Windows.h>
#define alib_assert_p(condition,fmt, ...) if( condition ){ MessageBox(0, convert_wstr(alib_strfmt(fmt, __VA_ARGS__)).c_str(), L"Assertion failed", MB_OK); exit(-1); }
#define alib_assert_pi(condition, fmt, ...) if (condition) { \
        int state = MessageBox(0, convert_wstr(alib_strfmt(fmt, __VA_ARGS__)).c_str(), L"Assertion failed. Ignore?", MB_YESNO); \
        switch (state){\
        case IDNO:\
            exit(1);\
            break;\
        };\
    }
#endif
#endif // ALIB_FORCE_ASSERT
#include <vector>
#include <map>
template <typename T>
_ALIB_FQUAL void alib_invalidatev(std::vector<T> __v) {
    for (int i = 0; i < __v.size(); i++) {
        delete& __v.at(i);
    }
    __v.clear();
}
template <typename K = std::string, typename V>
_ALIB_FQUAL void alib_invalidatem(std::map<K, V> m) {
    for (const auto& kv : m) {
        delete kv.second;
    }
    m.clear();
}
template <typename V_T>
_ALIB_FQUAL void alib_remove_if(std::vector<V_T> _vec, std::function<bool(V_T)> _p) {
    for (int i = 0; i < _vec.size(); i++) {
        if (_p(_vec.at(i))) {
            _vec.erase(_vec.begin() + i);
        }
    }
}
#include <chrono>
#include <thread>
#define alib_sleep_micros(micros) std::this_thread::sleep_for(std::chrono::microseconds(micros));
#define alib_sleep_millis(millis) std::this_thread::sleep_for(std::chrono::milliseconds(millis));
#define alib_sleep_second(second) std::this_thread::sleep_for(std::chrono::microseconds(second));


#endif // __lib_aveth_utils_hpp


// begin preprocessor defs that need to be explicitly defined
#if defined(ALIB_ANDROID) && !defined(alib_android_logging_helper__)
#define alib_android_logging_helper__
#if !defined(ALIB_PROJECT_NAME)
#define ALIB_PROJECT_NAME "UnnamedAndroidProject (DEFINE USING #define ALIB_PROJECT_NAME '')"
#endif
_ALIB_FQUAL int android_fread(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

_ALIB_FQUAL int android_fwrite(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

_ALIB_FQUAL fpos_t android_fseek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

_ALIB_FQUAL int android_fclose(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}
_ALIB_FQUAL long android_ftell(void* cookie) {
    return AAsset_getLength64((AAsset*)cookie);
}
_ALIB_FQUAL FILE* android_fopen(const char* fname, AAssetManager* _mgr) {
    AAsset* asset = AAssetManager_open(_mgr, fname, 0);
    if (!asset) return NULL;

    return funopen(asset, android_fread, android_fwrite, android_fseek, android_fclose);
}
#define alib_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, ALIB_PROJECT_NAME, __VA_ARGS__))
#define alib_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, ALIB_PROJECT_NAME, __VA_ARGS__))
#define alib_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, ALIB_PROJECT_NAME, __VA_ARGS__))
#define alib_LOGF(...) ((void)__android_log_print(ANDROID_LOG_FATAL, ALIB_PROJECT_NAME, __VA_ARGS__))
#define alib_LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, ALIB_PROJECT_NAME, __VA_ARGS__))
#endif

// Create macros to emulate "public T N = V" used in other languages
#if defined(ALIB_VARIABLE_MANAGER) && !defined(alib_visibility_helper__)
#define alib_visibility_helper__

// These macros assume that it's to invert a variable's visibility, so don't use them if you don't want that!

#define public(var) public: var; private:
#define private(var) private: var; public:

#endif


#if defined(ALIB_INSTANCER) && !defined(alib_instance_management_helper__)
#define alib_instance_management_helper__

#define __private_internal_(var) private: var; public:
// Creates a private(static TYPE* instance)
#define genInstance(TYPE) \
    __private_internal_(static TYPE* instance); \
    static TYPE* GetInstance() { return (instance == nullptr) ? new TYPE() : instance; }
#define genInstanceDef(TYPE) \
    TYPE* TYPE::instance = nullptr;

#endif


//
// nlohmann/json utilities
//
#if defined(INCLUDE_NLOHMANN_JSON_HPP_) && (!defined(alib_json_utilities__included_))
#define alib_json_utilities__included_
// nlohhman/json included, or alib_force_json defined use these utilities

using ___alib__json = nlohmann::json;
#define JSONREF ___alib__json &


int alib_j_geti(JSONREF j) {
    return j.get<int>();
}
float alib_j_getf(JSONREF j) {
    return j.get<float>();
}
double alib_j_getd(JSONREF j) {
    return j.get<double>();
}
std::string alib_j_getstr(JSONREF j) {
    return j.get<std::string>();
}
const char* alib_j_getchara(JSONREF j) {
    return alib_j_getstr(j).c_str();
}
bool alib_j_streq(JSONREF j, std::string match) {
    std::string src = j.get<std::string>();
    if (src.size() < match.size()) {}
    return (src.compare(match) == 0);
}
bool alib_j_costr(JSONREF j, std::string match) {
    if (j.is_array()) {
        for (int i = 0; i < j.size(); i++) {
            if (j[i].get<std::string>().compare(match.c_str()) == 0) { return true; };
        }
    }
    // If j_ty = array : the following will always return false.
    return j.contains(match);
}
bool alib_j_ieq(JSONREF j, int match) {
    return (j.get<int>() == match);
}
bool alib_j_feq(JSONREF j, float match) {
    return (j.get<float>() == match);
}
// If [j] contains keys formatted as "a<newline>b<newline>c"
// Note: replace <newline> with \\n (unescaped due to comment)
// (eg if the json element is {"a":{"b":{"c":"d"}}} it would return true)
bool alib_j_cokeys(___alib__json j, std::string _s) {
    ___alib__json j_t = j;
    std::vector<std::string> s_keys;
    alib_strsplit(_s, '\n', s_keys);


    for (int i = 0; i < s_keys.size(); i++) {
        std::string s_cur = s_keys[i];
        if (!j_t.contains(s_cur)) {
            return false;
        }
        j_t = j_t.at(s_cur);
    }
    return true;

}
#undef JSONREF
#undef _CRT_SECURE_NO_WARNINGS

#endif


