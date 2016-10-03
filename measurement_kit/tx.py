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
