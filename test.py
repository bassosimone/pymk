# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

from __future__ import print_function
import _measurement_kit as _mk
import time

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
_mk.destroy(handle)
while again[0]:
    time.sleep(1)
