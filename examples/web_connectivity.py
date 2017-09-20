# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

""" Example showing how to run OONI's WebConnectivity test
    using MeasurementKit and Twisted """

from __future__ import print_function

import measurement_kit
from twisted.internet import reactor

def main():
    """ Main function """

    def print_entry(entry):
        """ Print the entry when it is ready """
        print(entry)

    done = measurement_kit.WebConnectivity()                                   \
        .set_verbosity(measurement_kit.MK_LOG_DEBUG)                            \
        .set_options("nameserver", "8.8.8.8:53")                             \
        .set_options("no_collector", "1")                             \
        .set_input_filepath("fixtures/urls.txt")                              \
        .on_entry(print_entry)                                                 \
        .run_deferred()

    done.addCallback(lambda *_: reactor.stop())
    reactor.run()

if __name__ == "__main__":
    main()
