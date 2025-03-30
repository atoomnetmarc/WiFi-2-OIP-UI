"""
This script generates a list file (.lss) from the ELF file using avr-objdump.
The list file contains disassembled code and other information.
"""

Import("env", "projenv")

# Generate .lss file from .elf file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(
        'avr-objdump -h -S "$BUILD_DIR/${PROGNAME}.elf" > "hex/'
        + env["PIOENV"]
        + '.lss"',
        "Creating $BUILD_DIR/${PROGNAME}.lss",
    ),
)
