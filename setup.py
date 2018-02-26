from distutils.core import setup, Extension
import sys

# to avoid compilation warning
import distutils.sysconfig
cfg_vars = distutils.sysconfig.get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace("-Wstrict-prototypes", "").replace("-DNDEBUG", "-DDEBUG")

mymodule = Extension('fourmy',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include'],
                    libraries = ['CGAL', 'gmp', 'mpfr', 'boost_python-py{}{}'.format(*sys.version_info[:2])],

                    sources = ['src/fourmy.cc'])

setup (name = 'fourmy',
       version = '0.1',
       description = 'Some functions from CGAL for GIS, it does not aim to become full CGAL bindings. The interface is modeled after shapely.',
       ext_modules = [mymodule])
