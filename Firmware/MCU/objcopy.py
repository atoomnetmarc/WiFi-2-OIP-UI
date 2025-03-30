"""
This script adds post-build actions to the PlatformIO environment to generate
various binary files from the ELF file using objcopy.
"""

import sys

Import("env")

# Generate .hex file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "ihex",
                "-j",
                ".text",
                "-j",
                ".data",
                "-j",
                ".bss",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}.hex",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}.hex",
    ),
)

# Generate .bin file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "binary",
                "-j",
                ".text",
                "-j",
                ".data",
                "-j",
                ".bss",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}.bin",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}.bin",
    ),
)

# Generate .eeprom.hex file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "ihex",
                "-j",
                ".eeprom",
                "--set-section-flags",
                '.eeprom="alloc,load"',
                "--change-section-lma",
                ".eeprom=0",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}-eeprom.hex",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}-eeprom.hex",
    ),
)

# Generate .eeprom.bin file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "binary",
                "-j",
                ".eeprom",
                "--set-section-flags",
                '.eeprom="alloc,load"',
                "--change-section-lma",
                ".eeprom=0",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}-eeprom.bin",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}-eeprom.bin",
    ),
)

# Generate .fuse.bin file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "binary",
                "-j",
                ".fuse",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}-fuse.bin",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}-fuse.bin",
    ),
)

# Generate .lock.bin file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "binary",
                "-j",
                ".lock",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}-lock.bin",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}-lock.bin",
    ),
)

# Generate .signature.bin file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(
        " ".join(
            [
                "$OBJCOPY",
                "-O",
                "binary",
                "-j",
                ".signature",
                "$BUILD_DIR/${PROGNAME}.elf",
                "$BUILD_DIR/${PROGNAME}-signature.bin",
            ]
        ),
        "Building $BUILD_DIR/${PROGNAME}-signature.bin",
    ),
)
