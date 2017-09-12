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

def meek_fronted_requests(url):
    """ Runs the meek-fronted-requests test """
    done = tx.meek_fronted_requests(url, {
        "no_collector": "1",
    })
    def print_entry(entry):
        pprint(entry)
    done.addCallback(print_entry)
    return done

def dns_query(url):
    """ Runs the dns_query test """
    done = tx.dns_query(url)
    def print_entry(entry):
        pprint(entry)
    def errback(error):
        print("ERROR")
        print(error)
    done.addCallback(print_entry)
    done.addErrback(errback)
    return done

def initialize():
    """ Schedules initial event """
    #done = defer.gatherResults([
    #    web_connectivity("http://www.google.com/killer-robots.txt"),
    #    web_connectivity("http://nexa.polito.it/robots.txt")
    #])
    #done = defer.gatherResults([
    #    meek_fronted_requests("a0.awsstatic.com:d2zfqthxsdq309.cloudfront.net"),
    #    meek_fronted_requests("a0.awsstatic.com:d2zfqthxsdq309.cloudfront.net")
    #])
    done = defer.gatherResults([
        dns_query("google.com")
    ])
    done.addBoth(lambda *_: reactor.stop())

def main():
    """ Main function """
    tx.increase_verbosity()
    tx.increase_verbosity()
    tx.increase_verbosity()
    tx.increase_verbosity()
    reactor.callWhenRunning(initialize)
    reactor.run()

if __name__ == "__main__":
    main()
