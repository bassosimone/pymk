# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

""" Integration tests for MeasurementKit Python bindings """

from __future__ import print_function
import time
import unittest

import measurement_kit
from measurement_kit import _bindings as _mk

class TestIntegrationCxxLibrary(unittest.TestCase):
    """ Integration test that uses directly the C++ library """

    def test_ndt(self):
        """ Runs NDT test """

        handle = _mk.create("ndt")
        _mk.set_options(handle, "save_real_probe_ip", "1")
        _mk.set_verbosity(handle, 1)
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

def setup_dns_injection():
    """ Setups the dns-injection test """
    return measurement_kit.DnsInjection()                                      \
        .set_verbosity(measurement_kit.MK_LOG_DEBUG)                           \
        .set_options(b"backend", b"8.8.8.1:53")                                \
        .set_input_filepath(b"fixtures/hosts.txt")

def setup_http_invalid_request_line():
    """ Setups the http-invalid-request-line test """
    return measurement_kit.HttpInvalidRequestLine()                            \
        .set_verbosity(measurement_kit.MK_LOG_DEBUG)                           \
        .set_options(b"backend", b"http://213.138.109.232/")

def setup_ndt():
    """ Setups the network-diagnostic-tool test """
    return measurement_kit.NdtTest()                                           \
        .set_verbosity(measurement_kit.MK_LOG_DEBUG)

def setup_tcp_connect():
    """ Setups the tcp-connect test """
    return measurement_kit.TcpConnect()                                        \
        .set_verbosity(measurement_kit.MK_LOG_DEBUG)                           \
        .set_options(b"port", b"80")                                           \
        .set_input_filepath(b"fixtures/hosts.txt")

def setup_web_connectivity():
    """ Setups the web-connectivity test """
    return measurement_kit.WebConnectivity()                                   \
        .set_verbosity(measurement_kit.MK_LOG_DEBUG)                           \
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

class TestIntegrationAsync(unittest.TestCase):
    """ Integration test using async wrappers """

    def test_dns_injection(self):
        """ Runs dns-injection test """

        done = [False]
        def complete():
            done[0] = True

        setup_dns_injection().run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_http_invalid_request_line(self):
        """ Runs http-invalid-request-line test """

        done = [False]
        def complete():
            done[0] = True

        setup_http_invalid_request_line().run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_ndt(self):
        """ Runs ndt test """

        done = [False]
        def complete():
            done[0] = True

        setup_ndt().run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_tcp_connect(self):
        """ Runs tcp-connect test """

        done = [False]
        def complete():
            done[0] = True

        setup_tcp_connect().run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_web_connectivity(self):
        """ Runs web-connectivity test """

        done = [False]
        def complete():
            done[0] = True

        setup_web_connectivity().run_async(complete)

        while not done[0]:
            time.sleep(1)

class TestIntegrationDeferred(unittest.TestCase):
    """ Integration test using deferred wrappers """

    def test_dns_injection(self):
        """ Runs dns-injection test """
        from twisted.internet import reactor
        d = setup_dns_injection().run_deferred()
        d.addCallback(lambda *args: reactor.callFromThread(reactor.stop))
        reactor.run()

    def test_http_invalid_request_line(self):
        """ Runs http-invalid-request-line test """
        from twisted.internet import reactor
        d = setup_http_invalid_request_line().run_deferred()
        d.addCallback(lambda *args: reactor.callFromThread(reactor.stop))
        reactor.run()

    def test_ndt(self):
        """ Runs ndt test """
        from twisted.internet import reactor
        d = setup_ndt().run_deferred()
        d.addCallback(lambda *args: reactor.callFromThread(reactor.stop))
        reactor.run()

    def test_tcp_connect(self):
        """ Runs tcp-connect test """
        from twisted.internet import reactor
        d = setup_tcp_connect().run_deferred()
        d.addCallback(lambda *args: reactor.callFromThread(reactor.stop))
        reactor.run()

    def test_web_connectivity(self):
        """ Runs web-connectivity test """
        from twisted.internet import reactor
        d = setup_web_connectivity().run_deferred()
        d.addCallback(lambda *args: reactor.callFromThread(reactor.stop))
        reactor.run()

if __name__ == "__main__":
    unittest.main()
