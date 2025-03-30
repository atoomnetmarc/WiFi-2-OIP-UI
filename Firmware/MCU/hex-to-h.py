"""
This script converts binary files (signature, fuse, lock, eeprom, flash)
into a C header file (node_firmware.h) and writes a build timestamp
to another header file (node_firmware_build_time.h).
"""

import time

Import("env")

# Define the header file for the build timestamp
header_file = "include/node_firmware_build_time.h"

# Write the build timestamp to the header file
with open(header_file, "w") as f:
    f.write("// Automatically generated every time you build the project.\n\n")
    now = int(time.time())
    f.write("#define NODE_FIRMWARE_BUILD_TIME {}UL\n".format(now))
    f.write('#define NODE_FIRMWARE_BUILD_TIME_STR "{}"\n'.format(now))


def convert_file_to_h(source, target, env):
    """
    Converts binary files (signature, fuse, lock, eeprom, flash) into a
    C header file (node_firmware.h).
    """
    with open("include/node_firmware.h", "w") as fh:
        fh.write("// Automatically generated every time you build the project.\n\n")
        fh.write("#include <stdint.h>\n\n")
        fh.write("#ifndef node_firmware_h\n")
        fh.write("#define node_firmware_h\n\n")

        # Device signature
        fh.write(
            "// The device signature this firmware is valid for.\nconst uint8_t node_signature[] = {\n"
        )
        with open(env.subst("$BUILD_DIR/${PROGNAME}-signature.bin"), "rb") as f:
            data = f.read()
        # Reorder signature from msb-lsb to lsb-msb.
        c_array = ", ".join(["0x{:02X}".format(byte) for byte in reversed(data)])
        fh.write(c_array)
        fh.write("\n};\n\n")

        # Fuse bits
        fh.write(
            "// Low, high and extended fuse bits.\nconst uint8_t node_fuse[] = {\n"
        )
        with open(env.subst("$BUILD_DIR/${PROGNAME}-fuse.bin"), "rb") as f:
            data = f.read()
        c_array = ", ".join(["0x{:02X}".format(byte) for byte in data])
        fh.write(c_array)
        fh.write("\n};\n\n")

        # Lock bits
        fh.write("// Lock bits.\nconst uint8_t node_lock = ")
        with open(env.subst("$BUILD_DIR/${PROGNAME}-lock.bin"), "rb") as f:
            data = f.read()
        c_array = ", ".join(["0x{:02X}".format(byte) for byte in data])
        fh.write(c_array)
        fh.write(";\n\n")

        # Eeprom data
        fh.write("// Eeprom.\nconst uint8_t node_eeprom[] = {\n")
        with open(env.subst("$BUILD_DIR/${PROGNAME}-eeprom.bin"), "rb") as f:
            data = f.read()
        c_array = ", ".join(["0x{:02X}".format(byte) for byte in data])
        fh.write(c_array)
        fh.write("\n};\n\n")

        # Flash data
        fh.write("// Flash.\nconst uint8_t node_flash[] = {\n")
        with open(env.subst("$BUILD_DIR/${PROGNAME}.bin"), "rb") as f:
            data = f.read()
        c_array = ", ".join(["0x{:02X}".format(byte) for byte in data])
        fh.write(c_array)
        fh.write("\n};\n\n")

        fh.write("#endif\n")


env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", convert_file_to_h)
