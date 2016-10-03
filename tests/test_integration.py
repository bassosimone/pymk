# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

""" Integration tests for MeasurementKit Python bindings """

# pylint: disable=no-self-use

from __future__ import print_function
import time
import unittest

import measurement_kit

def run_from_bindings(test_name, verbosity, options):
    """ Run a specific tests using bindings """
    # pylint: disable=no-member
    from measurement_kit import _bindings as _mk
    handle = _mk.create(test_name)
    for key, value in options.items():
        _mk.set_options(handle, key, value)
    _mk.set_verbosity(handle, verbosity)
    again = [True]
    def on_complete():
        """ Function called when test is complete """
        print("test done")
        again[0] = False
    def on_log(severity, line):
        """ Function called for every produced log line """
        print("mk: <{}> {}".format(severity, line))
    _mk.on_log(handle, on_log)
    _mk.run_async(handle, on_complete)
    _mk.destroy(handle)  # Note: this MUST be possible
    while again[0]:
        time.sleep(1)
    # pylint: enable=no-member

class TestIntegrationCxxLibrary(unittest.TestCase):
    """ Integration test that uses directly the C++ library """

    def test_ndt(self):
        """ Runs NDT test """
        run_from_bindings("ndt", measurement_kit.MK_LOG_INFO, {
            "save_real_probe_ip": "1",
        })

VERBOSITY = measurement_kit.MK_LOG_INFO

def setup_dns_injection():
    """ Setups the dns-injection test """
    return measurement_kit.DnsInjection()                                      \
        .set_verbosity(VERBOSITY)                                              \
        .set_options(b"backend", b"8.8.8.1:53")                                \
        .set_input_filepath(b"fixtures/hosts.txt")

def setup_http_invalid_request_line():
    """ Setups the http-invalid-request-line test """
    return measurement_kit.HttpInvalidRequestLine()                            \
        .set_verbosity(VERBOSITY)                                              \
        .set_options(b"backend", b"http://213.138.109.232/")

def setup_ndt():
    """ Setups the network-diagnostic-tool test """
    return measurement_kit.NdtTest()                                           \
        .set_verbosity(VERBOSITY)

def setup_tcp_connect():
    """ Setups the tcp-connect test """
    return measurement_kit.TcpConnect()                                        \
        .set_verbosity(VERBOSITY)                                              \
        .set_options(b"port", b"80")                                           \
        .set_input_filepath(b"fixtures/hosts.txt")

def setup_web_connectivity():
    """ Setups the web-connectivity test """
    return measurement_kit.WebConnectivity()                                   \
        .set_verbosity(VERBOSITY)                                              \
        .set_options(b"nameserver", b"8.8.8.8:53")                             \
        .set_input_filepath(b"fixtures/urls.txt")

class TestIntegrationSync(unittest.TestCase):
    """ Integration test using sync wrappers """

    def test_dns_injection(self):
        """ Runs dns-injection test """
        setup_dns_injection().run()

    def test_http_invalid_request_line(self):
        """ Runs http-invalid-request-line test """
        setup_http_invalid_request_line().run()

    def test_ndt(self):
        """ Runs ndt test """
        setup_ndt().run()

    def test_tcp_connect(self):
        """ Runs tcp-connect test """
        setup_tcp_connect().run()

    def test_web_connectivity(self):
        """ Runs web-connectivity test """
        setup_web_connectivity().run()

def async_run_of(creator):
    """ Executes asynchronous run of the creator function """
    done = [False]
    def test_complete():
        """ Function called when test is complete """
        done[0] = True
    creator().run_async(test_complete)
    while not done[0]:
        time.sleep(1.0)

class TestIntegrationAsync(unittest.TestCase):
    """ Integration test using async wrappers """

    def test_dns_injection(self):
        """ Runs dns-injection test """
        async_run_of(setup_dns_injection)

    def test_http_invalid_request_line(self):
        """ Runs http-invalid-request-line test """
        async_run_of(setup_http_invalid_request_line)

    def test_ndt(self):
        """ Runs ndt test """
        async_run_of(setup_ndt)

    def test_tcp_connect(self):
        """ Runs tcp-connect test """
        async_run_of(setup_tcp_connect)

    def test_web_connectivity(self):
        """ Runs web-connectivity test """
        async_run_of(setup_web_connectivity)

def deferred_run_of(creator):
    """ Executes deferred run of the creator function """
    return creator().run_deferred()

class TestIntegrationDeferred(unittest.TestCase):
    """ Integration test using deferred wrappers """

    def test_all(self):
        # XXX I'd like to run each test separately but I don't know how
        # to do that avoiding the ReactorNotRestartable error
        # pylint: disable=no-member
        from twisted.internet import defer, reactor
        results = defer.gatherResults([
            deferred_run_of(setup_dns_injection),
            deferred_run_of(setup_http_invalid_request_line),
            deferred_run_of(setup_ndt),
            deferred_run_of(setup_tcp_connect),
            deferred_run_of(setup_web_connectivity),
        ])
        results.addCallback(lambda *_: reactor.stop())
        reactor.run()
        # pylint: enable=no-member

if __name__ == "__main__":
    unittest.main()
