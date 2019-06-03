
#include <Python.h>
#include "egihash.h"
#include <string>
#include <vector>

extern "C" {
// Workaround for issues in setuptools, -x c++, clang and -std=c++11 for older gcc
#include "keccak-tiny.c"
}

#if PY_MAJOR_VERSION >= 3
#   define QUAN_BYTES "y#"
#else
#   define QUAN_BYTES "s#"
#endif

static PyObject *
nrghash(PyObject *self, PyObject *args)
{
    uint64_t height;
    const char *data;
    int data_size;
    uint64_t nonce;

    if (!PyArg_ParseTuple(args, "K" QUAN_BYTES "K", &height, &data, &data_size, &nonce)) {
        return NULL;
    }
    
    egihash::h256_t hash_base(data, data_size);
    auto ret = egihash::light::hash(height, hash_base, nonce);
    
    // Calc block hash
    std::vector<uint8_t> buf(data, data+data_size);
    auto vnonce = reinterpret_cast<uint8_t*>(&nonce);
    buf.insert(buf.end(), vnonce, vnonce+sizeof(nonce));
    
    auto hashmix = ret.mixhash.to_hex();
    buf.insert(buf.end(), hashmix.begin(), hashmix.end());
    buf.push_back(0);
    
    egihash::h256_t block_hash(&buf.front(), buf.size());
    
    return Py_BuildValue(
        "(" QUAN_BYTES "," QUAN_BYTES "," QUAN_BYTES ")",
        ret.mixhash.b, sizeof(ret.mixhash.b),
        ret.value.b, sizeof(ret.value.b),
        block_hash.b, sizeof(block_hash.b));
}


static PyMethodDef nrghashMethods[] = {
    {"nrghash",  nrghash, METH_VARARGS, "Calculate QUAN hash"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef nrghashmodule = {
   PyModuleDef_HEAD_INIT,
   "nrghash",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   nrghashMethods
};

PyMODINIT_FUNC
PyInit_nrghash(void)
{
    return PyModule_Create(&nrghashmodule);
}

#else

PyMODINIT_FUNC
initnrghash(void)
{
    Py_InitModule("nrghash", nrghashMethods);
}
#endif
