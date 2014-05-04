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
    if (PyType_Ready(&fj_encoder_type) < 0) {
        return;
    }
    if (PyType_Ready(&fj_parser_type) < 0) {
        return;
    }
    if (PyType_Ready(&fj_generator_type) < 0) {
        return;
    }

    PyObject *module = Py_InitModule3("flojay", FlojayMethods,
                                      "flojay JSON encoder and parser, using yajl");

    Py_INCREF(&fj_encoder_type);
    Py_INCREF(&fj_parser_type);
    Py_INCREF(&fj_generator_type);

    PyModule_AddObject(module,
                       "JSONEncoder",
                       (PyObject *)&fj_encoder_type);
    PyModule_AddObject(module,
                       "JSONEventParser",
                       (PyObject *)&fj_parser_type);

    allocate_method_names();
}
