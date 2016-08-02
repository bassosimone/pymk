// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

// Related, interesting read: <https://gist.github.com/liuyu81/3473376>.

#include <Python.h> // Should be first header

#include <measurement_kit/ndt.hpp>
#include <measurement_kit/ooni.hpp>

extern "C" {

using namespace mk;

// Holds the Var<NetTest> actually used for running the test. Note that the
// code below SHOULD NOT assume that cookie is alive after the test has been
// started, i.e. no callback should ever refer to it.
struct MkCookie {
    Var<NetTest> net_test;
};

static PyObject *meth_library_version(PyObject *, PyObject *args) {
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    std::string version = library_version();
    if (version != MEASUREMENT_KIT_VERSION) {
        PyErr_SetString(PyExc_RuntimeError, "MK version mismatch");
        return nullptr;
    }
    return Py_BuildValue("s", version.c_str());
}

static PyObject *meth_create(PyObject *, PyObject *args) {
    const char *name = nullptr;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return nullptr;
    }
    MkCookie *cookie = new MkCookie;
    if (strcmp(name, "ndt") == 0) {
        cookie->net_test.reset(new ndt::NdtTest);
    } else if (strcmp(name, "dns_injection") == 0) {
        cookie->net_test.reset(new ooni::DnsInjection);
    } else if (strcmp(name, "http_invalid_request_line") == 0) {
        cookie->net_test.reset(new ooni::HttpInvalidRequestLine);
    } else if (strcmp(name, "tcp_connect") == 0) {
        cookie->net_test.reset(new ooni::TcpConnect);
    } else if (strcmp(name, "web_connectivity") == 0) {
        cookie->net_test.reset(new ooni::WebConnectivity);
    } else {
        /* nothing */ ;
    }
    if (!cookie->net_test) {
        PyErr_SetString(PyExc_RuntimeError, "invalid test name");
        return nullptr;
    }
    return Py_BuildValue("L", cookie);
}

static PyObject *meth_destroy(PyObject *, PyObject *args) {
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "L", &pointer)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    delete cookie;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_set_verbosity(PyObject *, PyObject *args) {
    int verbosity = 0;
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "Li", &pointer, &verbosity)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *) pointer;
    cookie->net_test->set_verbosity(verbosity);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_increase_verbosity(PyObject *, PyObject *args) {
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "L", &pointer)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *) pointer;
    cookie->net_test->increase_verbosity();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_on_log(PyObject *, PyObject *args) {
    PyObject *callback = nullptr;
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "LO:on_log", &pointer, &callback)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;

    // Increment the reference counting of the callback used for logging to
    // keep it safe and only release the reference when the logger dies
    Py_INCREF(callback);
    cookie->net_test->logger->on_eof([callback]() {
        // Note: this is called by the destructor of the logger when the
        // owning NetTest is also about to be destroyed
        Py_DECREF(callback);
    });

    cookie->net_test->on_log([callback](uint32_t severity, const char *line) {
        PyGILState_STATE state = PyGILState_Ensure(); // Acquires the GIL

        PyObject *args = Py_BuildValue("(is)", severity, line);
        if (args != nullptr) {
            PyObject *result = PyObject_CallObject(callback, args);
            if (result != nullptr) {
                Py_DECREF(result);
            } else {
                PyErr_Print();
            }
            Py_DECREF(args);
        } else {
            PyErr_Print();
        }

        PyGILState_Release(state); // Releases the GIL
    });

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_on_entry(PyObject *, PyObject *args) {
    PyObject *callback = nullptr;
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "LO:on_entry", &pointer, &callback)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;

    // Reference the callback to keep it safe and remove the reference when
    // we enter into the `end` state. It should not happen that `on_entry` is
    // called again, but for robustness, better to clear it.
    Py_INCREF(callback);
    Var<NetTest> net_test = cookie->net_test;
    cookie->net_test->on_end([callback, net_test]() {
        net_test->on_entry(nullptr);
        Py_DECREF(callback);
    });

    cookie->net_test->on_entry([callback](std::string entry) {
        PyGILState_STATE state = PyGILState_Ensure(); // Acquires the GIL

        PyObject *args = Py_BuildValue("(s)", entry.c_str());
        if (args != nullptr) {
            PyObject *result = PyObject_CallObject(callback, args);
            if (result != nullptr) {
                Py_DECREF(result);
            } else {
                PyErr_Print();
            }
            Py_DECREF(args);
        } else {
            PyErr_Print();
        }

        PyGILState_Release(state); // Releases the GIL
    });

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_set_input_filepath(PyObject *, PyObject *args) {
    const char *path = nullptr;
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "Ls", &pointer, &path)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    cookie->net_test->set_input_filepath(path);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_set_output_filepath(PyObject *, PyObject *args) {
    const char *path = nullptr;
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "Ls", &pointer, &path)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    cookie->net_test->set_output_filepath(path);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_set_options(PyObject *, PyObject *args) {
    const char *key = nullptr;
    long long pointer = 0LL;
    const char *value = nullptr;
    if (!PyArg_ParseTuple(args, "Lss", &pointer, &key, &value)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    cookie->net_test->set_options(key, value);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_run(PyObject *, PyObject *args) {
    long long pointer = 0LL;
    if (!PyArg_ParseTuple(args, "L", &pointer)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    Py_BEGIN_ALLOW_THREADS // Releases the GIL

    cookie->net_test->run();

    Py_END_ALLOW_THREADS // Acquires the GIL
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_run_async(PyObject *, PyObject *args) {
    long long pointer = 0LL;
    PyObject *callback = nullptr;
    if (!PyArg_ParseTuple(args, "LO:run_async", &pointer, &callback)) {
        return nullptr;
    }
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "callback must be callable");
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    Py_INCREF(callback);
    Py_BEGIN_ALLOW_THREADS // Releases the GIL

    cookie->net_test->run([callback]() {
        PyGILState_STATE state = PyGILState_Ensure(); // Acquires the GIL

        PyObject *result = PyObject_CallObject(callback, nullptr);
        if (result != nullptr) {
            Py_DECREF(result);
        } else {
            PyErr_Print();
        }
        Py_DECREF(callback);

        PyGILState_Release(state); // Releases the GIL
    });

    Py_END_ALLOW_THREADS // Acquires the GIL
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Methods[] = {
    {"library_version", meth_library_version, METH_VARARGS, ""},
    {"create", meth_create, METH_VARARGS, ""},
    {"destroy", meth_destroy, METH_VARARGS, ""},
    {"set_verbosity", meth_set_verbosity, METH_VARARGS, ""},
    {"increase_verbosity", meth_increase_verbosity, METH_VARARGS, ""},
    {"on_log", meth_on_log, METH_VARARGS, ""},
    {"on_entry", meth_on_entry, METH_VARARGS, ""},
    {"set_input_filepath", meth_set_input_filepath, METH_VARARGS, ""},
    {"set_output_filepath", meth_set_output_filepath, METH_VARARGS, ""},
    {"set_options", meth_set_options, METH_VARARGS, ""},
    {"run", meth_run, METH_VARARGS, ""},
    {"run_async", meth_run_async, METH_VARARGS, ""},
    {nullptr, nullptr, 0, nullptr},
};

// http://python3porting.com/cextensions.html
#if PY_MAJOR_VERSION >= 3
  #define MOD_ERROR_VAL NULL
  #define MOD_SUCCESS_VAL(val) val
  #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          static struct PyModuleDef moduledef = { \
            PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
          ob = PyModule_Create(&moduledef);
#else
  #define MOD_ERROR_VAL
  #define MOD_SUCCESS_VAL(val)
  #define MOD_INIT(name) void init##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          ob = Py_InitModule3(name, methods, doc);
#endif

MOD_INIT(_bindings) {
    PyObject *module = nullptr;

    MOD_DEF(module, "_bindings", "MeasurementKit bindings", Methods);
    if (module == nullptr) {
        return MOD_ERROR_VAL;
    }
    PyEval_InitThreads(); // Tell Python we're going to use threads
    return MOD_SUCCESS_VAL(module);
}

} // extern "C"
