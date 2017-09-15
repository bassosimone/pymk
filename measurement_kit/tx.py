# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

""" MeasurementKit twisted extension """

from __future__ import print_function

import json

from twisted.internet import defer, reactor

from . import pybind

def set_verbosity(level):
    """ Set verbosity to the specified level """
    pybind.set_verbosity(level)

def increase_verbosity():
    """ Increase current verbosity level """
    pybind.increase_verbosity()

def web_connectivity(input_, settings):
    """ Run OONI WebConnectivity test """
    done = defer.Deferred()
    def callback(entry):
        reactor.callInThread(
            lambda: reactor.callFromThread(done.callback, entry)
        )
    pybind.web_connectivity(input_, settings, callback)
    def do_load(entry):
        return json.loads(entry)
    done.addCallback(do_load)
    return done

def meek_fronted_requests(input_, settings):
    """ Run OONI MeekFrontedRequests test """
    done = defer.Deferred()
    def callback(entry):
        reactor.callInThread(
            lambda: reactor.callFromThread(done.callback, entry)
        )
    pybind.meek_fronted_requests(input_, settings, callback)
    def do_load(entry):
        return json.loads(entry)
    done.addCallback(do_load)
    return done

def dns_query(input_):
    """ Run OONI DNSQuery template """
    done = defer.Deferred()
    def callback(entry):
        reactor.callInThread(
            lambda: reactor.callFromThread(done.callback, entry)
        )
    pybind.dns_query(input_, callback)
    def do_load(entry):
        return json.loads(entry)
    done.addCallback(do_load)
    return done

def http_request(input_):
    """ Run OONI HTTPRequest template """
    done = defer.Deferred()
    def callback(entry):
        reactor.callInThread(
            lambda: reactor.callFromThread(done.callback, entry)
        )
    pybind.http_request(input_, callback)
    def do_load(entry):
        return json.loads(entry)
    done.addCallback(do_load)
    return done
