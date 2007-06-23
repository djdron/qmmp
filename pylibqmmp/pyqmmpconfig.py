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
#
# This module is intended to be used by the configuration scripts of extension
# modules that %Import PyQMMP modules.


import pyqtconfig


# These are installation specific values created when PyQMMP was
# configured.
_pkg_config = {
    '_pyqmmp_defines':        '',
    '_pyqmmp_inc_dir':        '../',
    '_pyqmmp_lib_dir':        '../',
    'pyqmmp_mod_dir':         '/usr/lib/python2.5/site-packages',
    'pyqmmp_qmmp_sip_flags':  '-x VendorID -t WS_X11 -x PyQt_NoPrintRangeBug -t Qt_4_2_0',
    'pyqmmp_sip_dir':         '/usr/share/sip',
    'pyqmmp_version':         0x000001,
    'pyqmmp_version_str':     '0.0.1'
}

_default_macros = None


class Configuration(pyqtconfig.Configuration):
    """The class that represents PyQMMP configuration values.
    """
    def __init__(self, sub_cfg=None):
        """Initialise an instance of the class.

        sub_cfg is the list of sub-class configurations.  It should be None
        when called normally.
        """
        if sub_cfg:
            cfg = sub_cfg
        else:
            cfg = []

        cfg.append(_pkg_config)

        pyqtconfig.Configuration.__init__(self, cfg)


class QicsTableModuleMakefile(pyqtconfig.QtModuleMakefile):
    """The Makefile class for modules that %Import qmmp.
    """
    def finalise(self):
        """Finalise the macros.
        """
        if self.config.sip_version < 0x040000:
            self.extra_libs.append(self.module_as_lib("qmmp"))

        self.extra_defines.extend(self.config._pyqmmp_defines)
        self.extra_include_dirs.append(self.config._pyqmmp_inc_dir)
        self.extra_lib_dirs.append(self.config._pyqmmp_lib_dir)
        self.extra_libs.append("qmmp")

        pyqtconfig.QtModuleMakefile.finalise(self)
