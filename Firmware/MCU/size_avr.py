"""
This script prints the size of the ELF file using avr-size.
"""

Import("env", "projenv")

# Print the size of the ELF file
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(
        'avr-size --mcu=${BOARD_MCU} -C "$BUILD_DIR/${PROGNAME}.elf"', ""
    ),
)
