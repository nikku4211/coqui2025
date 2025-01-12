/*
 mgba.h
 Copyright (c) 2016 Jeffrey Pfau

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Copyright (c) 2020-2025 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include <stdio.h>
#include <stdarg.h>

#ifndef MGBA_H
#define MGBA_H

#ifdef __cplusplus
extern "C" {
#endif

#define REG_DEBUG_ENABLE (vu16*) 0x4FFF780
#define REG_DEBUG_FLAGS (vu16*) 0x4FFF700
#define REG_DEBUG_STRING (char*) 0x4FFF600

static inline void mlog(const char* message, ...)
{
    // https://github.com/mgba-emu/mgba/blob/master/opt/libgba/mgba.c

    *REG_DEBUG_ENABLE = 0xC0DE;
    va_list args;
	va_start(args, message);
    vsnprintf(REG_DEBUG_STRING, 0x100, message, args);
    va_end(args);
    *REG_DEBUG_FLAGS = 2 | 0x100;
}

#ifdef __cplusplus
}
#endif

#endif
