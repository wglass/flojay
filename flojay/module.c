#include <Python.h>

#include "generator.h"
#include "encoder.h"
#include "parser.h"


static PyMethodDef FlojayMethods[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initflojay(void)
{
    if (PyType_Ready(&flojay_JSONEncoderType) < 0) {
        return;
    }
    if (PyType_Ready(&flojay_JSONEventParserType) < 0) {
        return;
    }
    if (PyType_Ready(&flojay_generator_type) < 0) {
        return;
    }

    PyObject *module = Py_InitModule3("flojay", FlojayMethods,
                                      "flojay JSON encoder and parser, using yajl");

    Py_INCREF(&flojay_JSONEncoderType);
    Py_INCREF(&flojay_JSONEventParserType);
    Py_INCREF(&flojay_generator_type);

    PyModule_AddObject(module,
                       "JSONEncoder",
                       (PyObject *)&flojay_JSONEncoderType);
    PyModule_AddObject(module,
                       "JSONEventParser",
                       (PyObject *)&flojay_JSONEventParserType);

    allocate_method_names();
}
