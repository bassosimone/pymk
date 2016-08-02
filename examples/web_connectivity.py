# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

from __future__ import print_function
import measurement_kit
from twisted.internet import reactor

def main():
    """ Main function """

    def print_entry(entry):
        """ Print the entry when it is ready """
        print(entry)

    d = measurement_kit.WebConnectivity()                                      \
        .set_verbosity(measurement_kit.MK_LOG_INFO)                            \
        .set_options(b"nameserver", b"8.8.8.8:53")                             \
        .set_input_filepath(b"fixtures/urls.txt")                              \
        .on_entry(print_entry)                                                 \
        .run_deferred()

    d.addCallback(lambda *args: reactor.callFromThread(reactor.stop))
    reactor.run()

if __name__ == "__main__":
    main()
