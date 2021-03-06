/*
 * Copyright (c) 2006-2015 David Schor (david@zigwap.com), ZigWap LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef LIBALEPH_H
#define LIBALEPH_H
/**
 * \file aleph.h
 * 
 * This file contains all the function prototypes and macros for working
 * with aleph strings. This is the only header needed to be included by
 * your project.
 * 
 * Aleph operates on dynamically allocated strings called Aleph Strings formally
 * called \link #a_str \em a_str \endlink and \link #a_cstr \em a_cstr \endlink.
 * \em a_cstr is a version of a_str that simply points to const data.
 * 
 * \b LibAleph is a portable Unicode string library for C. LibAleph operates on
 * \b UTF-8 encoded strings, provides utilities to work and manipulate them, as
 * well as passing them back to legacy interfaces that expect NULL-terminated
 * strings. 
 * 
 * LibAleph is highly intuitive and easy to use.
 * 
 * 
 * Aleph Concepts
 * --------------
 * 
 * Before going any further, we must talk about a few concepts that get used 
 * accross the entire library. They are clearly defined below:
 * 
 * Language:
 * 
 * - \b Grapheme             - The fundamental unit in written language.
 * 
 * - \b Glyph                - A basic element of of a language; a visual variant of a Grapheme.
 * 
 * - \b Character            - In LibAleph, a code point. (\b avoid using that term, it's _ambiguous_.)
 * 
 * Unicode:
 *
 * - \b Grapheme \b Cluster  - That's what the Unicode standard calls user-perceived characters.
 *                             They are what the USER thinks are characters. In reality a single
 *                             grapheme cluster might be composed of one or more code points. When
 *                             Aleph operates on grapheme clusters, we mean \b extended \b grapheme
 *                             \b clusters.
 * 
 * - \b Code \b Point        - Any numerical value in the Unicode codespace, I.E. any value from
 *                             \ref A_MIN_CP "0x0" to \ref A_MAX_CP "0x10FFFF".
 * 
 * - \b Code \b Unit         - A minimal bit-combination that represents a valid code point, in UTF-8.
 * 
 * 
 * Aleph Function Variations
 * -------------------------
 * 
 * LibAleph can operate on a string in three "modes":
 * 
 * - Based on offsets
 *      - These are the default variations as they are usually considerably more efficient and are
 *        normally more natural to worth with.
 * 
 *          Example: `a_substr()`, `a_find()`, `a_del()`, and `a_ins()`
 * 
 * - Based on code points indices
 *      - These functions rely on the index of the code point instead of their offsets. Those functions
 *        should be used sparingly as often they involve repeated linear scanning of the string.
 *      - Index-based functions all have a `x` prefix to them.
 * 
 *          Example: `a_xsubstr()`, `a_xfind()`, `a_xdel()`, and `a_xins()`
 * 
 * - Based on grapheme clusters index
 *      - These functions operate on whole grapheme clusters instead of code points. These functions all
 *        have a `g` prefix to them.
 * 
 *          Example: `a_gsubstr()`, `a_gfind_from()`, `a_del()`, and `a_ins()
 * 
 * 
 * Visual Overview
 * ---------------
 * 
 * \image html chars.png
 * 
 * 
 * Naming Convension
 * -----------------
 * 
 * LibAleph follows a relatively consistent naming convension:
 * 
 * - All LibAleph identifiers start with `a_` (or `A_`).
 * - A verb is used to describe the main functionality of the set
 *   of functions, for example `substr`, `find`, `cmp`, and `inst`.
 *    - Variations of the function that take different arguments will
 *      include the argument type in the name:
 *      - _cstr = `const char *`
 *      - _cp = `a_cp`
 *      - _chr = `const char *` (only reads a single codepoint)
 *      - _str  = `a_str` / `a_cstr`
 *      - _count / _len / _size = `size_t` / `long`
 * - Modifiers that impact how the function operaters are specified
 *   as a single letter prefix to the verb.
 *      - Case-insensitive version of the function is prefixed with an
 *        `i`, for example `icmp`, `ifind`, and `istartswith`.
 *      - Grapheme index version of the function is prefixed with a
 *        'g', for example `gfind` and `gsubstr`
 *      - Code point index version of the function is prefixed with a
 *        'x', for example `xfind`and `xsubstr`
 * 
 * Preconditions
 * -------------
 * 
 * LibAleph has two preconditions that \b MUST be enforced in all
 * functions:
 * 
 *  - Unless otherwise stated, ALL string inputs \b MUST be
 *    valid UTF-8 encoded strings.
 *  - ALL code point inputs \b MUST be a valid value in the Unicode
 *    codespace.
 * 
 * A set of sanitization functions are provided which can be used
 * to fix/validate UTF-8 encodings from unknown input sources.
 * Additionally, LibAleph makes heavy use of `assert()` statements to
 * validate that such inputs do not make their way into the library.
 * It's highly recommended that you do not disable asserts() during
 * debug mode, as it's your first line of defense.
 * 
 * Chaining
 * --------
 * 
 * LibAleph utilizes dynamically-allocated character arrays. As such,
 * they can fail usually under one circumstance: when `malloc()` or
 * `realloc()` returns `NULL`. (Although in pratice, almost all modern
 * systems overcommit memory by default, making this very unlikely)
 * 
 * LibAleph provides the special `NULL Passthrough` mode which, when
 * enabled, allows NULL arguments to 'passthrough' the function safely.
 * The NULL Passthrough mode can be enabled by defining the macro 
 * `A_NULL_PASSTHROUGH`.
 * 
 * a_str type
 * -----------
 * 
 * LibAleph operates on an `a_str` type which is in fact just a `char *`.
 * LibAleph does do minimal bookkeeping in a header it maintains just before
 * the actual string buffer. This design allows for aleph strings to be
 * treated safely as a NULL-terminated UTF-8 strings whereever desired.
 * Additionally, if you must, you can modify the content of the buffer
 * yourself provided you call `a_sync()` to make sure aleph bookkeeping is
 * in-sync with the modified buffer. You may also call `a_ensure()` and
 * `a_reserve()` to resize the buffer to the desired size.
 * 
 * It's important to note that passing an a_str object to a function that has
 * the potential to resize the buffer may invalidate the original pointer.
 * You must save the return value which may return a new pointer. (those 
 * functions are noted appropriately in this doc.) For 
 * example:
 * 
 *      a_cat_cstr(foo, "123");
 * 
 * is incorrect, you must assign the return value to foo, i.e.:
 * 
 *      foo = a_cat_cstr(foo, "123");
 * 
 * Hello World
 * --------------
 * 
 * \includelineno hello.c
 * 
 * To cluster, or not to cluster, that is the question
 * ------------------
 * 
 * LibAleph offers various ways of operating on \em grapheme \em clusters, a
 * sequence of one or more code points that should be treated as a single unit.
 * Generally speaking, grapheme clusters are what most text editors should be
 * operating on (when the user highlights a "character"). Such clusters should
 * not be broken down (e.g. add a line break in the middle) as that would very
 * likely result in deformed visual rendering.
 * 
 * Table of contents
 * ------------------
 * 
 * - Basic
 *    - \ref creation_functions "Creation/Destruction"
 *    - \ref assignment_functions "Assignments"
 * - Access/Iterator
 *    - \ref index_functions "Index"
 *    - \ref iterator_functions "Iterators"
 * - Character Info
 *    - \ref character_name_functions "Names"
 *    - \ref categories_functions "Categories"
 *    - \ref blocks_functions "Blocks"
 * - Normalization
 *    - \ref normalization_functions "Normalization"
 * - String Info
 *    - \ref length_functions "Length/Size"
 * - String Manipulation
 *    - \ref concatenation_functions "Concatenation"
 *    - \ref transformation_functions "Transformations"
 *    - \ref case_functions "Case Detection & Manipulation"
 *    - \ref insertion_functions "Insertion"
 *    - \ref replacement_functions "Replacement"
 *    - \ref deletion_functions "Deletion"
 *    - \ref trimming_functions "Trimming"
 *    - \ref reversal_functions "Reversal"
 *    - \ref escaping_functions "Escaping/Unescaping"
 * - Splitting/Joining
 *    - \ref splitting_functions "Splitting"
 *    - \ref joining_functions "Joining"
 * - Substrings
 *    - \ref substring_counting_functions "Substring Countin"
 *    - \ref substring_extraction_functions "Substring Extraction"
 *    - \ref tokenization_functions "Token/List Manipulation"
 * - Comparing/Searching
 *    - \ref matching_functions "Matching"
 *    - \ref search_functions "Searching"
 *    - \ref start_end_cmp_functions "Start/End Comparisons"
 * - Memory-related
 *    - \ref custom_memory_functions "Custom Memory"
 *    - \ref buffer_management_functions "Buffer Management"
 *    - \ref gc_functions "GC Functions"
 * - Debug/Validation
 *    - \ref validation_functions "Validation"
 *    - \ref debug_functions "Debug"
 * - I/O Functions
 *    - \ref io_functions "I/O Functions"
 * - Locale
 *    - \ref locale_functions "Locale"
 * - Miscellaneous
 *    - \ref hashing_functions "Hashing"
 * - Unicode Version
 *    - \ref unicode_version_functions "Unicode Version"
 * 
 * Additional Info
 * ---------------
 * 
 * 
 * \author Copyright (c) 2006-2015 David Schor (david@zigwap.com), ZigWap LLC
 * 
 * <dl class="section"><dt>Bugs</dt><dd>Please report bugs by filing an issue on GitHub: https://github.com/ZigWap/LibAleph</dd></dl>
 * 
 * \copyright MIT License (LibAleph code) CC BY-ND 3.0 (Documentation)
 */
#include <stddef.h> /* used for 'size_t'   */ 
#include <assert.h> /* used for 'assert()' */
/*
 * 
 */
#ifndef A_MIN_STR_SIZE
/**
 * \brief Used as the minimum buffer size for newly-allocated strings. You can
 *        define this yourself if you know your application will be dealing
 *        with mostly larger strings.
 */
/** \hideinitializer */
#   define A_MIN_STR_SIZE 16
#endif
/** 
 * \anchor custom_memory_functions
 * \name Custom Memory
 *
 * 
 * @{
 */
/**
 * \brief Define to use custom allocation routines instead of the standard
 *        library ones.
 */
#ifndef A_CUSTOM_ALLOC
#   define A_MALLOC(size) malloc(size)
#   define A_CALLOC(nmemb, size) calloc(nmemb, size)
#   define A_REALLOC(ptr, size) realloc(ptr, size)
#   define A_FREE(ptr) free(ptr)
#else
#   ifdef A_CUSTOM_ALLOC_HOOK

#   endif
#endif
/*@}*/
/**
 * \brief Defining A_INCLUDE_NAMES will include name look-up 
 *        functionality. (Which includes a name lookup table
 *        of considerable size.)
 */
#ifndef A_INCLUDE_NAMES
/** \hideinitializer */
#   define A_INCLUDE_NAMES 0
#endif
#ifndef A_INCLUDE_LOCALE
#   define A_INCLUDE_LOCALE 1
#endif
#ifndef A_INCLUDE_LOCALE_LANG_NAME
#   define A_INCLUDE_LOCALE_LANG_NAME 1
#endif
#ifndef A_INCLUDE_IO
#   define A_INCLUDE_IO 0
#else
#   include <stdio.h>
#endif
#ifndef A_INCLUDE_MEM
#   define A_INCLUDE_MEM 0
#else
#   ifndef A_MIN_STR_POOL_SIZE
        /**
        * \brief Used as the minimum string pool size for newly-allocated strings.
        *        You can define this yourself if you know your application will be
        *        dealing with mostly larger string pools.
        */
#       define A_MIN_STR_POOL_SIZE 16
#   endif
#endif
#ifndef DOXYGEN_DOCS /* START hide from doxygen */
#   define A_ASSERT_UTF8(s) \
        assert(((void)"LibAleph: Precondition Violated: string passed is not a valid UTF-8 string.", \
        !a_is_valid_utf8(s)))
#   define A_ASSERT_CODEPOINT(cp) \
        assert(((void)"LibAleph: Precondition Violated: codepoint provided is outside of the Unicode codespace.", \
        (A_MIN_CP <= (cp) && (cp) <= A_MAX_CP)))
#   define A_ASSERT_CODEPOINT_BOUNDARY(chr) \
        assert(((void)"LibAleph: Precondition Violated: offset provided doesn't point to a start of a code unit sequence.", \
        (((chr) & 0xC0) != 0x80)))
#endif  /* END hide from doxygen */
#ifdef __STDC_VERSION__
#   if __STDC_VERSION__ >= 199901L
#       define ALEPH_C_V 2
#   endif
#else
#   define ALEPH_C_V 1
#endif
/**
 * \brief Enables NULL passthrough for all library functions. I.e. allows for safe
 *        chaining of functions where if one fails and returns NULL, it will be 
 *        propagated onward without causing issues.
 * 
 * \note  NDEBUG must be defined to disable the standard assert that check for NULL values.
 */
#ifndef A_NULL_PASSTHROUGH
#   define A_NULL_PASSTHROUGH 0
#   define PASSTHROUGH_ON_FAIL(condition,val)
#else
#   if A_NULL_PASSTHROUGH
#       define PASSTHROUGH_ON_FAIL(condition,val) do { if (!(condition)) { return val; } } while (0)
#   else
#       define PASSTHROUGH_ON_FAIL(condition,val)
#   endif
#endif
/**
 * \brief The minimum codepoint possible in the Unicode codespace.
 */
/** \hideinitializer */
#define A_MIN_CP (0x000000)
/**
 * \brief The maximum codepoint possible in the Unicode codespace.
 */
/** \hideinitializer */
#define A_MAX_CP (0x10FFFF)
/**
 * \brief The maximum buffer size required to hold the longest
 *        code units sequence + a null terminator.
 */
/** \hideinitializer */
#define A_MAX_CHAR (4+1)
/**
 * \brief A typedef that should be used when dealing with aleph strings
 *        in order to distinguish them from normal NULL-terminated strings.
 */
typedef char *a_str;
/**
 * \brief A typedef that should be used when dealing with a const aleph
 *        strings in order to distinguish them from normal const NULL-terminated
 *        strings.
 */
typedef const char *a_cstr;
/**
 * \brief A typedef that represents a single Unicode code point.
 */
typedef int a_cp;
#if A_INCLUDE_MEM == 1
/**
 * \brief A struct typdef that hold a temp string pool object.
 */
typedef struct a_pool *a_pool;
#endif
/**
 * \brief A sentinel value representing EOS.
 */
/** \hideinitializer */
#define A_EOS ((size_t)-1U)
/*
 */
extern const char a_next_char_size[256];


/**
 * \anchor creation_functions
 * \name Creation and destruction
 *
 * Various functions used for creating and destructing Aleph string
 * objects, `a_str`. All a_str objects must be destroyed using one
 * of the destruction functions below such as `a_free()`.
 * 
 * Unless otherwise noted, creation functions always form valid NULL-
 * terminated UTF-8 strings that can be passed to functions that accept
 * `const char *` UTF-8 strings.
 * @{
 */
/**
 * \brief Creates a new string from a standard NULL-terminated UTF-8 string.
 * 
 * Creates a new Aleph string from the contents of \p str. If \p str is
 * NULL, a new empty string is created instead.
 * 
 * \param str A NULL-terminated UTF-8 string to initize the string to, or NULL.
 * \return a_str, otherwise NULL on failure.
 * \pre \em str must be a valid UTF-8 string, if that cannot be guaranteed,
 *      use `a_new_validate()` instead.
 */
a_str       a_new(const char *str);
/**
 * \brief Creates a new string from a specific code point repeated \p repeat times.
 * 
 * Creates a new Aleph string filled with code point \p codepoint repeated \p repeat time. 
 * 
 * \param codepoint A valid code point in the Unicode codespace. (i.e. #A_MIN_CP <= cp <= #A_MAX_CP)
 * \param repeat Number of times \p codepoint should be repeated.
 * \return A new a_str string, otherwise NULL on failure.
 */
a_str       a_new_cp(a_cp codepoint, size_t repeat);
/**
 * \brief Creates a string from a specific character repeated \p repeat times.
 * 
 * Creates a new Aleph string filled with code point \p chr repeated \p repeat time. 
 * 
 * \param chr A pointer to a valid code point encoded in UTF-8 code units.
 * \param repeat Number of times \p chr should be repeated.
 * \return A new a_str string, otherwise NULL on failure.
 * \note \p chr need not be NULL-terminated.
 */
a_str       a_new_chr(char *chr, size_t repeat);
/**
 * \brief Creates a string from a portion of a UTF-8 string.
 * 
 * Creates a new Aleph string from the first \p size bytes of
 * the UTF-8 encoded buffer \p str.
 * 
 * \param str The UTF-8 encoded string to copy from.
 * \param size The number of bytes from \p str to copy.
 * \return A new a_str string, otherwise NULL on failure.
 * \pre \p str must not be shorter than \p size.
 */
a_str       a_new_size(const char *str, size_t size);
/*
 * TODO: Incomplete!
 */
a_str       a_new_validate(const char *str);
/**
 * \brief Creates a string with a specific buffer size.
 * 
 * Creates a new empty Aleph string with a buffer size of at
 * least \p size bytes. 
 * 
 * \param size Minimum buffer size for the new string.
 * \return A new a_str string, otherwise NULL on failure.
 * \note This function adds 1 to \p size for a NULL-terminator.
 */
a_str       a_new_mem(size_t size);
/**
 * \brief Creates a new a_str of specific buffer size.
 * 
 * Creates a new Aleph string with a buffer size of at
 * least \p size bytes. Unlike `a_new_mem()`, this function
 * leaves the buffer untouched, not even NULL-terminated.
 * 
 * \param size Minimum buffer size for the new string.
 * \return A new a_str string, otherwise NULL on failure.
 */
a_str       a_new_mem_raw(size_t size);
/**
 * \brief Creates a new a_str duplicate.
 * 
 * Creates a new Aleph string duplicate of another Aleph
 * string \p str.
 * 
 * \param str An Aleph string to duplicate.
 * \return A new a_str string, otherwise NULL on failure.
 */
a_str       a_new_dup(a_cstr str);
/**
 * \brief Creates a new a_str from a long integer value.
 * 
 * Creates a new Aleph string from the long integer value
 * \p val.
 * 
 * \param val A long integer.
 * \return A new a_str string, otherwise NULL on failure.
 */
a_str       a_new_long(long val);
/**
 * \brief Creates a new a_str from an unsigned long integer value.
 * 
 * Creates a new Aleph string from the unsigned long integer value
 * \p val.
 * 
 * \param val An unsigned long integer.
 * \return A new a_str string, otherwise NULL on failure.
 */
a_str       a_new_ulong(unsigned long val);
/*
 * TODO: Incomplete!
 */
a_str       a_new_normalize(a_cstr str, int mode); /* see norm modes below */
/**
 * \brief Frees an Aleph string.
 * 
 * Frees an Aleph string.
 * 
 * \param str An Aleph string to free.
 */
void        a_free(a_str str);
/**
 * \brief Frees multiple Aleph strings.
 * 
 * Frees multiple Aleph strings passed as a
 * NULL-terminated argument list.
 * 
 * \param str The first a_str to free followed by
 *            either `NULL` or a list of a_str strings
 *            followed by `NULL`.
 */
void        a_free_n(a_str str, ...);
/**
 * \brief Frees an array of Aleph strings
 * 
 * Frees a NULL-terminated array of Aleph strings.
 * 
 * \param strv The NULL-terminated array of Aleph strings.
 * \note This function frees \p strv as well.
 */
void        a_free_vec(a_str *strv);
/*@}*/


/**
 * \anchor assignment_functions
 * \name Assignments
 *
 * Various functions used to replace the value of the string with a
 * new value.
 * @{
 */
/**
 * \brief Clears a string.
 * 
 * Clears an Aleph string, setting its length to zero.
 * 
 * \param str The Aleph string to be cleared.
 * \return \p str.
 */
a_str       a_clear(a_str str);
/**
 * \brief Sets the content of a string to the content of another string.
 * 
 * Sets the content of \p str to the content of \p newstr.
 * 
 * \param str The destination string.
 * \param newstr The source string.
 * \return \p str or a new a_str pointer; otherwise NULL on failure.
 */
a_str       a_set(a_str str, a_str newstr);
/**
 * \brief Sets the content of a string to the content of another string.
 * 
 * Sets the content of \p str to the content of the NULL-terminated UTF-8
 * encoded string, \p newstr.
 * 
 * \param str The destination string.
 * \param newstr The source string.
 * 
 * \return \p str or a new a_str pointer; otherwise NULL on failure.
 * \pre \em newstr must be a valid UTF-8 string, if that cannot be
 *          guaranteed, use `a_set_cstr_validate()` instead.
 */
a_str       a_set_cstr(a_str str, const char *newstr);
/**
 * \brief Sets the content of a string to the content of another string.
 * 
 * Sets the content of \p str to the content of the NULL-terminated UTF-8
 * encoded string, \p newstr. The content of \p str is then validated
 * following the rules defined in function `a_validate()`.
 * 
 * \param str The destination string.
 * \param newstr The source string.
 * 
 * \return \p str or a new a_str pointer; otherwise NULL on failure.
 * \note This function should be used over `a_set_cstr()` when the input
 *       cannot be guaranteed to be a valid UTF-8 string.
 */
a_str       a_set_cstr_validate(a_str str, const char *newstr);
/**
 * \brief Sets the content of a string to a portion of another string.
 * 
 * Sets the content of \p str to the first \p size bytes of the NULL-
 * terminated UTF-8 encoded string, \p newstr.
 * 
 * \param str The destination string.
 * \param newstr The source string.
 * \param size Number of bytes to copy from newstr to str.
 * 
 * \return \p str or a new a_str pointer; otherwise NULL on failure.
 * \pre \em newstr must be a valid UTF-8 string, if that cannot be
 *          guaranteed, use `a_set_cstr_validate()` instead.
 */
a_str       a_set_cstr_size(a_str str, const char *newstr, size_t size);
/*@}*/


/**
 * \anchor index_functions
 * \name Index
 *
 * Various functions used to access individual characters.
 * @{
 */
/**
 * \brief Returns the code point at a specific index.
 * 
 * Returns the code point at index \p index of string \p str.
 * 
 * \param str The string in context.
 * \param index The index of the code point.
 * 
 * \return The code point stored in \p str at position \p index.
 * \note This function should \b NOT be used to iterate over a string
 *       as it seeks the \p index 'th position from the start of the
 *       string each time. (I.E. O(n) complexity). To iterate over a
 *       string efficiently see the \ref Iterator functions section.
 */
a_cp        a_char_at(a_cstr str, size_t index);
/**
 * 
 */
size_t      a_char_offset(a_cstr str, size_t index);
size_t      a_char_offset_cstr(const char *str, size_t index);
size_t      a_char_index(a_cstr str, size_t offset);
size_t      a_char_index_cstr(const char *str, size_t offset);
/**
 * 
 */
size_t      a_char_offset_rev(a_cstr str, size_t index);
size_t      a_char_offset_rev_cstr(const char *str, size_t index);
size_t      a_char_index_rev(a_cstr str, size_t offset);
size_t      a_char_index_rev_cstr(const char *str, size_t offset);
/**
 * \brief Returns the start of the grapheme cluster at a specific index.
 * 
 * This function returns a pointer to the first byte of the grapheme cluster
 * that starts at index \p index of string \p str.
 * 
 * \param str The string in context.
 * \param index The index of the code point.
 * 
 * \return A pointer to the first byte of the grapheme cluster.
 * \note This function should \b NOT be used to iterate over a string
 *       as it seeks the \p index 'th position from the start of the
 *       string each time. (I.E. O(n) complexity). To iterate over a
 *       string efficiently see the \ref iterator_functions Iterator functions section.
 */
char       *a_gchar_at(a_str str, size_t index);
/*@}*/


/**
 * \anchor iterator_functions
 * \name Iterator
 *
 * Various functions used to traverse UTF-8 encoded strings.
 * 
 * The idiomatic syntax for traversing strings are:
 * 
 * \includelineno iterator.c
 * 
 * It's the programmers responsibility to determin when the end
 * of the string is reached. (I.E. check for the NULL-terminator.)
 * 
 * @{
 */
/**
 * \brief Peeks at the next code point.
 * 
 * This function peek at the next code point in the string
 * and return its value.
 * 
 * \param str The string to peek at.
 * 
 * \return The value of the next code point in the string.
 * \pre \p str must be a valid UTF-8 string.
 */
a_cp        a_peek(const char *str);
a_cp        a_peek_prev(const char *s);
/**
 * \brief Advances one code point.
 * 
 * This function advances the string pointed by \p str by a single code
 * point and returns a pointer to the first code unit of the next code
 * point in the string.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return A pointer to the first code unit of the next code point of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_prev()
 */
char       *a_next(char **str);
/**
 * \brief Advances one code point.
 * 
 * This function advances the string pointed by \p str by a single code
 * point and returns a pointer to the first code unit of the next code
 * point in the string.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return A pointer to the first code unit of the next code point of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_prev_cstr()
 */
char       *a_next_cstr(const char **str);
/**
 * \brief Advences to the next code point and returns its value.
 * 
 * This function returns the first code point value of the string pointed
 * by \p str, while advencing the pointer pointed by \p str by a single code
 * point.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return The code point value of the first code point of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_prev_cp()
 */
a_cp        a_next_cp(char **str);
/**
 * \brief Advences to the next code point and returns its value.
 * 
 * This function returns the first code point value of the string pointed
 * by \p str, while advencing the pointer pointed by \p str by a single code
 * point.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return The code point value of the first code point of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_prev_cp_cstr()
 */
a_cp        a_next_cp_cstr(const char **str);
/**
 * \brief Advences to the next grapheme cluster. 
 * 
 * This function advences the pointer pointed by \p str to the next grapheme
 * cluster. The pointer is then return.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return A pointer to the first code unit of the next grapheme cluster of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 */
char       *a_gnext(char **str);
/**
 * \brief Advences to the next grapheme cluster.
 * 
 * This function advences the pointer pointed by \p str to the next grapheme
 * cluster. The pointer is then return.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return A pointer to the first code unit of the next grapheme cluster of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 */
char       *a_gnext_cstr(const char **str);
/**
 * \brief Advances one code point.
 * 
 * This macro increments a `const char *` by a single code point.
 * 
 * \param str The string to operate on.
 * 
 * \return The incremented \p str pointer.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \warning This macro DOES expands to code that evalutes \p str TWICE.
 * \see a_next()
 */
/** \hideinitializer */
#define     a_next_chr(str) ((char*)((str) + a_next_char_size[(int)*(unsigned char*)(str)]))
/**
 * \brief Returns a pointer to the end of the string.
 * 
 * This function returns a pointer to '\0' code unit located at
 * the end of the string pointed by \p str.
 * 
 * \param str The string in context.
 * \return a pointer to the end of string \p str.
 */
char       *a_end(a_str str);
/**
 * \brief Returns a pointer to the end of the string.
 * 
 * This function returns a pointer to '\0' code unit located at
 * the end of the string pointed by \p str.
 * 
 * \param str The string in context.
 * \return a pointer to the end of string \p str.
 * \pre \p str must be a valid UTF-8 string.
 */
char       *a_end_cstr(const char *str);
/**
 * \brief Returns a pointer to the last code point of the string.
 * 
 * This function returns a pointer to the first code unit of the
 * last code point of the string pointed by \p str.
 * 
 * \param str The string in context.
 * \return a pointer to the last code point of string \p str.
 */
char       *a_last(a_str str);
/**
 * \brief Returns a pointer to the last code point of the string.
 * 
 * This function returns a pointer to the first code unit of the
 * last code point of the string pointed by \p str.
 * 
 * \param str The string in context.
 * \return a pointer to the last code point of string \p str.
 * \pre \p str must be a valid UTF-8 string.
 */
char       *a_last_cstr(const char *str);
/**
 * \brief Advances to the previous code point.
 * 
 * This function advances the string pointed by \p str by a single code
 * point backwards and returns a pointer to that position.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return A pointer to the first code unit of the previous code point of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_next()
 */
char       *a_prev(char **str);
/**
 * \brief Advances to the previous code point.
 * 
 * This function advances the string pointed by \p str by a single code
 * point backwards and returns a pointer to that position.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return A pointer to the first code unit of the previous code point of \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_next_cstr()
 */
char       *a_prev_cstr(const char **str);
/**
 * \brief Advences to the previous code point and returns its value.
 * 
 * This function advences the pointer pointed by \p str by a single code
 * point to the previous code point and returns its value.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return The code point value of the previous code point pointed by \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_next_cp()
 */
a_cp        a_prev_cp(char **str);
/**
 * \brief Advences to the previous code point and returns its value.
 * 
 * This function advences the pointer pointed by \p str by a single code
 * point to the previous code point and returns its value.
 * 
 * \param str A pointer to the string to operate on.
 * 
 * \return The code point value of the previous code point pointed by \p str.
 * \pre \p str must be a pointer to a valid UTF-8 string.
 * \see a_next_cp_cstr()
 */
a_cp        a_prev_cp_cstr(const char **str);
#ifdef A_ITERATOR
char       *a_it_at(a_str s);
char       *a_it_next(a_str s);
char       *a_it_prev(a_str s);
char       *a_it_begin(a_str s);
char       *a_it_end(a_str s);
int         a_it_at_end(a_cstr s);
int         a_it_at_start(a_cstr s);
#endif
/*@}*/


/** 
 * \anchor validation_functions
 * \name Validation
 *
 * Various functions used to validate UTF-8 encoded strings.
 * @{
 */
const char *a_is_valid_utf8(const char *cstr);
a_str       a_validate(a_str str);
/*@}*/


/** 
 * \anchor debug_functions
 * \name Debug
 *
 * Various functions used to print useful info about an a_str object 
 * or a specific code point.
 * @{
 */
void        a_dump(a_str str, int val);
void        a_dump_cp(a_cp cp);
/*@}*/


/**
 * \anchor length_functions
 * \name Length and Size
 *
 * Various functions used to retrive or calculate the length of a
 * string in bytes, code points, and grapheme clusters.
 * @{
 */
size_t      a_len(a_cstr str);
size_t      a_len_cstr(const char *str);
size_t      a_len_cstr_max(const char *str, size_t max);
size_t      a_size(a_cstr s);
/** \hideinitializer */
#define     a_size_chr(c) ((a_next_char_size[(int)(unsigned char)c]))
/** \hideinitializer */
#define     a_size_chr_cstr(s) (a_size_chr((unsigned char)*s))
size_t      a_gsize_chr_cstr(const char *str);
size_t      a_glen(a_cstr str);
size_t      a_glen_cstr(const char *s);
size_t      a_mem(a_cstr str);
int         a_is_empty(a_cstr str);
/*@}*/


/**
 * \anchor concatenation_functions
 * \name Concatenation
 *
 * Various functions used to concatenate values onto an Aleph string.
 * 
 * \note It's important to note that no normalization form is closed
 *       under string concatenation. (I.E. for strings `X` and `Y`,
 *       normalized in same form Z; cat(X,Y) need not be normalized,
 *       for all forms.) For such situations, only use a_cat_norm().
 * @{
 */
a_str       a_cat(a_str str, a_str str2);
a_str       a_cat_cstr(a_str str, const char *str2);
a_str       a_cat_chr(a_str str, const char *chr);
a_str       a_cat_str(a_str str, a_str str2);
a_str       a_cat_len(a_str str, const char *str2, size_t l);
a_str       a_cat_cp(a_str str, a_cp codepoint);
a_str       a_cat_norm(a_str str, a_str str2, int mode);
a_str       a_cat_ulong(a_str str, unsigned long val);
a_str       a_cat_long(a_str str, long val);
/*@}*/


/**
 * \anchor insertion_functions
 * \name Insertion
 *
 * LibAleph provides a set of utilities of inserting a string at
 * a specific position of another string. Insertion can be done
 * based on the code point index, grapheme cluster index, or simply
 * based on the offset in the string.
 * 
 * @{
 */
a_str       a_ins(a_str str, a_cstr str2, size_t index);
a_str       a_ins_chr(a_str str, const char *chr, size_t index);
a_str       a_ins_cstr(a_str str, const char *str2, size_t index);
a_str       a_ins_cp(a_str str, a_cp codepoint, size_t index);
a_str       a_ins_offset(a_str str, a_cstr str2, size_t offset);
a_str       a_ins_offset_chr(a_str str, const char *chr, size_t offset);
a_str       a_ins_offset_cstr(a_str str, const char *str2, size_t offset);
a_str       a_ins_offset_cp(a_str str, a_cp codepoint, size_t offset);
a_str       a_gins(a_str str, a_cstr str2, size_t index);
a_str       a_gins_chr(a_str str, const char *chr, size_t index);
a_str       a_gins_cstr(a_str str, const char *str2, size_t index);
a_str       a_gins_cp(a_str str, a_cp codepoint, size_t index);
/*@}*/


/** 
 * \anchor replacement_functions
 * \name Replacement
 *
 * Various ways of inserting values into an Aleph string.
 * @{
 */
a_str       a_rep(a_str str, a_cstr oldstr, a_cstr newstr);
a_str       a_rep_cstr_cstr(a_str str, const char *oldstr, const char *newstr);
a_str       a_rep_cp(a_str str, a_cp oldcp, a_cp newcp);
a_str       a_irep(a_str str, a_cstr oldstr, a_cstr newstr);
a_str       a_irep_cstr_cstr(a_str str, const char *oldstr, const char *newstr);
a_str       a_irep_cp(a_str str, a_cp oldcp, a_cp newcp);
a_str       a_rep_all(a_str str, a_cstr oldstr, a_cstr newstr);
a_str       a_rep_all_cstr_cstr(a_str str, const char *oldstr, const char *newstr);
a_str       a_rep_all_cp(a_str str, a_cp oldcp, a_cp newcp);
a_str       a_irep_all(a_str str, a_cstr oldstr, a_cstr newstr);
a_str       a_irep_all_cstr_cstr(a_str str, const char *oldstr, const char *newstr);
a_str       a_rep_all_cp(a_str str, a_cp oldcp, a_cp newcp);
/*@}*/


/**
 * \anchor deletion_functions
 * \name Deletion
 * 
 * Various ways of deleting characters from a string
 * @{
 */
/**
 * \brief Deletes a portion of the string.
 * 
 * This function deletes \p length code points from \p str starting
 * at \p start.
 * 
 * \param str A pointer to the string to operate on.
 * \param start The starting code point index
 * \param length The number of code points to delete.
 * 
 * \return \p str with the specified portion deleted.
 */
a_str       a_del(a_str str, size_t start, size_t length);
a_str       a_del_offset(a_str str, size_t start, size_t length);
/*@}*/


/**
 * \anchor splitting_functions
 * \name Splitting
 *
 * Functions used to split a string with various options.
 * @{
 */
/* splitting *//*
a_str      *a_split(a_cstr str, a_cstr delimiters);
a_str      *a_split_count(a_cstr str, a_cstr delimiters, size_t *count);
a_str      *a_split_cstr(a_cstr str, const char *delimiters);
a_str      *a_split_cstr_cstr(const char *str, const char *delimiters);
a_str      *a_split_cp(a_cstr str, a_cp codepoint);
a_str      *a_split_opts(const char *str, const char *delimiters, size_t *count, int options);*/
a_str      *a_split(const char *str, a_cp codepoint);
a_str      *a_split_cstr(const char *str, a_cp codepoint);
a_str      *a_split_delim_opts(const char *str, const char *delimiters, size_t *count, int options);
a_str      *a_split_delim_count(const char *str, const char *delimiters, size_t *count);
a_str      *a_split_delim(const char *str, const char *delimiters);
enum a_split_options
{
    a_split_no_options   = 0x00,
    a_split_remove_empty = 0x01,
    a_split_trim_tokens  = 0x02
};
/*@}*/


/** 
 * \anchor joining_functions
 * \name Joining
 *
 * Functions used to join multiple Aleph strings together
 * @{
 */
a_str       a_join(a_str str, ...);
a_str       a_join_on(a_str str, a_cstr glue, ...);
a_str       a_join_on_cstr(a_str str, const char *glue, ...);
a_str       a_join_on_opts(a_str str, int opts, ...); /* skip blanks, etc.. */
/*@}*/


/**
 * \anchor buffer_management_functions
 * \name Buffer Size Management
 *
 * Various functions used to manage the internal buffer size
 * of an a_str object.
 * @{
 */
a_str       a_reserve(a_str str, size_t l);
a_str       a_ensure(a_str str, size_t l);
/*@}*/


/**
 * \anchor substring_counting_functions
 * \name Substring Counting
 *
 * Various functions used to count the number of delimiters/substrings
 * found in the original string.
 * @{
 */
size_t      a_count(a_cstr str, a_cstr delmiters);
size_t      a_count_cstr(a_cstr str, const char *delmiters);
size_t      a_count_cstr_cstr(const char *str, const char *delmiters);
size_t      a_count_cp(a_cstr str, a_cp codepoint);
size_t      a_count_cstr_cp(const char *str, a_cp codepoint);
size_t      a_count_substr(a_cstr str, a_cstr substr);
size_t      a_count_substr_cstr(a_cstr str, const char *substr);
size_t      a_count_substr_cstr_cstr(const char *str, const char *substr);

size_t      a_icount_substr(a_cstr str, a_cstr substr);
size_t      a_icount_substr_cstr(a_cstr str, const char *substr);
size_t      a_icount_substr_cstr_cstr(const char *str, const char *substr);
/*@}*/


/** 
 * \anchor substring_extraction_functions
 * \name Substring Extraction
 *
 * Various functions used to extract a substring from a string.
 * @{
 */
a_str       a_substr(a_cstr str, long start, long length);
a_str       a_substr_inplace(a_str str, long start, long length);
a_str       a_substr_offset(a_cstr str, long start, long length);
a_str       a_substr_offset_inplace(a_str str, long start, long length);
a_str       a_substr_between_cstr(a_cstr str, const char *sub1, const char *sub2, int inclusive);
a_str       a_substr_between_cstr_inplace(a_str str, const char *sub1, const char *sub2, int inclusive);
a_str       a_left(a_cstr str, size_t length);
a_str       a_left_offset(a_cstr str, size_t length);
a_str       a_left_offset_inplace(a_str str, size_t length);
a_str       a_right(a_cstr str, size_t length);
a_str       a_right_offset(a_cstr str, size_t length);
a_str       a_right_offset_inplace(a_str str, size_t length);
/*@}*/


/** 
 * \anchor trimming_functions
 * \name Trimming
 *
 * Various functions used to trim a string.
 * @{
 */
a_str       a_trim(a_str str);
a_str       a_trim_cstr(a_str str, const char *chars);
a_str       a_trim_left(a_str str);
a_str       a_trim_left_cstr(a_str str, const char *chars);
a_str       a_trim_right(a_str str);
a_str       a_trim_right_cstr(a_str str, const char *chars);
/*@}*/


/** 
 * \anchor matching_functions
 * \name Matching
 *
 * Various functions used in comparing two strings fully or partially.
 * @{
 */
int         a_cmp(a_cstr str1, a_cstr str2);
int         a_cmp_cstr(a_cstr str1, const char *str2);
int         a_cmp_cstr_cstr(const char *str, const char *str2);
int         a_icmp(a_cstr str1, a_cstr str2);
int         a_icmp_cstr(a_cstr str1, const char *str2);
int         a_icmp_cstr_cstr(const char *str1, const char *str2);
int         a_icmp_simple(a_cstr str1, a_cstr str2);
int         a_icmp_simple_cstr(a_cstr str1, const char *str2);
int         a_icmp_simple_cstr_cstr(const char *str1, const char *str2);
int         a_icmp_min(a_cstr str1, a_cstr str2);
int         a_icmp_min_cstr(a_cstr str1, const char *str2);
int         a_icmp_min_cstr_cstr(const char *str1, const char *str2);
int         a_icmp_min_cstr_len(a_cstr str1, const char *str2, size_t *match_len);
int         a_icmp_min_cstr_cstr_len(const char *str1, const char *str2, size_t *match_len);
int         a_icmpn(a_cstr str1, a_cstr str2, size_t n);
int         a_icmpn_cstr(a_cstr str1, const char *str2, size_t n);
int         a_icmpn_cstr_cstr(const char *str1, const char *str2, size_t n);/*
int         a_cmp_canonical(a_cstr str1, a_cstr str2);
int         a_cmp_canonical_cstr(a_cstr str1, const char *str2);
int         a_cmp_canonical_cstr_cstr(const char *str1, const char *str2);
int         a_cmp_wild(const char *str, const char *pattern);
int         a_cmp_regex(const char *str, const char *pattern);
int         a_icmp_canonical(a_cstr str1, a_cstr str2);
int         a_icmp_canonical_cstr(a_cstr str1, const char *str2);
int         a_icmp_canonical_cstr_cstr(const char *str1, const char *str2);
int         a_icmp_compatibility(a_cstr str1, a_cstr str2);
int         a_icmp_compatibility_cstr(a_cstr str1, const char *str2);
int         a_icmp_compatibility_cstr_cstr(const char *str1, const char *str2);
int         a_icmp_wild(const char *str, const char *pattern);*/
/*@}*/


/** 
 * \anchor search_functions
 * \name Searching
 *
 * Various functions used to search a string.
 * @{
 */
size_t      a_find(a_cstr str, a_cstr substr);
size_t      a_find_cstr(a_cstr str, const char *substr);
size_t      a_find_cstr_cstr(const char *str, const char *substr);
size_t      a_find_cp(a_cstr str, a_cp codepoint);
size_t      a_find_from(a_cstr str, a_cstr substr, size_t index);
size_t      a_find_from_cstr(a_cstr str, const char *substr, size_t index);/*
size_t      a_find_from_cp(a_cstr str, a_cp codepoint, size_t index);*/
size_t      a_find_offset(a_cstr str, a_cstr substr);
size_t      a_find_offset_cstr(a_cstr str, const char *substr);/*
size_t      a_find_offset_cp(a_cstr str, a_cp codepoint);*/
size_t      a_find_offset_from(a_cstr str, a_cstr substr, size_t offset);
size_t      a_find_offset_from_cstr(a_cstr str, const char *substr, size_t offset);/*
size_t      a_find_offset_from_cp(a_cstr str, a_cp codepoint, size_t offset);
size_t      a_rfind(a_cstr str, a_cstr substr);
size_t      a_rfind_cstr(a_cstr str, const char *substr);
size_t      a_rfind_cp(a_cstr str, a_cp codepoint);
size_t      a_rfind_from(a_cstr str, a_cstr substr, size_t index);
size_t      a_rfind_from_cstr(a_cstr str, const char *substr, size_t index);
size_t      a_rfind_from_cp(a_cstr str, a_cp codepoint, size_t index);
size_t      a_rfind_offset(a_cstr str, a_cstr substr);
size_t      a_rfind_offset_cstr(a_cstr str, const char *substr);
size_t      a_rfind_offset_cp(a_cstr str, a_cp codepoint);
size_t      a_rfind_offset_from(a_cstr str, a_cstr substr, size_t offset);
size_t      a_rfind_offset_from_cstr(a_cstr str, const char *substr, size_t offset);
size_t      a_rfind_offset_from_cp(a_cstr str, a_cp codepoint, size_t offset);*/

size_t      a_ifind(a_cstr str, a_cstr substr);
size_t      a_ifind_cstr(a_cstr str, const char *substr);
size_t      a_ifind_cstr_cstr(const char *str, const char *substr);
size_t      a_ifind_from(a_cstr str, a_cstr substr, size_t index);
size_t      a_ifind_from_cstr(a_cstr str, const char *substr, size_t index);
size_t      a_ifind_from_cp(a_cstr str, a_cp codepoint, size_t index);
size_t      a_ifind_from_len(a_cstr str, a_cstr substr, size_t index, size_t *match_len);
size_t      a_ifind_from_len_cstr(a_cstr str, const char *substr, size_t index, size_t *match_len);/*
size_t      a_ifind_offset(a_cstr str, a_cstr substr);
size_t      a_ifind_offset_cstr(a_cstr str, const char *substr);
size_t      a_ifind_offset_cp(a_cstr str, a_cp codepoint);*/
size_t      a_ifind_offset_from(a_cstr str, a_cstr substr, size_t offset);
size_t      a_ifind_offset_from_cstr(a_cstr str, const char *substr, size_t offset);/*
size_t      a_ifind_offset_from_cp(a_cstr str, a_cp codepoint, size_t offset);*/
size_t      a_ifind_offset_from_len(a_cstr str, a_cstr substr, size_t offset, size_t *match_len);
size_t      a_ifind_offset_from_len_cstr(a_cstr str, const char *substr, size_t offset, size_t *match_len);/*
size_t      a_irfind(a_cstr str, a_cstr substr);
size_t      a_irfind_cstr(a_cstr str, const char *substr);
size_t      a_irfind_cp(a_cstr str, a_cp codepoint);
size_t      a_irfind_from(a_cstr str, a_cstr substr, size_t index);
size_t      a_irfind_from_cstr(a_cstr str, const char *substr, size_t index);
size_t      a_irfind_from_cp(a_cstr str, a_cp codepoint, size_t index);
size_t      a_irfind_offset(a_cstr str, a_cstr substr);
size_t      a_irfind_offset_cstr(a_cstr str, const char *substr);
size_t      a_irfind_offset_cp(a_cstr str, a_cp codepoint);
size_t      a_irfind_offset_from(a_cstr str, a_cstr substr, size_t offset);
size_t      a_irfind_offset_from_cstr(a_cstr str, const char *substr, size_t offset);
size_t      a_irfind_offset_from_cp(a_cstr str, a_cp codepoint, size_t offset);*/
/*@}*/


/** 
 * \anchor start_end_cmp_functions
 * \name Start/End comparison
 *
 * Various functions used to test the end or start of a string against
 * a substring.
 * @{
 */
int         a_startswith(a_cstr str, a_cstr substr);
int         a_startswith_cstr(a_cstr str, const char *substr);
int         a_startswith_norm(a_str str, a_str substr);
int         a_endswith(a_cstr str, a_str substr);
int         a_endswith_cstr(a_cstr str, const char *substr);
int         a_endswith_cstr_cstr(const char *str, const char *substr);
int         a_endswith_norm(a_str str, a_str substr);

int         a_istartswith(a_cstr str, a_cstr substr);
int         a_istartswith_cstr(a_cstr str, const char *substr);
int         a_istartswith_norm(a_str str, a_str substr);
int         a_iendswith(a_cstr str, a_str substr);
int         a_iendswith_cstr(a_cstr str, const char *substr);
int         a_iendswith_cstr_cstr(const char *str, const char *substr);
int         a_iendswith_norm(a_str str, a_str substr);
/*@}*/


/** 
 * \anchor reversal_functions
 * \name Reversal
 *
 * Functions used to perfrom string reversal.
 * 
 * Various semitic languages such as Hebrew and Arabic are
 * written from right to left. Older systems often times could not
 * handle or didn't support reordering those languages visually.
 * Because of this such documents were encoded in visual order (i.e.
 * the first letter of a line is the rightmost one)
 * 
 * (see: Hebrew: ISO/IEC 8859-8 / Arabic: ISO/IEC 8859-6)
 * 
 * Unicode, however defines characters to be in logical order (i.e.
 * the order you read the string). Reversal functions can be used to
 * reverse a segment of R-L text into logical ordering.
 * 
 *     a_str example = a_new("שָׁלוֹם");
 *     a_str example2 = a_greverse_new(example);
 *     
 *     printf("=> '%s'\n",  example);
 *     printf("=> '%s'\n",  a_reverse(example));
 *     printf("=> '%s'\n",  example2);
 *     
 *     a_free_n(example, example2, NULL);
 * 
 * Prints:
 * 
 *      => 'שָׁלוֹם'
 *      => 'םֹולָׁש'
 *      => 'םוֹלשָׁ'
 *
 * 
 * @{
 */
a_str       a_reverse(a_str str);
a_str       a_reverse_new(a_cstr str);
a_str       a_reverse_str(a_cstr str, a_str output);
a_str       a_reverse_paragraph(a_str str);
a_str       a_greverse(a_str str);
a_str       a_greverse_new(a_cstr str);
a_str       a_greverse_str(a_cstr str, a_str output);
/*@}*/


/** 
 * \anchor escaping_functions
 * \name Escaping/Unescaping
 *
 * Functions used to transform a string into an escaped string and
 * vice versa.
 * 
 * @{
 */
a_str       a_escape(a_str str); /* escape '\b', '\f', '\n', '\r', '\t', '\v', '\' and '"'; 0x01-0x1F, and 0x7F-0xFF  */
a_str       a_escape_except(a_str str, const char *except);
a_str       a_unescape(a_str str);
/*@}*/



#if A_INCLUDE_IO == 1
/**
 * \anchor io_functions
 * \name Input/Output handling
 *
 * Functions used in reading and writing to a FILE object.
 * 
 * @{
 */
a_str       a_file_read(FILE *fp);
a_str       a_file_readline(FILE *fp);
a_str       a_file_readline_delim(FILE *fp, const char *delim);
a_str       a_file_readline_delim_str(FILE *fp, const char *delim, a_str str);
size_t      a_file_line_count(FILE *fp);
#endif
/*@}*/


#if A_INCLUDE_MEM == 1 || defined(DOXYGEN_DOCS)
/** 
 * \anchor gc_functions
 * \name Garbage Collection
 *
 * A set of functions to facilitate a light weight string release pool
 * that can be used to collect strings and free them all at once at a
 * later period.
 * 
 * @{
 */
a_str       a_gc_collect(a_pool *pool, a_str str);
void        a_gc_free(a_pool pool);
a_pool      a_gc_new();
#define     a_gc a_pool a_tmp_pool = a_gc_new()
#define     a_gc_done() a_gc_free(a_tmp_pool)
#define     a_(s) a_gc_collect(&a_tmp_pool, s)
#define     A_(p, s) a_gc_collect(&p, s)
/*@}*/
#endif


#if ALEPH_C_V == 2
/** \name Formatting
 *
 * ...
 * 
 * @{
 */
a_str       a_format(a_str str, const char *format, ...);
/*@}*/
#endif

/** 
 * \anchor transformation_functions
 * \name Transformation Functions
 * 
 * @{
 */
a_cp        a_to_cp(const char *str);
const char *a_to_utf8(a_cp codepoint, char *buffer);
const char *a_to_utf8_size(a_cp codepoint, char *buffer, int *size);
/*@}*/

/** 
 * \anchor case_functions
 * \name Case Detection & Manipulation
 *
 * Unicode offers 3 forms of casing: uppercase, titlecase, and
 * lowercase. Additionally, case folding is also offered for case-
 * insensitive comparisons. Additionally, characters may also be 
 * uncased - simultaneously lowercase, uppercase, and titlecase.
 * 
 * The main functions (a_to_upper/a_to_lower/etc.) use full case
 * conversion when transforming strings, this means both the size
 * and the length of the string may change.
 * 
 * Assumptions:
 * 
 *      As with most things Unicode-related, many assumptions must
 *      be thrown out the window.
 * 
 *      * The size of the string may change during any transformation.
 * 
 *      * The length of the string may change during full case
 *        conversion.
 * 
 *      * Transforms are irreversible operations.
 * 
 * @{
 */
/**
 * \brief Transforms the string to uppercase.
 * 
 * This function transforms all the letters in the string to
 * uppercase (also known as capital or majuscule) using full
 * uppercase conversion.
 * 
 * \note The new string need not have the same size nor length
 *       as the original string. Additionally, this operation is
 *       irreversible.
 * 
 * \param str The string in context.
 * \return \p str uppercased; otherwise NULL on failure.
 * \see a_to_lower()
 */
a_str       a_to_upper(a_str str);
char       *a_to_upper_cp(a_cp cp, char *b);
a_cp       *a_to_upper_cp_cp(a_cp cp, a_cp *b);
a_str       a_to_upper_simple(a_str str);
char       *a_to_upper_simple_chr(a_cp cp, char *b);
a_cp        a_to_upper_simple_chr_cp(a_cp cp);
/**
 * \brief Transforms the string to lowercase.
 * 
 * This function transforms all the letters in the string to
 * lowercase (also known as small or minuscule) using full
 * lowercase conversion.
 * 
 * \note The new string need not have the same size nor length
 *       as the original string. Additionally, this operation is
 *       irreversible.
 * 
 * \param str The string in context.
 * \return \p str lowercased; otherwise NULL on failure.
 * \see a_to_upper()
 */
a_str       a_to_lower(a_str str);
char       *a_to_lower_cp(a_cp cp, char *b);
a_cp       *a_to_lower_cp_cp(a_cp cp, a_cp *b);
a_str       a_to_lower_simple(a_str str);
char       *a_to_lower_simple_chr(a_cp cp, char *b);
a_cp        a_to_lower_simple_chr_cp(a_cp cp);
/**
 * \brief Transforms the string to titlecase.
 * 
 * This function transforms all the letters in the string to
 * titlecase using full uppercase and lowercase conversions.
 * 
 * \note The new string need not have the same size nor length
 *       as the original string. Additionally, this operation is
 *       irreversible.
 * 
 * \param str The string in context.
 * \return \p str titlecased; otherwise NULL on failure.
 * \see a_to_upper()
 * \see a_to_lower()
 */
a_str       a_to_title(a_str str);
char       *a_to_title_cp(a_cp cp, char *b);
a_cp       *a_to_title_cp_cp(a_cp cp, a_cp *b);
a_str       a_to_title_simple(a_str str);
char       *a_to_title_simple_chr(a_cp cp, char *b);
a_cp        a_to_title_simple_chr_cp(a_cp cp);

a_str       a_to_fold(a_str str);
char       *a_to_fold_cp(a_cp cp, char *b);
a_cp       *a_to_fold_cp_cp(a_cp cp, a_cp *b);
char       *a_to_fold_simple_chr(a_cp cp, char *b);
a_cp        a_to_fold_simple_chr_cp(a_cp cp);

int         a_is_title(const char *str);
int         a_is_title_cp(a_cp codepoint);
int         a_is_lower(const char *str);
int         a_is_lower_cp(a_cp codepoint);
int         a_is_upper(const char *str);
int         a_is_upper_cp(a_cp codepoint);
int         a_is_folded(const char *str);
int         a_is_folded_cp(a_cp codepoint);
/*@}*/


#if A_INCLUDE_NAMES == 1 || defined(DOXYGEN_DOCS)
/** 
 * \anchor character_name_functions
 * \name Names 
 *
 * ...
 * 
 * @{
 */
/**
 * \brief Size for a buffer large enough to store
 *        any Unicode character name. (and a \0)
 */
#   define A_NAME_MAX_SIZE 100
char       *a_name_cp(a_cp codepoint, char *buff);
/*@}*/
#endif


/**
 * \anchor categories_functions
 * \name Categories
 *
 * ...
 * 
 * @{
 */
int         a_ascii_is_alpha(a_cp codepoint);
int         a_ascii_is_digit(a_cp codepoint);
int         a_ascii_is_xdigit(a_cp codepoint);
int         a_ascii_is_odigit(a_cp codepoint);
int         a_ascii_is_bdigit(a_cp codepoint);
int         a_ascii_is_alnum(a_cp codepoint);
int         a_ascii_is_blank(a_cp codepoint);
int         a_ascii_is_cntrl(a_cp codepoint);
int         a_ascii_is_graph(a_cp codepoint);
int         a_ascii_is_lower(a_cp codepoint);
int         a_ascii_is_upper(a_cp codepoint);
int         a_ascii_is_print(a_cp codepoint);
int         a_ascii_is_punct(a_cp codepoint);
int         a_ascii_is_space(a_cp codepoint);

int         a_is_alpha(a_cp codepoint);
int         a_is_alphanumeric(a_cp codepoint);
int         a_is_bmp(a_cp codepoint);
int         a_is_control(a_cp codepoint);
int         a_is_currency(a_cp codepoint);
int         a_is_digit(a_cp codepoint);
int         a_is_format(a_cp codepoint);
int         a_is_graphical(a_cp codepoint);
int         a_is_letter(a_cp codepoint);
int         a_is_mark(a_cp codepoint);
int         a_is_noncharacter(a_cp codepoint);
int         a_is_number(a_cp codepoint);
int         a_is_private_use(a_cp codepoint);
int         a_is_punctuation(a_cp codepoint);
int         a_is_separator(a_cp codepoint);
int         a_is_space(a_cp codepoint);
int         a_is_supplementary(a_cp codepoint);
int         a_is_surrogate(a_cp codepoint);
int         a_is_surrogate_high(a_cp codepoint);
int         a_is_surrogate_low(a_cp codepoint);
int         a_is_symbol(a_cp codepoint);
int         a_is_math(a_cp codepoint);
int         a_category(a_cp codepoint);
const char *a_category_to_str(int category);
const char *a_category_to_description(int category);
int         a_category_from_str(const char *category);
enum a_general_categories
{
    a_gc_lu                         = 1 << 0 ,
    a_gc_letter_uppercase           = a_gc_lu,
    a_gc_ll                         = 1 << 1 ,
    a_gc_letter_lowercase           = a_gc_ll,
    a_gc_lt                         = 1 << 2 ,
    a_gc_letter_titlecase           = a_gc_lt,
    a_gc_lm                         = 1 << 3 ,
    a_gc_letter_modifier            = a_gc_lm,
    a_gc_lo                         = 1 << 4 ,
    a_gc_letter_other               = a_gc_lo,
    a_gc_mn                         = 1 << 5 ,
    a_gc_mark_nonspacing            = a_gc_mn,
    a_gc_mc                         = 1 << 6 ,
    a_gc_mark_spacing_combining     = a_gc_mc,
    a_gc_me                         = 1 << 7 ,
    a_gc_mark_enclosing             = a_gc_me,
    a_gc_nd                         = 1 << 8 ,
    a_gc_number_decimal_digit       = a_gc_nd,
    a_gc_nl                         = 1 << 9 ,
    a_gc_number_letter              = a_gc_nl,
    a_gc_no                         = 1 << 10,
    a_gc_number_other               = a_gc_no,
    a_gc_pc                         = 1 << 11,
    a_gc_punctuation_connector      = a_gc_pc,
    a_gc_pd                         = 1 << 12,
    a_gc_punctuation_dash           = a_gc_pd,
    a_gc_ps                         = 1 << 13,
    a_gc_punctuation_open           = a_gc_ps,
    a_gc_pe                         = 1 << 14,
    a_gc_punctuation_close          = a_gc_pe,
    a_gc_pi                         = 1 << 15,
    a_gc_punctuation_initial        = a_gc_pi,
    a_gc_pf                         = 1 << 16,
    a_gc_punctuation_final          = a_gc_pf,
    a_gc_po                         = 1 << 17,
    a_gc_punctuation_other          = a_gc_po,
    a_gc_sm                         = 1 << 18,
    a_gc_symbol_math                = a_gc_sm,
    a_gc_sc                         = 1 << 19,
    a_gc_symbol_currency            = a_gc_sc,
    a_gc_sk                         = 1 << 20,
    a_gc_symbol_modifier            = a_gc_sk,
    a_gc_so                         = 1 << 21,
    a_gc_symbol_other               = a_gc_so,
    a_gc_zs                         = 1 << 22,
    a_gc_separator_space            = a_gc_zs,
    a_gc_zl                         = 1 << 23,
    a_gc_separator_line             = a_gc_zl,
    a_gc_zp                         = 1 << 24,
    a_gc_separator_paragraph        = a_gc_zp,
    a_gc_cc                         = 1 << 25,
    a_gc_other_control              = a_gc_cc,
    a_gc_cf                         = 1 << 26,
    a_gc_other_format               = a_gc_cf,
    a_gc_cs                         = 1 << 27,
    a_gc_other_surrogate            = a_gc_cs,
    a_gc_co                         = 1 << 28,
    a_gc_other_private_use          = a_gc_co,
    a_gc_cn                         = 1 << 29,
    a_gc_other_not_assigned         = a_gc_cn,
    /* derived categories */
    a_gc_alpha          = a_gc_lu|a_gc_ll|a_gc_lt|a_gc_lm|a_gc_lo,
    a_gc_alphanumeric   = a_gc_lu|a_gc_ll|a_gc_lt|a_gc_lm|a_gc_lo|a_gc_nd|a_gc_nl|a_gc_no,
    a_gc_graphical      = a_gc_lu|a_gc_ll|a_gc_lt|a_gc_lm|a_gc_lo|a_gc_mn|a_gc_mc|a_gc_me|a_gc_nd|a_gc_nl|a_gc_no|a_gc_pc|a_gc_pd|a_gc_ps|a_gc_pe|a_gc_pi|a_gc_pf|a_gc_po|a_gc_sm|a_gc_sc|a_gc_sk|a_gc_so|a_gc_zs|a_gc_zl|a_gc_zp,
    a_gc_letter         = a_gc_lu|a_gc_ll|a_gc_lt|a_gc_lm|a_gc_lo,
    a_gc_mark           = a_gc_mn|a_gc_mc|a_gc_me,
    a_gc_number         = a_gc_nd|a_gc_nl|a_gc_no,
    a_gc_punctuation    = a_gc_pc|a_gc_pd|a_gc_ps|a_gc_pe|a_gc_pi|a_gc_pf|a_gc_po,
    a_gc_separator      = a_gc_zs|a_gc_zl|a_gc_zp,
    a_gc_symbol         = a_gc_sm|a_gc_sc|a_gc_sk|a_gc_so
};
/*@}*/


/** 
 * \anchor blocks_functions
 * \name Blocks
 *
 * ...
 * 
 * @{
 */
a_cp        a_block_start(int block);
a_cp        a_block_end(int block);
a_cp        a_block_size(int block);
const char *a_block_name(int block);
const char *a_block_name_cp(a_cp codepoint);
int         a_block_cp(a_cp codepoint);
enum a_blocks
{
        a_block_basic_latin                                        = 0,
        a_block_latin_1_supplement                                 = 1,
        a_block_latin_extended_a                                   = 2,
        a_block_latin_extended_b                                   = 3,
        a_block_ipa_extensions                                     = 4,
        a_block_spacing_modifier_letters                           = 5,
        a_block_combining_diacritical_marks                        = 6,
        a_block_greek_and_coptic                                   = 7,
        a_block_cyrillic                                           = 8,
        a_block_cyrillic_supplement                                = 9,
        a_block_armenian                                           = 10,
        a_block_hebrew                                             = 11,
        a_block_arabic                                             = 12,
        a_block_syriac                                             = 13,
        a_block_arabic_supplement                                  = 14,
        a_block_thaana                                             = 15,
        a_block_nko                                                = 16,
        a_block_samaritan                                          = 17,
        a_block_mandaic                                            = 18,
        a_block_arabic_extended_a                                  = 19,
        a_block_devanagari                                         = 20,
        a_block_bengali                                            = 21,
        a_block_gurmukhi                                           = 22,
        a_block_gujarati                                           = 23,
        a_block_oriya                                              = 24,
        a_block_tamil                                              = 25,
        a_block_telugu                                             = 26,
        a_block_kannada                                            = 27,
        a_block_malayalam                                          = 28,
        a_block_sinhala                                            = 29,
        a_block_thai                                               = 30,
        a_block_lao                                                = 31,
        a_block_tibetan                                            = 32,
        a_block_myanmar                                            = 33,
        a_block_georgian                                           = 34,
        a_block_hangul_jamo                                        = 35,
        a_block_ethiopic                                           = 36,
        a_block_ethiopic_supplement                                = 37,
        a_block_cherokee                                           = 38,
        a_block_unified_canadian_aboriginal_syllabics              = 39,
        a_block_ogham                                              = 40,
        a_block_runic                                              = 41,
        a_block_tagalog                                            = 42,
        a_block_hanunoo                                            = 43,
        a_block_buhid                                              = 44,
        a_block_tagbanwa                                           = 45,
        a_block_khmer                                              = 46,
        a_block_mongolian                                          = 47,
        a_block_unified_canadian_aboriginal_syllabics_extended     = 48,
        a_block_limbu                                              = 49,
        a_block_tai_le                                             = 50,
        a_block_new_tai_lue                                        = 51,
        a_block_khmer_symbols                                      = 52,
        a_block_buginese                                           = 53,
        a_block_tai_tham                                           = 54,
        a_block_combining_diacritical_marks_extended               = 55,
        a_block_balinese                                           = 56,
        a_block_sundanese                                          = 57,
        a_block_batak                                              = 58,
        a_block_lepcha                                             = 59,
        a_block_ol_chiki                                           = 60,
        a_block_sundanese_supplement                               = 61,
        a_block_vedic_extensions                                   = 62,
        a_block_phonetic_extensions                                = 63,
        a_block_phonetic_extensions_supplement                     = 64,
        a_block_combining_diacritical_marks_supplement             = 65,
        a_block_latin_extended_additional                          = 66,
        a_block_greek_extended                                     = 67,
        a_block_general_punctuation                                = 68,
        a_block_superscripts_and_subscripts                        = 69,
        a_block_currency_symbols                                   = 70,
        a_block_combining_diacritical_marks_for_symbols            = 71,
        a_block_letterlike_symbols                                 = 72,
        a_block_number_forms                                       = 73,
        a_block_arrows                                             = 74,
        a_block_mathematical_operators                             = 75,
        a_block_miscellaneous_technical                            = 76,
        a_block_control_pictures                                   = 77,
        a_block_optical_character_recognition                      = 78,
        a_block_enclosed_alphanumerics                             = 79,
        a_block_box_drawing                                        = 80,
        a_block_block_elements                                     = 81,
        a_block_geometric_shapes                                   = 82,
        a_block_miscellaneous_symbols                              = 83,
        a_block_dingbats                                           = 84,
        a_block_miscellaneous_mathematical_symbols_a               = 85,
        a_block_supplemental_arrows_a                              = 86,
        a_block_braille_patterns                                   = 87,
        a_block_supplemental_arrows_b                              = 88,
        a_block_miscellaneous_mathematical_symbols_b               = 89,
        a_block_supplemental_mathematical_operators                = 90,
        a_block_miscellaneous_symbols_and_arrows                   = 91,
        a_block_glagolitic                                         = 92,
        a_block_latin_extended_c                                   = 93,
        a_block_coptic                                             = 94,
        a_block_georgian_supplement                                = 95,
        a_block_tifinagh                                           = 96,
        a_block_ethiopic_extended                                  = 97,
        a_block_cyrillic_extended_a                                = 98,
        a_block_supplemental_punctuation                           = 99,
        a_block_cjk_radicals_supplement                            = 100,
        a_block_kangxi_radicals                                    = 101,
        a_block_ideographic_description_characters                 = 102,
        a_block_cjk_symbols_and_punctuation                        = 103,
        a_block_hiragana                                           = 104,
        a_block_katakana                                           = 105,
        a_block_bopomofo                                           = 106,
        a_block_hangul_compatibility_jamo                          = 107,
        a_block_kanbun                                             = 108,
        a_block_bopomofo_extended                                  = 109,
        a_block_cjk_strokes                                        = 110,
        a_block_katakana_phonetic_extensions                       = 111,
        a_block_enclosed_cjk_letters_and_months                    = 112,
        a_block_cjk_compatibility                                  = 113,
        a_block_cjk_unified_ideographs_extension_a                 = 114,
        a_block_yijing_hexagram_symbols                            = 115,
        a_block_cjk_unified_ideographs                             = 116,
        a_block_yi_syllables                                       = 117,
        a_block_yi_radicals                                        = 118,
        a_block_lisu                                               = 119,
        a_block_vai                                                = 120,
        a_block_cyrillic_extended_b                                = 121,
        a_block_bamum                                              = 122,
        a_block_modifier_tone_letters                              = 123,
        a_block_latin_extended_d                                   = 124,
        a_block_syloti_nagri                                       = 125,
        a_block_common_indic_number_forms                          = 126,
        a_block_phags_pa                                           = 127,
        a_block_saurashtra                                         = 128,
        a_block_devanagari_extended                                = 129,
        a_block_kayah_li                                           = 130,
        a_block_rejang                                             = 131,
        a_block_hangul_jamo_extended_a                             = 132,
        a_block_javanese                                           = 133,
        a_block_myanmar_extended_b                                 = 134,
        a_block_cham                                               = 135,
        a_block_myanmar_extended_a                                 = 136,
        a_block_tai_viet                                           = 137,
        a_block_meetei_mayek_extensions                            = 138,
        a_block_ethiopic_extended_a                                = 139,
        a_block_latin_extended_e                                   = 140,
        a_block_meetei_mayek                                       = 141,
        a_block_hangul_syllables                                   = 142,
        a_block_hangul_jamo_extended_b                             = 143,
        a_block_high_surrogates                                    = 144,
        a_block_high_private_use_surrogates                        = 145,
        a_block_low_surrogates                                     = 146,
        a_block_private_use_area                                   = 147,
        a_block_cjk_compatibility_ideographs                       = 148,
        a_block_alphabetic_presentation_forms                      = 149,
        a_block_arabic_presentation_forms_a                        = 150,
        a_block_variation_selectors                                = 151,
        a_block_vertical_forms                                     = 152,
        a_block_combining_half_marks                               = 153,
        a_block_cjk_compatibility_forms                            = 154,
        a_block_small_form_variants                                = 155,
        a_block_arabic_presentation_forms_b                        = 156,
        a_block_halfwidth_and_fullwidth_forms                      = 157,
        a_block_specials                                           = 158,
        a_block_linear_b_syllabary                                 = 159,
        a_block_linear_b_ideograms                                 = 160,
        a_block_aegean_numbers                                     = 161,
        a_block_ancient_greek_numbers                              = 162,
        a_block_ancient_symbols                                    = 163,
        a_block_phaistos_disc                                      = 164,
        a_block_lycian                                             = 165,
        a_block_carian                                             = 166,
        a_block_coptic_epact_numbers                               = 167,
        a_block_old_italic                                         = 168,
        a_block_gothic                                             = 169,
        a_block_old_permic                                         = 170,
        a_block_ugaritic                                           = 171,
        a_block_old_persian                                        = 172,
        a_block_deseret                                            = 173,
        a_block_shavian                                            = 174,
        a_block_osmanya                                            = 175,
        a_block_elbasan                                            = 176,
        a_block_caucasian_albanian                                 = 177,
        a_block_linear_a                                           = 178,
        a_block_cypriot_syllabary                                  = 179,
        a_block_imperial_aramaic                                   = 180,
        a_block_palmyrene                                          = 181,
        a_block_nabataean                                          = 182,
        a_block_phoenician                                         = 183,
        a_block_lydian                                             = 184,
        a_block_meroitic_hieroglyphs                               = 185,
        a_block_meroitic_cursive                                   = 186,
        a_block_kharoshthi                                         = 187,
        a_block_old_south_arabian                                  = 188,
        a_block_old_north_arabian                                  = 189,
        a_block_manichaean                                         = 190,
        a_block_avestan                                            = 191,
        a_block_inscriptional_parthian                             = 192,
        a_block_inscriptional_pahlavi                              = 193,
        a_block_psalter_pahlavi                                    = 194,
        a_block_old_turkic                                         = 195,
        a_block_rumi_numeral_symbols                               = 196,
        a_block_brahmi                                             = 197,
        a_block_kaithi                                             = 198,
        a_block_sora_sompeng                                       = 199,
        a_block_chakma                                             = 200,
        a_block_mahajani                                           = 201,
        a_block_sharada                                            = 202,
        a_block_sinhala_archaic_numbers                            = 203,
        a_block_khojki                                             = 204,
        a_block_khudawadi                                          = 205,
        a_block_grantha                                            = 206,
        a_block_tirhuta                                            = 207,
        a_block_siddham                                            = 208,
        a_block_modi                                               = 209,
        a_block_takri                                              = 210,
        a_block_warang_citi                                        = 211,
        a_block_pau_cin_hau                                        = 212,
        a_block_cuneiform                                          = 213,
        a_block_cuneiform_numbers_and_punctuation                  = 214,
        a_block_egyptian_hieroglyphs                               = 215,
        a_block_bamum_supplement                                   = 216,
        a_block_mro                                                = 217,
        a_block_bassa_vah                                          = 218,
        a_block_pahawh_hmong                                       = 219,
        a_block_miao                                               = 220,
        a_block_kana_supplement                                    = 221,
        a_block_duployan                                           = 222,
        a_block_shorthand_format_controls                          = 223,
        a_block_byzantine_musical_symbols                          = 224,
        a_block_musical_symbols                                    = 225,
        a_block_ancient_greek_musical_notation                     = 226,
        a_block_tai_xuan_jing_symbols                              = 227,
        a_block_counting_rod_numerals                              = 228,
        a_block_mathematical_alphanumeric_symbols                  = 229,
        a_block_mende_kikakui                                      = 230,
        a_block_arabic_mathematical_alphabetic_symbols             = 231,
        a_block_mahjong_tiles                                      = 232,
        a_block_domino_tiles                                       = 233,
        a_block_playing_cards                                      = 234,
        a_block_enclosed_alphanumeric_supplement                   = 235,
        a_block_enclosed_ideographic_supplement                    = 236,
        a_block_miscellaneous_symbols_and_pictographs              = 237,
        a_block_emoticons                                          = 238,
        a_block_ornamental_dingbats                                = 239,
        a_block_transport_and_map_symbols                          = 240,
        a_block_alchemical_symbols                                 = 241,
        a_block_geometric_shapes_extended                          = 242,
        a_block_supplemental_arrows_c                              = 243,
        a_block_cjk_unified_ideographs_extension_b                 = 244,
        a_block_cjk_unified_ideographs_extension_c                 = 245,
        a_block_cjk_unified_ideographs_extension_d                 = 246,
        a_block_cjk_compatibility_ideographs_supplement            = 247,
        a_block_tags                                               = 248,
        a_block_variation_selectors_supplement                     = 249,
        a_block_supplementary_private_use_area_a                   = 250,
        a_block_supplementary_private_use_area_b                   = 251,
        a_block_count
};
/*@}*/


/** 
 * \anchor normalization_functions
 * \name Normalization
 *
 * Normalization is a process by which Unicode text can converted into a form
 * where unwanted distinctions are eliminated. The Unicode standard defines 4
 * normalization forms:
 * 
 *  - Normalization Form D (NFD)
 *  - Normalization Form KD (NFKD)
 *  - Normalization Form C (NFC)
 *  - Normalization Form KC (NFKC)
 * 
 * (NF=Normalization Form; C=Composed; D=Decomposed; K=Kompatibility)
 * 
 * Where forms NFD and NFKD generally decompose code points while NFC and NFKC
 * compose code points wherever possible.
 * 
 * Normalization is also very important to providing a unique canonical order
 * for sequences of combining code points that are visually indistinguishable.
 * 
 * Decompositions
 * --------------
 * 
 * Decomposables code points are precomposed code points that may be broken down
 * (decomposed) into one or more other code points - conceptually, reducing a
 * code point to an equivalent sequence of constituent parts. Two types of 
 * decompositions exist:
 * 
 *  - Canonical     - Both the code point and its decomposition are treated
 *                    as equivalent.
 * 
 *  - Compatibility - A decomposition that may result in loss of information
 *                    (e.g. formatting info) that is important in various
 *                    contexts.
 * 
 * 
 * @{
 */
a_str       a_normalize(a_str str, int mode);
a_str       a_normalize_cstr(const char *str, int mode);
a_str       a_new_normalize(a_cstr str, int mode); /* also declared above */
int         a_is_quick_normalized(a_str str, int mode);
enum a_normalization_forms
{
    a_norm_nfc,  /* Canonical decomposition followed by canonical composition.     */
    a_norm_nfd,  /* Canonical decomposition.                                       */
    a_norm_nfkc, /* Compatibility decomposition followed by canonical composition. */
    a_norm_nfkd  /* Compatibility decomposition. */
};
enum a_normalization_forms_quick_check
{
    a_norm_yes,
    a_norm_no,
    a_norm_maybe
};
/*@}*/



/** 
 * \anchor hashing_functions
 * \name Hashing
 *
 * ...
 * 
 * @{
 */
unsigned long   a_hash(a_cstr str);
unsigned int    a_crc32(a_cstr str);
/*@}*/


/** 
 * \anchor unicode_version_functions
 * \name Version
 *
 * ...
 * 
 * @{
 */
const char     *a_unicode_version(void);
const char     *a_unicode_version_reference(void);
const char     *a_unicode_version_url(void);
/*@}*/


#if A_INCLUDE_LOCALE == 1
/** 
 * \anchor locale_functions
 * \name Locale
 *
 * ...
 * 
 * @{
 */
/**
 * 
 * A locale is an identifier that refers to a set of user preferences
 * that tend to be shared across significant swaths of the world. 
 * 
 * Various things that relay on the user's locale are parsing of dates,
 * times, numbers, and currencies, measurement units, collation, as well,
 * as languages, scripts, translated names for time zones, and countries.
 * 
 */
/**
 * 
 * \brief Set the current locale
 * 
 * language[_-]script[_-]region[_-]variant([_-]optional t/u extensions)
 * 
 * language - Unicode base language code - <a href="http://www.loc.gov/standards/iso639-2/">ISO-639</a>
 *            language code such as 'en', 'es', 'ale', 'fr', and 'he'.
 *   script - Unicode script code - <a href="http://www.unicode.org/iso15924/iso15924-codes.html">ISO-15924</a> 
 *            script code such as 'Latn', 'Hebr', 'Arab', 'Hant', and 'Zmth'.
 *   region - Unicode region code - <a href="http://www.iso.org/iso/en/prods-services/iso3166ma/02iso-3166-code-lists/list-en1.html">ISO-3166</a>
 *            region code such as 'AU', 'EG', 'IL', and 'US'.
 *  variant - Unicode language variant code
 * 
 * Example locales:
 * 
 *  - 'en', 'en-US', 'en-Latn-US'
 *  - 'he', 'he-IL', 'he-Hebr-IL', 'he-Hebr-US'
 *  - 'en', 'en-AU', 'en-Latn-AU'
 *  - 'ar', 'ar-EG', 'ar-Arab-EG'
 * 
 * (although specifyingt the script where that script will typically be used is highly discourged)
 * 
 * As per UTS #35, no BCP47 grandfathered tag are accepted.
 * 
 * \note The default locale is root.
 * 
 */
int a_locale_set(const char *id);
/*@}*/
#endif


#if 0
#ifdef A_INCLUDE_TOKEN
/** 
 * \anchor tokenization_functions
 * \name Token/List Manipulation
 *
 * ...
 * 
 * @{
 */
/* token/list manipulation (most of those are just convenience variations of the same internal function) */
a_str       a_tok_add(a_str str, a_cstr token, const char *delimiter);
a_str       a_tok_add_cp(a_str str, a_cstr token, a_cp codepoint);
a_str       a_tok_add_cstr(a_str str, const char *token, const char *delimiter);
a_str       a_tok_add_cstr_cp(a_str str, const char *token, a_cp codepoint);
a_str       a_tok_rem(a_str str, a_cstr token, const char *delimiter, size_t nth);
a_str       a_tok_rem_cp(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_rem_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_rem_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_del(a_str str, const char *delimiter, size_t nth);
a_str       a_tok_del_cp(a_str str, a_cp codepoint, size_t nth);
a_str       a_tok_find(a_str str, a_cstr token, const char *delimiter, size_t nth);
a_str       a_tok_find_cp(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_find_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_find_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_get(a_str str, const char *delimiter, size_t nth);
a_str       a_tok_get_cp(a_str str, a_cp codepoint, size_t nth);
a_str       a_tok_len(a_str str, const char *delimiter, size_t nth);
a_str       a_tok_len_cp(a_str str, a_cp codepoint, size_t nth);
a_str       a_tok_ins(a_str str, a_cstr token, const char *delimiter, size_t nth); /* insert as the Nth token */
a_str       a_tok_ins_cp(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_ins_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_ins_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_put(a_str str, a_str token, const char *delimiter, size_t nth);               /* override Nth token */
a_str       a_tok_put_cp(a_str str, a_str token, a_cp codepoint, size_t nth);
a_str       a_tok_put_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_put_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_rep(a_str str, a_cstr token, a_cstr new_token, a_cp codepoint, size_t nth);      /* replace a token */
a_str       a_tok_rep_cp(a_str str, a_cstr token, a_cstr new_token, const char *delimiter, size_t nth);
a_str       a_tok_rep_cstr(a_str str, a_cstr token, const char *new_token, a_cp codepoint, size_t nth);
a_str       a_tok_rep_cstr_cp(a_str str, a_cstr token, const char *new_token, const char *delimiter, size_t nth);
a_str       a_tok_rep_cstr_cstr(a_str str, const char *token, const char *new_token, a_cp codepoint, size_t nth);
a_str       a_tok_rep_cstr_cstr_cp(a_str str, const char *token, const char *new_token, const char *delimiter, size_t nth);
a_str       a_tok_rem(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_rem_cp(a_str str, a_cstr token, const char *delimiter, size_t nth);
a_str       a_tok_rem_cstr(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_rem_cstr_cp(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_wild(a_str str, a_cstr pattern, a_cp codepoint, size_t nth);                       /* wildmatch Nth token */
a_str       a_tok_wild_cp(a_str str, a_cstr pattern, const char *delimiter, size_t nth);
a_str       a_tok_wild_cstr(a_str str, const char *pattern, a_cp codepoint, size_t nth);
a_str       a_tok_wild_cstr_cp(a_str str, const char *pattern, const char *delimiter, size_t nth);
a_str       a_tok_cont(a_str str, a_cstr pattern, a_cp codepoint, size_t nth);                       /* match Nth token */
a_str       a_tok_cont_cp(a_str str, a_cstr pattern, const char *delimiter, size_t nth);
a_str       a_tok_cont_cstr(a_str str, const char *pattern, a_cp codepoint, size_t nth);             /* match Nth token */
a_str       a_tok_cont_cstr_cp(a_str str, const char *pattern, const char *delimiter, size_t nth);
a_str       a_tok_sort(a_str str, const char *delimiter, int sort_option);
a_str       a_tok_sort_cp(a_str str, a_cp codepoint, int sort_option);
            /* case insensitive version ... */
a_str       a_tok_i_add(a_str str, a_cstr token, const char *delimiter);
a_str       a_tok_i_add_cp(a_str str, a_cstr token, a_cp codepoint);
a_str       a_tok_i_add_cstr(a_str str, const char *token, const char *delimiter);
a_str       a_tok_i_add_cstr_cp(a_str str, const char *token, a_cp codepoint);
a_str       a_tok_i_rem(a_str str, a_cstr token, const char *delimiter, size_t nth);
a_str       a_tok_i_rem_cp(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_i_rem_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_i_rem_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_i_del(a_str str, const char *delimiter, size_t nth);
a_str       a_tok_i_del_cp(a_str str, a_cp codepoint, size_t nth);
a_str       a_tok_i_find(a_str str, a_cstr token, const char *delimiter, size_t nth);
a_str       a_tok_i_find_cp(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_i_find_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_i_find_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_i_get(a_str str, const char *delimiter, size_t nth);
a_str       a_tok_i_get_cp(a_str str, a_cp codepoint, size_t nth);
a_str       a_tok_i_len(a_str str, const char *delimiter, size_t nth);
a_str       a_tok_i_len_cp(a_str str, a_cp codepoint, size_t nth);
a_str       a_tok_i_ins(a_str str, a_cstr token, const char *delimiter, size_t nth); /* insert as the Nth token */
a_str       a_tok_i_ins_cp(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_i_ins_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_i_ins_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_i_put(a_str str, a_str token, const char *delimiter, size_t nth);               /* override Nth token */
a_str       a_tok_i_put_cp(a_str str, a_str token, a_cp codepoint, size_t nth);
a_str       a_tok_i_put_cstr(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_i_put_cstr_cp(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_i_rep(a_str str, a_cstr token, a_cstr new_token, a_cp codepoint, size_t nth);      /* replace a token */
a_str       a_tok_i_rep_cp(a_str str, a_cstr token, a_cstr new_token, const char *delimiter, size_t nth);
a_str       a_tok_i_rep_cstr(a_str str, a_cstr token, const char *new_token, a_cp codepoint, size_t nth);
a_str       a_tok_i_rep_cstr_cp(a_str str, a_cstr token, const char *new_token, const char *delimiter, size_t nth);
a_str       a_tok_i_rep_cstr_cstr(a_str str, const char *token, const char *new_token, a_cp codepoint, size_t nth);
a_str       a_tok_i_rep_cstr_cstr_cp(a_str str, const char *token, const char *new_token, const char *delimiter, size_t nth);
a_str       a_tok_i_rem(a_str str, a_cstr token, a_cp codepoint, size_t nth);
a_str       a_tok_i_rem_cp(a_str str, a_cstr token, const char *delimiter, size_t nth);
a_str       a_tok_i_rem_cstr(a_str str, const char *token, a_cp codepoint, size_t nth);
a_str       a_tok_i_rem_cstr_cp(a_str str, const char *token, const char *delimiter, size_t nth);
a_str       a_tok_i_wild(a_str str, a_cstr pattern, a_cp codepoint, size_t nth);                       /* wildmatch Nth token */
a_str       a_tok_i_wild_cp(a_str str, a_cstr pattern, const char *delimiter, size_t nth);
a_str       a_tok_i_wild_cstr(a_str str, const char *pattern, a_cp codepoint, size_t nth);
a_str       a_tok_i_wild_cstr_cp(a_str str, const char *pattern, const char *delimiter, size_t nth);
a_str       a_tok_i_cont(a_str str, a_cstr pattern, a_cp codepoint, size_t nth);                       /* match Nth token */
a_str       a_tok_i_cont_cp(a_str str, a_cstr pattern, const char *delimiter, size_t nth);
a_str       a_tok_i_cont_cstr(a_str str, const char *pattern, a_cp codepoint, size_t nth);             /* match Nth token */
a_str       a_tok_i_cont_cstr_cp(a_str str, const char *pattern, const char *delimiter, size_t nth);
a_str       a_tok_i_sort(a_str str, const char *delimiter, int sort_option);
a_str       a_tok_i_sort_cp(a_str str, a_cp codepoint, int sort_option);
/*@}*/
#endif
#endif

enum a_gcb_property
{
    a_gcb_CR,
    a_gcb_other, /* UCD calls it XX in various places as well... */
    a_gcb_control,
    a_gcb_extend,
    a_gcb_prepend,
    a_gcb_spacing_mark,
    a_gcb_regional_indicator,
    a_gcb_L,
    a_gcb_V,
    a_gcb_T,
    a_gcb_LF,
    a_gcb_LV,
    a_gcb_LVT,
    a_gcb_count
};

#ifndef DOXYGEN_DOCS
struct a_header
{
    size_t len;  /* length (in code points)         */
    size_t size; /* size (in bytes)                 */
    size_t mem;  /* mem size                        */
    #ifdef A_ITERATOR
    char *it;    /* pointer to the current char     */
    #endif
};
#define a_buff(b) ((char*)b + sizeof (struct a_header))
#define a_header(b) ((struct a_header*)((char*)b - sizeof (struct a_header)))
#endif

#endif
/* EOF */
