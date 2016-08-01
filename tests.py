# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

from __future__ import print_function
import _measurement_kit as _mk
import measurement_kit
import time
import unittest

class TestIntegrationCxxLibrary(unittest.TestCase):
    """ Integration test that uses directly the C++ library """

    def test_ndt(self):
        """ Runs NDT test """

        handle = _mk.create("ndt")
        _mk.set_options(handle, "save_real_probe_ip", "1")
        _mk.set_verbosity(handle, 1)
        again = [True]

        def on_complete():
            print("test done")
            again[0] = False

        def on_log(severity, line):
            print("mk: <{}> {}".format(severity, line))

        _mk.on_log(handle, on_log)
        _mk.run_async(handle, on_complete)
        _mk.destroy(handle)  # Note: this MUST be possible
        while again[0]:
            time.sleep(1)

class TestIntegrationSync(unittest.TestCase):
    """ Integration test using sync wrappers """

    def test_dns_injection(self):
        """ Runs dns-injection test """
        measurement_kit.DnsInjection()                                         \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .set_options(b"backend", b"8.8.8.1:53")                            \
            .set_input_filepath(b"fixtures/hosts.txt")                         \
            .run()

    def test_http_invalid_request_line(self):
        """ Runs http-invalid-request-line test """
        measurement_kit.HttpInvalidRequestLine()                               \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .set_options(b"backend", b"http://213.138.109.232/")               \
            .run()

    def test_ndt(self):
        """ Runs ndt test """
        measurement_kit.NdtTest()                                              \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .run()

    def test_tcp_connect(self):
        """ Runs tcp-connect test """
        measurement_kit.TcpConnect()                                           \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .set_options(b"port", b"80")                                       \
            .set_input_filepath(b"fixtures/hosts.txt")                         \
            .run()

class TestIntegrationAsync(unittest.TestCase):
    """ Integration test using async wrappers """

    def test_dns_injection(self):
        """ Runs dns-injection test """

        done = [False]
        def complete():
            done[0] = True

        measurement_kit.DnsInjection()                                         \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .set_options(b"backend", b"8.8.8.1:53")                            \
            .set_input_filepath(b"fixtures/hosts.txt")                         \
            .run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_http_invalid_request_line(self):
        """ Runs http-invalid-request-line test """

        done = [False]
        def complete():
            done[0] = True

        measurement_kit.HttpInvalidRequestLine()                               \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .set_options(b"backend", b"http://213.138.109.232/")               \
            .run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_ndt(self):
        """ Runs ndt test """

        done = [False]
        def complete():
            done[0] = True

        measurement_kit.NdtTest()                                              \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .run_async(complete)

        while not done[0]:
            time.sleep(1)

    def test_tcp_connect(self):
        """ Runs tcp-connect test """

        done = [False]
        def complete():
            done[0] = True

        measurement_kit.TcpConnect()                                           \
            .set_verbosity(measurement_kit.MK_LOG_DEBUG)                       \
            .set_options(b"port", b"80")                                       \
            .set_input_filepath(b"fixtures/hosts.txt")                         \
            .run_async(complete)

        while not done[0]:
            time.sleep(1)

if __name__ == "__main__":
    unittest.main()
