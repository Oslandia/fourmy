from distutils.core import setup, Extension
import sys
import os

# to avoid compilation warning
import distutils.sysconfig
cfg_vars = distutils.sysconfig.get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace("-Wstrict-prototypes", "").replace("-DNDEBUG", "-DDEBUG")

path_separator = ":" if os.name == "posix" else ";"
cgal = "CGAL" if "CGAL_LIBNAME" not in os.environ else os.environ["CGAL_LIBNAME"]
gmp = "gmp"  if "GMP_LIBNAME" not in os.environ else os.environ["GMP_LIBNAME"]
mpfr = "mpfr"  if "MPFR_LIBNAME" not in os.environ else os.environ["MPFR_LIBNAME"]
boost_python = 'boost_python-py{}{}'.format(*sys.version_info[:2]) if "BOOSTPYTHON_LIBNAME" not in os.environ else os.environ["BOOSTPYTHON_LIBNAME"]

mymodule = Extension('fourmy._fourmy',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = [] if "INCLUDE_PATH" not in os.environ else os.environ["INCLUDE_PATH"].split(path_separator),
                    library_dirs = [] if "LIBRARY_PATH" not in os.environ else os.environ["LIBRARY_PATH"].split(path_separator),
                    libraries = [cgal, gmp, mpfr, boost_python],

                    sources = ['fourmy/_fourmy.cc'])

setup (name = 'fourmy',
       version = '0.0.3',
       description = 'Some functions from CGAL for GIS, it does not aim to become full CGAL bindings. The interface is modeled after shapely.',
       ext_modules = [mymodule],
       packages=['fourmy'])
