# Adapted from distutils documentation

from setuptools import setup, Extension

extension = Extension('measurement_kit._bindings',
                      language = "c++",
                      extra_compile_args = ['-std=c++14'],
                      libraries = ['measurement_kit'],
                      sources = ['measurement_kit/_bindings.cpp'])

setup(name = 'measurement_kit',
      version = '0.3.0-dev.1',
      description = 'Portable C++11 network measurement library',
      author = 'Simone Basso',
      author_email = 'bassosimone@gmail.com',
      url = 'https://measurement-kit.github.io/',
      long_description = '''
Portable C++11 network measurement library (Python bindings)
''',
      packages = ["measurement_kit"],
      ext_modules = [
          extension,
          Extension("measurement_kit.pybind",
                    language="c++",
                    extra_compile_args=["-std=c++14"],
                    libraries=["measurement_kit"],
                    sources=[
                        "measurement_kit/pybind/module.cpp",
                        "measurement_kit/pybind/compat-0.3.cpp"
                    ])
      ])
