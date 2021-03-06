/*
** Copyright 2008, Google Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define LOG_TAG "RPC"

#define PRINT(x...) do {                                    \
        fprintf(stdout, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stdout, ##x);                               \
    } while(0)

#ifdef DEBUG
#define D PRINT
#else
#define D(x...) do { } while(0)
#endif

#ifdef VERBOSE
#define V PRINT
#else
#define V(x...) do { } while(0)
#endif

#define E(x...) do {                                        \
        fprintf(stderr, "%s(%d) ", __FUNCTION__, __LINE__); \
        fprintf(stderr, ##x);                               \
    } while(0)

#define FAILIF(cond, msg...) do {                                              \
        if (__builtin_expect (cond, 0)) {                                      \
            fprintf(stderr, "%s:%s:(%d): ", __FILE__, __FUNCTION__, __LINE__); \
            fprintf(stderr, ##msg);                                            \
        }                                                                      \
    } while(0)

#endif/*DEBUG_H*/
