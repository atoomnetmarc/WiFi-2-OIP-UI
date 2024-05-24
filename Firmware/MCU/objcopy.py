import sys

Import("env")

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-j", ".text", "-j", ".data", "-j", ".bss",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}.hex"
    ]), "Building $BUILD_DIR/${PROGNAME}.hex again")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary", "-j", ".text", "-j", ".data", "-j", ".bss",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}.bin")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "ihex", "-j", ".eeprom", "--set-section-flags", ".eeprom=\"alloc,load\"", "--change-section-lma", ".eeprom=0",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}-eeprom.hex"
    ]), "Building $BUILD_DIR/${PROGNAME}-eeprom.hex")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary", "-j", ".eeprom", "--set-section-flags", ".eeprom=\"alloc,load\"", "--change-section-lma", ".eeprom=0",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}-eeprom.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}-eeprom.bin")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary", "-j", ".fuse",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}-fuse.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}-fuse.bin")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary", "-j", ".lock",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}-lock.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}-lock.bin")
)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.hex",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary", "-j", ".signature",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}-signature.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}-signature.bin")
)
