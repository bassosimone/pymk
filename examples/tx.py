# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

""" Running WebConnectivity using the tx.py submodule """

from __future__ import print_function

from pprint import pprint

from measurement_kit import tx
from twisted.internet import defer, reactor

def web_connectivity(url):
    """ Runs the web-connectivity test """
    done = tx.web_connectivity(url, {
        "nameserver": "8.8.4.4:53",
    })
    def print_entry(entry):
        pprint(entry)
    done.addCallback(print_entry)
    return done

def initialize():
    """ Schedules initial event """
    done = defer.gatherResults([
        web_connectivity("http://www.google.com/killer-robots.txt"),
        web_connectivity("http://nexa.polito.it/robots.txt")
    ])
    done.addBoth(lambda *_: reactor.stop())

def main():
    """ Main function """
    reactor.callWhenRunning(initialize)
    reactor.run()

if __name__ == "__main__":
    main()
