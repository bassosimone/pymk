# MeasurementKit Python bindings

First you need to install MeasurementKit on your system. On a MacOS
system you can use the [brew
tap](https://github.com/bassosimone/homebrew-measurement-kit) to
install the *development version* (currently 0.3.0-dev):

```
brew tap bassosimone/measurement-kit
brew install --verbose --HEAD measurement-kit
```

Then,

```
brew install pybind11
virtualenv venv
source venv/bin/activate
pip install -r requirements.txt
python setup.py install
python tests/test_integration.py
python examples/web_connectivity.py
```
