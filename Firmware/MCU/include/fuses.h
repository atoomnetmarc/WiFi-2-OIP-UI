/*

Copyright 2024-2025 Marc Ketel
SPDX-License-Identifier: Apache-2.0

*/

#include <avr/io.h>
#include <avr/signature.h>

#ifndef fuses_h
#define fuses_h

LOCKBITS = LB_MODE_1;
FUSES = {
    .low = (FUSE_SUT1 & FUSE_CKSEL3),
    .high = (FUSE_SPIEN & FUSE_WDTON & FUSE_EESAVE & FUSE_BODLEVEL1),
    .extended = (FUSE_SELFPRGEN),
};

#endif