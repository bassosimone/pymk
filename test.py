# Part of measurement-kit <https://measurement-kit.github.io/>.
# Measurement-kit is free software. See AUTHORS and LICENSE for more
# information on the copying conditions.

from __future__ import print_function
import _mk
import time

handle = _mk.create()
_mk.setopt(handle, "foo", "bar")
again = [True]

def on_complete(err):
    print("result:", err)
    again[0] = False

_mk.run_async(handle, on_complete)
_mk.destroy(handle)
while again[0]:
    time.sleep(1)
