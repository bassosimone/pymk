# Adapted from distutils documentation

from distutils.core import setup, Extension

module1 = Extension('_mk',
                    language = "c++",
                    extra_compile_args = ['-std=c++11'],
                    include_dirs = ['/usr/local/include'],
                    libraries = ['measurement_kit'],
                    library_dirs = ['/usr/local/lib'],
                    sources = ['py_module.cpp'])

setup (name = 'measurement_kit',
       version = '1.0',
       description = 'Portable C++11 network measurement library',
       author = 'Simone Basso',
       author_email = 'bassosimone@gmail.com',
       url = 'https://measurement-kit.github.io/',
       long_description = '''
Portable C++11 network measurement library (Python bindings)
''',
       ext_modules = [module1])
