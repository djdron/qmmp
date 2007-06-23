# This script configures PyQMMP and generates the Makefiles.
#
# Copyright (c) 2004
# 	Integrated Computer Solutions, Inc. <info@ics.com>
#
# This file is part of PyQMMP.
#
# This copy of PyQMMP is licensed for use under the terms of the
# PyQMMP Commercial License Agreement version ICS-2.6.  See the file
# LICENSE for more details.
#
# PyQMMP is supplied WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


import sys
import os
import string
import glob
import getopt
import shutil
import py_compile

import sipconfig
# Where we must get a pyqtconfig module
from PyQt4 import pyqtconfig


# Initialise the globals.
pyqmmp_version = 0x0001
pyqmmp_version_str = "0.0.1"

sip_min_v3_version = 0x030b00
sip_min_v4_version = 0x040100
qt_min_version = 0x030000

#qmmp_sip_flags = []
#qmmp_version = None
qmmp_define = ""

# Get the PyQt configuration.
pyqtcfg = pyqtconfig.Configuration()

if pyqtcfg.qt_version == 0:
    sipconfig.error("SIP has been built with Qt support disabled.")

# Command line options.
opt_pyqmmpmoddir = pyqtcfg.default_mod_dir
opt_pyqmmpsipdir = pyqtcfg.default_sip_dir
opt_qmmpincdir = "../" #pyqtcfg.qt_inc_dir
opt_qmmplibdir = "../" #pyqtcfg.qt_lib_dir
opt_static = 0
opt_debug = 0
opt_concat = 0
opt_split = 1
opt_releasegil = 0
opt_tracing = 0
opt_verbose = 0


def usage(rcode = 2):
    """Display a usage message and exit.

    rcode is the return code passed back to the calling process.
    """
    print "Usage:"
    print "    python configure.py [-h] [-c] [-d dir] [-g] [-j #] [-k] [-n dir] [-o dir] [-r] [-u] [-v dir] [-w]"
    print "where:"
    print "    -h      display this help message"
    print "    -c      concatenate each module's C/C++ source files"
    print "    -d dir  where the PyQMMP modules will be installed [default %s]" % opt_pyqmmpmoddir
    print "    -g      always release the GIL (SIP v3.x behaviour)"
    print "    -j #    split the concatenated C++ source files into # pieces [default 1]"
    print "    -k      build the PyQMMP modules as static libraries"
    print "    -n dir  the directory containing the QicsTable header files [default %s]" % opt_qmmpincdir
    print "    -o dir  the directory containing the QicsTable library [default %s]" % opt_qmmplibdir
    print "    -r      generate code with tracing enabled [default disabled]"
    print "    -u      build with debugging symbols"
    print "    -v dir  where the PyQMMP .sip files will be installed [default %s]" % opt_pyqmmpsipdir
    print "    -w      don't suppress compiler output during configuration"

    sys.exit(rcode)


def inform_user():
    """Tell the user the option values that are going to be used.
    """
    sipconfig.inform("The PyQMMP modules will be installed in %s." % opt_pyqmmpmoddir)
    sipconfig.inform("The PyQMMP .sip files will be installed in %s." % opt_pyqmmpsipdir)


def create_config(module, template):
    """Create the PyQMMP configuration module so that it can be imported
    by build scripts.

    module is the module file name.
    template is the template file name.
    """
    sipconfig.inform("Creating %s..." % module)

    content = {
        "pyqmmp_version":              pyqmmp_version,
        "pyqmmp_version_str":          pyqmmp_version_str,
        "pyqmmp_mod_dir":              opt_pyqmmpmoddir,
        "pyqmmp_sip_dir":              opt_pyqmmpsipdir,
        "pyqmmp_qmmp_sip_flags":  qmmp_sip_flags,

        # These are internal.
        "_pyqmmp_defines":             qmmp_define,
        "_pyqmmp_inc_dir":             opt_qmmpincdir,
        "_pyqmmp_lib_dir":             opt_qmmplibdir
    }

    sipconfig.create_config_module(module, template, content)


def check_qmmp():
    """See if QicsTable can be found and what its version is.
    """
    # Find the QicsTable header files.
    nspace = os.path.join(opt_qmmpincdir, "QicsNamespace.h")

    if os.access(nspace, os.F_OK):
        # Get the version number string.
        _, vstr = sipconfig.read_version(nspace, "QicsTable", None, "QICSTABLE_VERSION")

        # Extract the version number and hope that the format is consistent.
        try:
            vlist = string.split(vstr)
            maj, min, bug = string.split(vlist[1], ".")
            versnr = (int(maj) << 16) | (int(min) << 8) | int(bug)
            vstr = string.join(vlist[1:])
        except:
            raise ValueError, "Unable to extract version number from QICSTABLE_VERSION in QicsNamespace.h"

        if glob.glob(os.path.join(opt_qmmplibdir, "*qmmp*")):
            global qmmp_version
            qmmp_version = versnr

            sipconfig.inform("QicsTable %s is being used." % vstr)
        else:
            sipconfig.inform("The QicsTable library could not be found in %s and so the qmmp module will not be built. If QicsTable is installed then use the -o argument to explicitly specify the correct directory." % opt_qmmplibdir)
    else:
        sipconfig.inform("QicsNamespace.h could not be found in %s and so the qmmp module will not be built. If QicsTable is installed then use the -n argument to explicitly specify the correct directory." % opt_qmmpincdir)


def set_sip_flags():
    """Set the SIP platform, version and feature flags.
    """
    global qmmp_sip_flags
    # For new version of PyQt (same 4 >) we must use pyqtcfg.pyqt_sip_flags but not pyqtcfg.pyqt_qt_sip_flags
    qmmp_sip_flags = string.split(pyqtcfg.pyqt_sip_flags)

    # Handle the QicsTable version tag.  At the moment QicsTable doesn't have
    # multiple versions.  When it does, add them and the %Timeline tags to this
    # disctionary.
    qmmp_tags = {
    }

    if qmmp_tags:
        qmmp_sip_flags.append("-t")
        qmmp_sip_flags.append(sipconfig.version_to_sip_tag(qmmp_version, qmmp_tags, "QicsTable"))


def generate_code(mname, imports=None, extra_cflags=None, extra_cxxflags="-I%s/Qt3Support/ -I%s/QtXml/ -I%s/QtGui/ -I%s -I../../ -L../../" % (pyqtcfg.qt_inc_dir, pyqtcfg.qt_inc_dir, pyqtcfg.qt_inc_dir, pyqtcfg.qt_inc_dir), extra_define=None, extra_include_dir=None, extra_lflags="-lQt3Support", extra_lib_dir=None, extra_lib=None, sip_flags=None):
    """Generate the code for a module.

    mname is the name of the module.
    imports is the list of PyQt modules that this one %Imports.
    extra_cflags is a string containing additional C compiler flags.
    extra_cxxflags is a string containing additional C++ compiler flags.
    extra_define is a name to add to the list of preprocessor defines.
    extra_include_dir is the name of a directory to add to the list of include
    directories.
    extra_lflags is a string containing additional linker flags.
    extra_lib_dir is the name of a directory to add to the list of library
    directories.
    extra_lib is the name of an extra library to add to the list of libraries.
    sip_flags is the list of sip flags to use instead of the defaults.
    """
    sipconfig.inform("Generating the C++ source for the %s module..." % mname)

    try:
        shutil.rmtree(mname)
    except:
        pass

    try:
        os.mkdir(mname)
    except:
        sipconfig.error("Unable to create the %s directory." % mname)

    # Build the SIP command line.
    argv = [pyqtcfg.sip_bin]

    if sip_flags:
        argv.extend(sip_flags)

    if opt_concat:
        argv.append("-j")
        argv.append(str(opt_split))

    if opt_tracing:
        argv.append("-r")

    if opt_releasegil:
        argv.append("-g")

    argv.append("-c")
    argv.append(mname)

    buildfile = os.path.join(mname, mname + ".sbf")
    argv.append("-b")
    argv.append(buildfile)

    argv.append("-I")
    argv.append(pyqtcfg.pyqt_sip_dir)

    # SIP assumes POSIX style path separators.
    argv.append(string.join(["sip", mname, mname + "mod.sip"], "/"))

    os.system(string.join(argv))

    # Check the result.
    if not os.access(buildfile, os.F_OK):
        sipconfig.error("Unable to create the C++ code.")

    # Compile the Python stub.
    if pyqtcfg.sip_version < 0x040000:
        sipconfig.inform("Compiling %s.py..." % mname)
        py_compile.compile(os.path.join(mname, mname + ".py"), os.path.join(mname, mname + ".pyc"))

    # Generate the Makefile.
    sipconfig.inform("Creating the Makefile for the %s module..." % mname)

    installs = []

    if pyqtcfg.sip_version >= 0x040000:
        warnings = 1
    else:
        warnings = 0
        installs.append([[mname + ".py", mname + ".pyc"], opt_pyqmmpmoddir])

    sipfiles = []

    for s in glob.glob("sip/" + mname + "/*.sip"):
        sipfiles.append(os.path.join("..", "sip", mname, os.path.basename(s)))

    installs.append([sipfiles, os.path.join(opt_pyqmmpsipdir, mname)])

    makefile = sipconfig.SIPModuleMakefile(
        configuration=pyqtcfg,
        build_file=mname + ".sbf",
        dir=mname,
        install_dir=opt_pyqmmpmoddir,
        installs=installs,
        qt=1,
        warnings=warnings,
        static=opt_static,
        debug=opt_debug
    )

    if extra_cflags:
        makefile.extra_cflags.append(extra_cflags)

    if extra_cxxflags:
        makefile.extra_cxxflags.append(extra_cxxflags)

    if extra_define:
        makefile.extra_defines.append(extra_define)

    if extra_include_dir:
        makefile.extra_include_dirs.append(extra_include_dir)

    if extra_lflags:
        makefile.extra_lflags.append(extra_lflags)

    if extra_lib_dir:
        makefile.extra_lib_dirs.append(extra_lib_dir)

    if extra_lib:
        makefile.extra_libs.append(extra_lib)

    if pyqtcfg.sip_version < 0x040000 and imports:
        # Inter-module links.
        for im in imports:
            makefile.extra_lib_dirs.insert(0, pyqtcfg.pyqt_mod_dir)
            makefile.extra_libs.insert(0, makefile.module_as_lib(im))

    makefile.generate()


def check_license():
    """Handle the validation of the PyQMMP license.
    """
    try:
        import license
        ltype = license.LicenseType
        lname = license.LicenseName

        try:
            lfile = license.LicenseFile
        except AttributeError:
            lfile = None
    except ImportError:
        ltype = None

    if ltype is None:
        ltype = "GPL"
        lname = "GNU General Public License"
        lfile = None

    sipconfig.inform("This is the %s version of PyQMMP %s (licensed under the %s) for Python %s on %s." % (ltype, pyqmmp_version_str, lname, string.split(sys.version)[0], sys.platform))

    # Common checks.
    if ltype == "GPL" and sys.platform == "win32":
        error("You cannot use the GPL version of PyQMMP under Windows.")

    try:
        qted = pyqtcfg.qt_edition
    except AttributeError:
        qted = None

    if qted:
        if (qted == "free" and ltype != "GPL") or (qted != "free" and ltype == "GPL"):
            sipconfig.error("This version of PyQMMP and the %s edition of Qt have incompatible licenses." % qted)

    # Confirm the license.
    print
    print "Type 'L' to view the license."
    print "Type 'yes' to accept the terms of the license."
    print "Type 'no' to decline the terms of the license."
    print

    while 1:
        try:
            resp = raw_input("Do you accept the terms of the license? ")
        except:
            resp = ""

        resp = string.lower(string.strip(resp))

        if resp == "yes":
            break

        if resp == "no":
            sys.exit(0)

        if resp == "l":
            os.system("more LICENSE")

    # If there should be a license file then check it is where it should be.
    if lfile:
        if os.access(os.path.join("sip", lfile), os.F_OK):
            sipconfig.inform("Found the license file %s." % lfile)
        else:
            sipconfig.error("Please copy the license file %s to the sip directory." % lfile)


def create_makefiles():
    """Create the additional Makefiles.
    """
    sipconfig.inform("Creating top level Makefile...")

    sipconfig.ParentMakefile(
        configuration=pyqtcfg,
        subdirs=["qmmp"],
        installs=("pyqmmpconfig.py", opt_pyqmmpmoddir)
    ).generate()


def main(argv):
    """Create the configuration module module.

    argv is the list of command line arguments.
    """
    try:
        optlist, args = getopt.getopt(argv[1:], "hcd:gj:kn:o:ruv:w")
    except getopt.GetoptError:
        usage()

    global opt_pyqmmpmoddir, opt_pyqmmpsipdir
    global opt_qmmpincdir, opt_qmmplibdir
    global opt_static, opt_debug, opt_concat, opt_releasegil, opt_split
    global opt_tracing, opt_verbose

    """
    for opt, arg in optlist:
        if opt == "-h":
            usage(0)
        elif opt == "-c":
            opt_concat = 1
        elif opt == "-d":
            opt_pyqmmpmoddir = arg
        elif opt == "-g":
            opt_releasegil = 1
        elif opt == "-j":
            try:
                opt_split = int(arg)
            except:
                usage()
        elif opt == "-k":
            opt_static = 1
        elif opt == "-n":
            opt_qmmpincdir = arg
        elif opt == "-o":
            opt_qmmplibdir = arg
        elif opt == "-r":
            opt_tracing = 1
        elif opt == "-u":
            opt_debug = 1
        elif opt == "-v":
            opt_pyqmmpsipdir = arg
        elif opt == "-w":
            opt_verbose = 1

    if args:
        usage()
    """

    #check_license()

    sipconfig.inform("SIP %s is being used." % pyqtcfg.sip_version_str)

    # Check SIP is new enough.
    if pyqtcfg.sip_version_str[:8] != "snapshot":
        minv = None

        if pyqtcfg.sip_version >= 0x040000:
            if pyqtcfg.sip_version < sip_min_v4_version:
                minv = sip_min_v4_version
        else:
            if pyqtcfg.sip_version < sip_min_v3_version:
                minv = sip_min_v3_version

        if minv:
            sipconfig.error("This version of PyQMMP requires SIP v%s or later" % sipconfig.version_to_string(minv))

    # Check for QicsTable.
    #check_qmmp()

    # If QicsTable wasn't found then there is no point carrying on.  An
    # appropriate error message will already have been displayed.
    #if qmmp_version is None:
     #   sys.exit(1)

    # Set the SIP platform, version and feature flags.
    set_sip_flags()

    # Tell the user what's been found.
    inform_user()

    lib_dir = "../../"
    #if opt_qmmplibdir != "../lib":
	 #   lib_dir = opt_qmmplibdir

    # Generate the code.
    generate_code("qmmp", imports=["PyQt4"], extra_define=qmmp_define, extra_include_dir=opt_qmmpincdir, extra_lib_dir=lib_dir, extra_lib="qmmp", sip_flags=qmmp_sip_flags)

    # Create the additional Makefiles.
    create_makefiles()

    # Install the configuration module.
    create_config("pyqmmpconfig.py", "pyqmmpconfig.py.in")


###############################################################################
# The script starts here.
###############################################################################

if __name__ == "__main__":
    try:
        main(sys.argv)
    except SystemExit:
        raise
    except:
        print \
"""An internal error occured.  Please report all the output from the program,
including the following traceback, to support@ics.com.
"""
        raise
