# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

""" MeasurementKit bindings for Python """

from __future__ import print_function
from . import _bindings as _mk

# Note: the following call would raise an error if there is a mismatch between
# the version of MK we compiled with and the one we link with
MK_LIBRARY_VERSION = _mk.library_version()
print("MeasurementKit version:", MK_LIBRARY_VERSION)

MK_BINDINGS_VERSION = "1"
__version__ = MK_LIBRARY_VERSION + "-" + MK_BINDINGS_VERSION


# Note: keep this in sync with <measurement_kit/logger.hpp>
MK_LOG_WARNING = 0
MK_LOG_INFO = 1
MK_LOG_DEBUG = 2
MK_LOG_DEBUG2 = 3
MK_LOG_VERBOSITY_MASK = 31
MK_LOG_JSON = 32


class _BaseTest(object):
    """ Base class for all MeasurementKit tests """

    def __init__(self, name):
        self._handle = _mk.create(name)

    def __del__(self):
        _mk.destroy(self._handle)

    def set_verbosity(self, value):
        """ Set verbosity of the test's private logger """
        _mk.set_verbosity(self._handle, value)
        return self

    def increase_verbosity(self):
        """ Make the test's private logger more verbose """
        _mk.increase_verbosity(self._handle)
        return self

    def on_log(self, callback):
        """ Set function to be called when a log line is produced """
        _mk.on_log(self._handle, callback)
        return self

    def on_entry(self, callback):
        """ Set function to be called when a test entry is produced """
        _mk.on_entry(self._handle, callback)
        return self

    def set_input_filepath(self, value):
        """ Set file path where to read the input from """
        _mk.set_input_filepath(self._handle, value)
        return self

    def set_output_filepath(self, value):
        """ Set file path where to write the output into """
        _mk.set_output_filepath(self._handle, value)
        return self

    def set_options(self, key, value):
        """ Set a specific option of the test (see MeasurementKit manual) """
        _mk.set_options(self._handle, key, value)
        return self

    def run(self):
        """ Run the test and block until the test is complete """
        _mk.run(self._handle)

    def run_async(self, callback):
        """ Run the test and call the callback when it is complete """
        _mk.run_async(self._handle, callback)

    def run_deferred(self):
        """ Run the test and fire the deferred's callback when done """
        from twisted.internet import reactor, defer
        done = defer.Deferred()

        def callback():
            """ Function called when test is complete """
            reactor.callInThread(
                lambda: reactor.callFromThread(done.callback, None)
            )

        _mk.run_async(self._handle, callback)
        return done


class DnsInjection(_BaseTest):
    """ OONI's dns-injection test """

    def __init__(self):
        super(self.__class__, self).__init__(b"dns_injection")


class HttpInvalidRequestLine(_BaseTest):
    """ OONI's http-invalid-request-line test """

    def __init__(self):
        super(self.__class__, self).__init__(b"http_invalid_request_line")


class NdtTest(_BaseTest):
    """ The network-diagnostic-tool's test """

    def __init__(self):
        super(self.__class__, self).__init__(b"ndt")


class TcpConnect(_BaseTest):
    """ OONI's tcp-connect test """

    def __init__(self):
        super(self.__class__, self).__init__(b"tcp_connect")


class WebConnectivity(_BaseTest):
    """ OONI's web-connectivity test """

    def __init__(self):
        super(self.__class__, self).__init__(b"web_connectivity")
