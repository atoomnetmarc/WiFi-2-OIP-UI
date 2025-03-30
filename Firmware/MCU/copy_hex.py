"""
This script copies the generated HEX file to a specific directory
('hex/') with a name that includes the PlatformIO environment.
"""

Import("env", "projenv")
from shutil import copyfile


def copyhex(*args, **kwargs):
    """
    Copies the HEX file to the 'hex/' directory with a name that includes
    the PlatformIO environment.
    """
    copyfile(str(kwargs["target"][0]), "hex/" + env["PIOENV"] + ".hex")


env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", copyhex)
