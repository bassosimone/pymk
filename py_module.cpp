// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

// Related, interesting read: <https://gist.github.com/liuyu81/3473376>.

#include <Python.h> // Should be first header
#include <measurement_kit/ndt.hpp>
#include <measurement_kit/ooni.hpp>

extern "C" {

using namespace mk;

struct MkCookie {
    Var<NetTest> net_test;
};

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
    {"create", meth_create, METH_VARARGS, ""},
    {"destroy", meth_destroy, METH_VARARGS, ""},
    {"increase_set_verbosity", meth_set_verbosity, METH_VARARGS, ""},
    {"increase_verbosity", meth_increase_verbosity, METH_VARARGS, ""},
    {"set_input_filepath", meth_set_input_filepath, METH_VARARGS, ""},
    {"set_output_filepath", meth_set_output_filepath, METH_VARARGS, ""},
    {"set_options", meth_set_options, METH_VARARGS, ""},
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

MOD_INIT(_mk) {
    PyObject *module = nullptr;

    MOD_DEF(module, "_mk", "MeasurementKit bindings", Methods);
    if (module == nullptr) {
        return MOD_ERROR_VAL;
    }
    PyEval_InitThreads(); // Tell Python we're going to use threads
    return MOD_SUCCESS_VAL(module);
}

} // extern "C"
