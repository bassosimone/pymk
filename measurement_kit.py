# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

from __future__ import print_function

import _measurement_kit as _mk

# Note: the following call would raise an error if there is a mismatch between
# the version of MK we compiled with and the one we link with
MK_LIBRARY_VERSION = _mk.library_version()
print("MeasurementKit version:", MK_LIBRARY_VERSION)


# Note: keep this in sync with <measurement_kit/logger.hpp>
MK_LOG_WARNING = 0
MK_LOG_INFO = 1
MK_LOG_DEBUG = 2
MK_LOG_DEBUG2 = 3
MK_LOG_VERBOSITY_MASK = 31
MK_LOG_JSON = 32


class _BaseTest(object):

    def __init__(self, name):
        self._handle = _mk.create(name)

    def __del__(self):
        _mk.destroy(self._handle)

    def set_verbosity(self, value):
        _mk.set_verbosity(self._handle, value)
        return self

    def increase_verbosity(self):
        _mk.increase_verbosity(self._handle)
        return self

    def on_log(self, callback):
        _mk.on_log(self._handle, callback)
        return self

    def set_input_filepath(self, value):
        _mk.set_input_filepath(self._handle, value)
        return self

    def set_output_filepath(self, value):
        _mk.set_output_filepath(self._handle, value)
        return self

    def set_options(self, key, value):
        _mk.set_options(self._handle, key, value)
        return self

    def run(self):
        _mk.run(self._handle)

    def run_async(self, callback):
        _mk.run_async(self._handle, callback)

    def run_deferred(self):
        from twisted.internet import defer
        d = defer.Deferred()
        def callback():
            d.callback(None)
        _mk.run_async(self._handle, callback)
        return d


class DnsInjection(_BaseTest):
    def __init__(self):
        super(self.__class__, self).__init__(b"dns_injection")


class HttpInvalidRequestLine(_BaseTest):
    def __init__(self):
        super(self.__class__, self).__init__(b"http_invalid_request_line")


class NdtTest(_BaseTest):
    def __init__(self):
        super(self.__class__, self).__init__(b"ndt")


class TcpConnect(_BaseTest):
    def __init__(self):
        super(self.__class__, self).__init__(b"tcp_connect")


class WebConnectivity(_BaseTest):
    def __init__(self):
        super(self.__class__, self).__init__(b"web_connectivity")
