/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#if defined(__clang_analyzer__) && !defined(SDL_DISABLE_ANALYZE_MACROS)
#define SDL_DISABLE_ANALYZE_MACROS 1
#endif

#include "../SDL_internal.h"

#if defined(__WIN32__)
#include "../core/windows/SDL_windows.h"
#endif

#include "SDL_stdinc.h"

#if defined(__WIN32__) && (!defined(HAVE_SETENV) || !defined(HAVE_GETENV))
/* Note this isn't thread-safe! */
static char *SDL_envmem = NULL; /* Ugh, memory leak */
static size_t SDL_envmemlen = 0;
#endif

/* Put a variable into the environment */
/* Note: Name may not contain a '=' character. (Reference: http://www.unix.com/man-page/Linux/3/setenv/) */
#if defined(HAVE_SETENV)
int
SDL_setenv(const char *name, const char *value, int overwrite)
{
    /* Input validation */
    if (!name || strlen(name) == 0 || SDL_strchr(name, '=') != NULL || !value) {
        return (-1);
    }
    
    return setenv(name, value, overwrite);
}
#elif defined(__WIN32__)
int
SDL_setenv(const char *name, const char *value, int overwrite)
{
    /* Input validation */
    if (!name || strlen(name) == 0 || SDL_strchr(name, '=') != NULL || !value) {
        return (-1);
    }
    
    if (!overwrite) {
        char ch = 0;
        const size_t len = GetEnvironmentVariableA(name, &ch, sizeof (ch));
        if (len > 0) {
            return 0;  /* asked not to overwrite existing value. */
        }
    }
    if (!SetEnvironmentVariableA(name, *value ? value : NULL)) {
        return -1;
    }
    return 0;
}
/* We have a real environment table, but no real setenv? Fake it w/ putenv. */
#elif (defined(HAVE_GETENV) && defined(HAVE_PUTENV) && !defined(HAVE_SETENV))
int
SDL_setenv(const char *name, const char *value, int overwrite)
{
    size_t len;
    char *new_variable;

    /* Input validation */
    if (!name || strlen(name) == 0 || SDL_strchr(name, '=') != NULL || !value) {
        return (-1);
    }
    
    if (getenv(name) != NULL) {
        if (overwrite) {
            unsetenv(name);
        } else {
            return 0;  /* leave the existing one there. */
        }
    }

    /* This leaks. Sorry. Get a better OS so we don't have to do this. */
    len = strlen(name) + strlen(value) + 2;
    new_variable = (char *)malloc(len);
    if (!new_variable) {
        return (-1);
    }

    SDL_snprintf(new_variable, len, "%s=%s", name, value);
    return putenv(new_variable);
}
#else /* roll our own */
static char **SDL_env = (char **) 0;
int
SDL_setenv(const char *name, const char *value, int overwrite)
{
    int added;
    int len, i;
    char **new_env;
    char *new_variable;

    /* Input validation */
    if (!name || strlen(name) == 0 || SDL_strchr(name, '=') != NULL || !value) {
        return (-1);
    }

    /* See if it already exists */
    if (!overwrite && SDL_getenv(name)) {
        return 0;
    }

    /* Allocate memory for the variable */
    len = strlen(name) + strlen(value) + 2;
    new_variable = (char *)malloc(len);
    if (!new_variable) {
        return (-1);
    }

    SDL_snprintf(new_variable, len, "%s=%s", name, value);
    value = new_variable + strlen(name) + 1;
    name = new_variable;

    /* Actually put it into the environment */
    added = 0;
    i = 0;
    if (SDL_env) {
        /* Check to see if it's already there... */
        len = (value - name);
        for (; SDL_env[i]; ++i) {
            if (SDL_strncmp(SDL_env[i], name, len) == 0) {
                break;
            }
        }
        /* If we found it, just replace the entry */
        if (SDL_env[i]) {
            free(SDL_env[i]);
            SDL_env[i] = new_variable;
            added = 1;
        }
    }

    /* Didn't find it in the environment, expand and add */
    if (!added) {
        new_env = realloc(SDL_env, (i + 2) * sizeof(char *));
        if (new_env) {
            SDL_env = new_env;
            SDL_env[i++] = new_variable;
            SDL_env[i++] = (char *) 0;
            added = 1;
        } else {
            free(new_variable);
        }
    }
    return (added ? 0 : -1);
}
#endif

/* Retrieve a variable named "name" from the environment */
#if defined(HAVE_GETENV)
char *
SDL_getenv(const char *name)
{
    /* Input validation */
    if (!name || strlen(name)==0) {
        return NULL;
    }

    return getenv(name);
}
#elif defined(__WIN32__)
char *
SDL_getenv(const char *name)
{
    size_t bufferlen;

    /* Input validation */
    if (!name || strlen(name)==0) {
        return NULL;
    }
    
    bufferlen =
        GetEnvironmentVariableA(name, SDL_envmem, (DWORD) SDL_envmemlen);
    if (bufferlen == 0) {
        return NULL;
    }
    if (bufferlen > SDL_envmemlen) {
        char *newmem = (char *) realloc(SDL_envmem, bufferlen);
        if (newmem == NULL) {
            return NULL;
        }
        SDL_envmem = newmem;
        SDL_envmemlen = bufferlen;
        GetEnvironmentVariableA(name, SDL_envmem, (DWORD) SDL_envmemlen);
    }
    return SDL_envmem;
}
#else
char *
SDL_getenv(const char *name)
{
    int len, i;
    char *value;

    /* Input validation */
    if (!name || strlen(name)==0) {
        return NULL;
    }
    
    value = (char *) 0;
    if (SDL_env) {
        len = strlen(name);
        for (i = 0; SDL_env[i] && !value; ++i) {
            if ((SDL_strncmp(SDL_env[i], name, len) == 0) &&
                (SDL_env[i][len] == '=')) {
                value = &SDL_env[i][len + 1];
            }
        }
    }
    return value;
}
#endif

/* vi: set ts=4 sw=4 expandtab: */
