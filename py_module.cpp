// Part of measurement-kit <https://measurement-kit.github.io/>.
// Measurement-kit is free software. See AUTHORS and LICENSE for more
// information on the copying conditions.

// Related, interesting read: <https://gist.github.com/liuyu81/3473376>.

#include <Python.h> // Should be first header
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

extern "C" {

struct MkState {
    PyObject *callback = nullptr;
    std::string key;
    std::thread thread;
    std::string value;
};

#define COOKIE 0xdeadbeefabad1deaUL

struct MkCookie {
    unsigned long long cookie = COOKIE;
    std::shared_ptr<MkState> s{new MkState};
};

static PyObject *meth_create(PyObject *, PyObject *args) {
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    MkCookie *cookie = nullptr;
    try {
        cookie = new MkCookie;
    } catch (const std::bad_alloc &) {
        PyErr_SetString(PyExc_MemoryError, "cannot allocate MkCookie");
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
    if (cookie->cookie != COOKIE) {
        PyErr_SetString(PyExc_RuntimeError, "invalid object cookie");
        return nullptr;
    }
    cookie->cookie = 0UL; // Weak prevention of double free
    delete cookie;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *meth_setopt(PyObject *, PyObject *args) {
    const char *key = nullptr;
    long long pointer = 0LL;
    const char *value = nullptr;
    if (!PyArg_ParseTuple(args, "Lss", &pointer, &key, &value)) {
        return nullptr;
    }
    MkCookie *cookie = (MkCookie *)pointer;
    if (cookie->cookie != COOKIE) {
        PyErr_SetString(PyExc_RuntimeError, "invalid object cookie");
        return nullptr;
    }
    cookie->s->key = key;
    cookie->s->value = value;
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
    if (cookie->cookie != COOKIE) {
        PyErr_SetString(PyExc_RuntimeError, "invalid object cookie");
        return nullptr;
    }
    auto st = cookie->s;
    if (st->callback != nullptr) {
        PyErr_SetString(PyExc_RuntimeError, "already running");
        return nullptr;
    }
    st->callback = callback;
    Py_INCREF(callback);
    Py_BEGIN_ALLOW_THREADS // Releases the GIL

    st->thread = std::thread([st]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        PyGILState_STATE state = PyGILState_Ensure(); // Acquires the GIL

        PyObject *args = Py_BuildValue("(i)", 0);
        if (args != nullptr) {
            PyObject *result = PyObject_CallObject(st->callback, args);
            if (result != nullptr) {
                Py_DECREF(result);
            } else {
                PyErr_Print();
            }
            Py_DECREF(args);
        } else {
            PyErr_Print();
        }

        Py_DECREF(st->callback);
        st->callback = nullptr;

        PyGILState_Release(state); // Releases the GIL
    });
    st->thread.detach();

    Py_END_ALLOW_THREADS // Acquires the GIL
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef Methods[] = {
    {"create", meth_create, METH_VARARGS, ""},
    {"destroy", meth_destroy, METH_VARARGS, ""},
    {"setopt", meth_setopt, METH_VARARGS, ""},
    {"run_async", meth_run_async, METH_VARARGS, ""},
    {nullptr, nullptr, 0, nullptr},
};

PyMODINIT_FUNC init_mk() {
    PyObject *module = Py_InitModule("_mk", Methods);
    if (module == nullptr) {
        return;
    }
    PyEval_InitThreads(); // Tell Python we're going to use threads
}

}
