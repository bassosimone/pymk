from distutils.core import setup, Extension

module1 = Extension('_mk',
                    language="c++",
                    include_dirs = ['/usr/local/include'],
                    libraries = ['measurement_kit'],
                    library_dirs = ['/usr/local/lib'],
                    extra_compile_args=['-std=c++11'],
                    sources = ['py_module.cpp'])

setup (name = 'measurement_kit',
       version = '1.0',
       description = 'This is a demo package',
       author = 'Martin v. Loewis',
       author_email = 'bassosimone@gmail.com',
       url = 'https://measurement-kit.github.io/',
       long_description = '''
Portable C++11 network measurement library (Python bindings)
''',
       ext_modules = [module1])
