#ifndef PORTABILITY_H
#  define PORTABILITY_H

/*
 * Copyright (C) 2001 Alan Robertson <alanr@unix.sh>
 * This software licensed under the GNU LGPL.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#  define	EOS			'\0'
#  define	DIMOF(a)		((int) (sizeof(a)/sizeof(a[0])) )

/* Needs to be defined before any other includes, otherwise some system
 * headers do not behave as expected! Major black magic... */
#  undef _GNU_SOURCE            /* in case it was defined on the command line */
#  define _GNU_SOURCE

/* Please leave this as the first #include - Solaris needs it there */
#  ifdef HAVE_CONFIG_H
#    include <config.h>
#  endif

/* Prototypes for libreplace functions */

#  ifndef HAVE_DAEMON
  /* We supply a replacement function, but need a prototype */
int daemon(int nochdir, int noclose);
#  endif                        /* HAVE_DAEMON */

#  ifndef HAVE_SETENV
  /* We supply a replacement function, but need a prototype */
int setenv(const char *name, const char *value, int why);
#  endif                        /* HAVE_SETENV */

#  ifndef HAVE_STRERROR
  /* We supply a replacement function, but need a prototype */
char *strerror(int errnum);
#  endif                        /* HAVE_STRERROR */

#  ifndef HAVE_STRCHRNUL
  /* We supply a replacement function, but need a prototype */
char *strchrnul (const char *s, int c_in);
#  endif                        /* HAVE_STRCHRNUL */

#  ifndef HAVE_ALPHASORT
#    include <dirent.h>
int
 alphasort(const void *dirent1, const void *dirent2);
#  endif                        /* HAVE_ALPHASORT */

#  ifndef HAVE_STRNLEN
size_t strnlen(const char *s, size_t maxlen);
#  else
#    	define USE_GNU
#  endif

#  ifndef HAVE_STRNDUP
char *strndup(const char *str, size_t len);
#  else
#    	define USE_GNU
#  endif

#  if NEED_G_HASH_ITER

#    include <glib.h>
typedef struct fake_ghi {
    GHashTable *hash;
    int nth;                    /* current index over the iteration */
    int lpc;                    /* internal loop counter inside g_hash_table_find */
    gpointer key;
    gpointer value;
} GHashTableIter;

static inline void
g_hash_prepend_value(gpointer key, gpointer value, gpointer user_data)
{
    GList **values = (GList **) user_data;

    *values = g_list_prepend(*values, value);
}

/* Since: 2.14 */
static inline GList *
g_hash_table_get_values(GHashTable * hash_table)
{
    GList *values = NULL;

    g_hash_table_foreach(hash_table, g_hash_prepend_value, &values);
    return values;
}

static inline gboolean
g_hash_table_nth_data(gpointer key, gpointer value, gpointer user_data)
{
    GHashTableIter *iter = (GHashTableIter *) user_data;

    if (iter->lpc++ == iter->nth) {
        iter->key = key;
        iter->value = value;
        return TRUE;
    }
    return FALSE;
}

/* Since: 2.16 */
static inline void
g_hash_table_iter_init(GHashTableIter * iter, GHashTable * hash_table)
{
    iter->hash = hash_table;
    iter->nth = 0;
    iter->lpc = 0;
    iter->key = NULL;
    iter->value = NULL;
}

static inline gboolean
g_hash_table_iter_next(GHashTableIter * iter, gpointer * key, gpointer * value)
{
    gboolean found = FALSE;

    iter->lpc = 0;
    iter->key = NULL;
    iter->value = NULL;
    if (iter->nth < g_hash_table_size(iter->hash)) {
        found = ! !g_hash_table_find(iter->hash, g_hash_table_nth_data, iter);
        iter->nth++;
    }
    if (key)
        *key = iter->key;
    if (value)
        *value = iter->value;
    return found;
}

/* Since: 2.16 */
static inline void
g_hash_table_iter_remove (GHashTableIter *iter)
{
    g_hash_table_remove(iter->hash, iter->key);
    iter->nth--; /* Or zero to be safe? */
}

/* Since: 2.16 */
static inline int
g_strcmp0 (const char     *str1,
           const char     *str2)
{
  if (!str1)
    return -(str1 != str2);
  if (!str2)
    return str1 != str2;
  return strcmp (str1, str2);
}
#  endif                        /* !HAVE_LIBGLIB_2_0 */

#ifdef NEED_G_LIST_FREE_FULL
#  include <glib.h>
#  include <string.h>
/* Since: 2.28 */
static inline void g_list_free_full(GList *list, GDestroyNotify free_func)
{
   g_list_foreach(list, (GFunc) free_func, NULL);
   g_list_free(list);
}
#endif

/* Replacement error codes for non-linux */
#ifndef ENOTUNIQ
#  define ENOTUNIQ  900
#endif

#ifndef ECOMM
#  define ECOMM     901
#endif

#ifndef ELIBACC
#  define ELIBACC   902
#endif

#ifndef EREMOTEIO
#  define EREMOTEIO 903
#endif

#ifndef EUNATCH
#  define EUNATCH   904
#endif

#ifndef ENOKEY
#  define ENOKEY    905
#endif

/*
 * Some compilers (eg. Sun studio) do not define __FUNCTION__
 */
#  ifdef __SUNPRO_C
#    define __FUNCTION__ __func__
#  endif

#  ifdef __MY_UNKNOWN_C
#    define __FUNCTION__ "__FUNCTION__"
#  endif

#endif                          /* PORTABILITY_H */
