"""Common script objects."""

__id__ = '$Id: hoffload_common.py 697795 2017-05-05 02:51:21Z nisar $'
__url__ = '$URL: svn://bcawlan-svn.lvn.broadcom.net/svn/bcawlan/components/chips/scripts/branches/CHIPSSCRIPTS_BRANCH_17_100/hoffload_common.py $'

#
# $ Copyright Broadcom Corporation $
#
# <<Broadcom-WL-IPTag/Proprietary:>>
#

import argparse
import json
import logging
import mmap
import os
import subprocess
import sys

AUTO_GEN = '/* Auto Generated by %s, DO NOT EDIT! */'
HOFFLOAD_MODULE_ID = 'hoffload_module_id_t'
BCM_HOFFLOAD = 'BCM_HOFFLOAD_MODULE_'
HOFFLOAD_MEM_TAG = 'hoffload_dev_addr'


class Error(Exception):
    """Errors handled by applications."""

    def __init__(self, msg):
        super(Error, self).__init__(msg)
        self.msg = sys.argv[0] + ': Error: ' + msg

    def __str__(self):
        return self.msg


class HoffloadApp(object):
    """Application class proto."""

    def __init__(self, args):
        self.args = args
        self.cm = args.cm

    def __call__(self):
        """Default entry proto."""


class Common(object):
    """Common methods."""

    def __init__(self, name=None):
        self.name = (name if name else
                     os.path.splitext(os.path.basename(
                         sys.argv[0]))[0])
        self.set_logging()

    def set_logging(self):
        """Set up logging."""

        logging.basicConfig(
            format='%%(asctime)s %s %%(message)s' % self.name,
            datefmt='%Y%m%d.%H%M%S')

    @staticmethod
    def set_logging_level(level):
        """ Set logging level."""

        logging.getLogger().setLevel(level)

    @staticmethod
    def run_cmd(*cmd, **kws):
        """Run command and get its output."""

        logging.debug('+' + ' '.join(cmd))
        output = subprocess.check_output(cmd, **kws)
        return output.splitlines()


class HoffloadConfigParser(object):
    """A tool to parse config file."""

    def __init__(self, config, enum=None):
        self.config = config
        self.module_table = {}
        self.elements = None
        self.modules = None
        self.parse_module_config()
        if enum:
            self.parse_module_enum(enum)

    def get_element(self, element_name):
        """Get the value of a given element name."""
        return self.elements[element_name]

    def get_module_id(self, module_name):
        """Get a module's id (hoffload_module_id_t enum) for a given
            module name.
        """

        module_id = self.get_module_enum(module_name)
        return self.module_table[module_id]

    def get_module_index(self, module_name):
        """Get a module's location index as computed using ordinal position
            of modules in config file.
        """

        for index, module in enumerate(self.modules):
            if module['name'] == module_name:
                return index
        else:
            raise Error('%s: %s not found' % (self.config, module_name))

    def get_module_enum(self, module_name):
        """Get a module's enum."""

        for module in self.modules:
            if module['name'] == module_name:
                return module['id']

    def parse_module_config(self):
        """Parse module config file and extract 'modules' ."""

        all_lines = ''
        with open(self.config) as config_file:
            for line in config_file:
                all_lines += line.strip()
            self.elements = json.loads(all_lines)
        self.modules = self.elements['modules']

    def parse_module_enum(self, enum):
        """Parse module enum header file."""

        with open(enum) as enum_file:
            data = mmap.mmap(enum_file.fileno(), 0, access=mmap.ACCESS_READ)

        # extract lines containing enum definition from the header file
        lines = []
        pos_end = data.find(HOFFLOAD_MODULE_ID)
        if pos_end != -1:
            pos_bgn = data.rfind('{', 0, pos_end)
            if pos_bgn != -1:
                lines = data[pos_bgn + 1:pos_end - 1].splitlines()

        # split the lines based on '='
        module_id = 0
        for line in lines:
            line = line.strip(',')
            if BCM_HOFFLOAD in line:
                tokens = line.split('=', 1)
                if len(tokens) > 1:
                    module_id = int(tokens[1].strip())
                # compute enum values if not specified explicitly
                self.module_table[tokens[0].strip()] = module_id
                module_id += 1


class CommandLine(object):
    """Command-line methods."""

    def __init__(self, cm=None):
        self.cm = cm if cm else Common()
        self.version = ''
        self.idkw = __id__
        self.urlkw = __url__
        self.desc = self.epilog = None
        self.call_args = ()
        self.call_kws = {}
        self.app_class = HoffloadApp
        self.args = self.parser = None

    def __call__(self):
        """Default entry point."""

        self.set_options()
        return self.start_app()

    def set_options(self):
        """Set command-line options and arguments."""

        class Formatter(argparse.ArgumentDefaultsHelpFormatter,
                        argparse.RawDescriptionHelpFormatter):
            """Combined formatter."""

        self.parser = parser = argparse.ArgumentParser(
            formatter_class=Formatter, description=self.desc,
            epilog=self.epilog, add_help=False)

        # Help options.
        # pylint: disable=protected-access
        parser.add_argument('-h', '-?', '--help',
                            action=argparse._HelpAction,
                            help='Show this message and exit')
        parser.add_argument('--version', action='version',
                            version='Version: ' + '; '.join(
                                [x for x in self.version,
                                 self.idkw.strip('$ '),
                                 self.urlkw.strip('$ ')
                                 if x]),
                            help='Print version and exit')
        parser.add_argument('--app-version', default=0,
                            help='Application version, defines ' +
                            'compatiblity with other components and "src"')
        parser.add_argument(
            '-t', '--tool-path-prefix', help='Tool path prefix',
            default='/projects/hnd/tools/linux/' +
            'hndtools-armeabi-2013.11/bin')
        # Result file.
        parser.add_argument('-r', '--result',
                            help='File with tool results')
        # Debugging option.
        parser.add_argument('-d', '--debug', action='store_true',
                            help='Enable debug logging')

    def get_args(self):
        """Get command-line arguments."""

        self.args = self.parser.parse_args()
        self.cm.set_logging_level(logging.DEBUG if self.args.debug else
                                  logging.INFO)

    def start_app(self):
        """Start application."""

        stat = self.get_args()
        if stat:
            return stat

        logging.debug('Start ' + ' '.join([
            (('"' + arg + '"') if ' ' in arg else arg)
            for arg in sys.argv]))
        stat = 0
        self.args.cm = self.cm
        try:
            self.app_class(self.args)(*self.call_args, **self.call_kws)
        except Error as err:
            logging.error(str(err))
            stat = 1

        logging.debug('End')
        return stat


# vim: ts=4:sw=4:tw=80:et
