/*

 Copyright 2016 Petr Tuma

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 */

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//---------------------------------------------------------------
// String Helpers

static bool gobble_string (char *&arg_input, const char *arg_string) {
    int string_length = strlen (arg_string);
    if (strncmp (arg_input, arg_string, string_length) == 0) {
        arg_input += string_length;
        return (true);
    }
    return (false);
}

static bool gobble_number (char *&arg_input, long &arg_number) {
    errno = 0;
    char *next;
    long number;
    number = strtol (arg_input, &next, 0);
    if (errno == 0) {
        arg_input = next;
        arg_number = number;
        return (true);
    }
    return (false);
}

static bool gobble_character (char *&arg_input, char arg_character) {
    if (*arg_input == arg_character) {
        arg_input ++;
        return (true);
    }
    return (false);
}

//---------------------------------------------------------------
// Library Configuration

extern "C" char **environ;

static const char ENV_PREFIX [] = "SCW";

#define MAX_OVERRIDES 16

// The configuration is really (int, long) rather than (long, long),
// but this avoids overflow checking and looks way more symmetric :-)
struct override_t {
    long name;
    long value;
};

// Quick and dirty. A more efficient data structure
// would pull in more library code which is not good.
static override_t overrides [MAX_OVERRIDES];
static int overrides_count = 0;

/**
 * Initialize the configuration using the environment variables.
 */
static void read_configuration () {
    for (char **variable = environ ; *variable ; variable ++) {
        char *setting = *variable;

        long name;
        long value;

        if (!gobble_string (setting, ENV_PREFIX)) continue;
        if (!gobble_number (setting, name)) continue;
        if (!gobble_character (setting, '=')) continue;
        if (!gobble_number (setting, value)) continue;
        if (!gobble_character (setting, 0)) continue;

        if (overrides_count == MAX_OVERRIDES) _exit (1);

        printf ("Overriding configuration %li with value %li.\n", name, value);
        overrides [overrides_count].name = name;
        overrides [overrides_count].value = value;
        overrides_count ++;
    }
}

//---------------------------------------------------------------
// Library Installation

static long (*original_sysconf) (int name) = NULL;

static void intercept_functions () {
    original_sysconf = (long (*) (int)) dlsym (RTLD_NEXT, "sysconf");
}

//---------------------------------------------------------------
// Wrapper Utilities

static bool initialized = false;
static bool initializing = false;

static void initialize (void)
{
    // We should never be called recursively while initializing.
    if (__atomic_test_and_set (&initializing, __ATOMIC_SEQ_CST)) _exit (1);

    read_configuration ();
    intercept_functions ();

    // Remember we are now initialized.
    __atomic_test_and_set (&initialized, __ATOMIC_SEQ_CST);
    __atomic_clear (&initializing, __ATOMIC_SEQ_CST);
}

//---------------------------------------------------------------
// System Config Wrapper

extern "C" long sysconf (int name) {
    if (!initialized) initialize ();

    for (int override = 0 ; override < overrides_count ; override ++) {
        if (overrides [override].name == name) {
            return (overrides [override].value);
        }
    }
    return ((*original_sysconf) (name));
}
