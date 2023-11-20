/*
 * QEMU OpenTitan Data Object Exchange Mailbox
 *
 * Copyright (c) 2023 Rivos, Inc.
 *
 * Author(s):
 *  Emmanuel Blot <eblot@rivosinc.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef HW_OPENTITAN_OT_MBX_H
#define HW_OPENTITAN_OT_MBX_H

#include "qom/object.h"

#define TYPE_OT_MBX "ot-mbx"
OBJECT_DECLARE_SIMPLE_TYPE(OtMbxState, OT_MBX)

#define OT_MBX_HOST_REGS_COUNT 17u
#define OT_MBX_SYS_REGS_COUNT  6u

#define OT_MBX_HOST_APERTURE \
    DIV_ROUND_UP((OT_MBX_HOST_REGS_COUNT * sizeof(uint32_t)), 0x10u)
#define OT_MBX_SYS_APERTURE \
    DIV_ROUND_UP((OT_MBX_SYS_REGS_COUNT * sizeof(uint32_t)), 0x10u)

#endif /* HW_OPENTITAN_OT_MBX_H */