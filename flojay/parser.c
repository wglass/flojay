#include <Python.h>
#include <api/yajl_parse.h>

#include "parser.h"


static PyMethodDef fj_parser_methods[] = {
    {
        "__init__",
        (PyCFunction)fj_parser_init, 2,
        "Init!"
    },
    {
        "parse",
        (PyCFunction)fj_parser_parse, 2,
        "Parse a hunk of JSON and invoke appropriate callbacks."
    },
    {NULL}  /* Sentinel */
};

PyTypeObject fj_parser_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "flojay.JSONEventParser",             /*tp_name*/
    sizeof(struct fj_parser), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)fj_parser_dealloc, /* tp_dealloc */
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "flojay JSON event-based parser, based on yajl",  /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    fj_parser_methods,  /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)fj_parser_init,   /* tp_init */
    PyType_GenericAlloc,       /* tp_alloc */
    PyType_GenericNew          /* tp_new */
};

PyObject * handle_null_method;
PyObject * handle_boolean_method;
PyObject * handle_number_method;
PyObject * handle_string_method;
PyObject * handle_start_map_method;
PyObject * handle_map_key_method;
PyObject * handle_end_map_method;
PyObject * handle_start_array_method;
PyObject * handle_end_array_method;

static PyObject * allocate_method(char * name) {
    PyObject * method_string = PyString_FromString(name);
    if( method_string == NULL ) {
        return NULL;
    }
    Py_INCREF(method_string);
    return method_string;
}

void allocate_method_names(void) {
    handle_null_method = allocate_method("handle_null");
    handle_boolean_method = allocate_method("handle_boolean");
    handle_number_method = allocate_method("handle_number");
    handle_string_method = allocate_method("handle_string");
    handle_start_map_method = allocate_method("handle_start_map");
    handle_map_key_method = allocate_method("handle_map_key");
    handle_end_map_method = allocate_method("handle_end_map");
    handle_start_array_method = allocate_method("handle_start_array");
    handle_end_array_method = allocate_method("handle_end_array");
};


static int
fj_handle_null(void * self) {

    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_null_method, NULL);
    return 1;
}

static int
fj_handle_boolean(void * self, int boolean) {
    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_boolean_method, PyBool_FromLong(boolean), NULL);
    return 1;
}

static int
fj_handle_number(void * self, const char * number, size_t len) {
    PyObject * str = PyString_FromStringAndSize(number, len);
    PyObject *python_number = PyNumber_Int(str);

    if( python_number == NULL ) {
        python_number = PyNumber_Long(str);
        PyErr_Clear();
    }
    if( python_number == NULL ) {
        python_number = PyNumber_Float(str);
    }

    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_number_method, python_number, NULL);
    return 1;
}

static int
fj_string_callback(void * self,
                       PyObject * method_to_call,
                       const unsigned char * str, size_t len) {

    PyObject * python_string = PyUnicode_Decode((char *) str, len,
                                                ENCODING, "strict");

    if( python_string == NULL ) {
        return 0;
    };

    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               method_to_call, python_string, NULL);

    return 1;
}

static int
fj_handle_string(void * self, const unsigned char * str, size_t len) {

    return fj_string_callback(self, handle_string_method, str, len);
}

static int
fj_handle_start_map(void * self) {
    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_start_map_method, NULL);
    return 1;
}

static int
fj_handle_map_key(void * self, const unsigned char * str, size_t len) {
    return fj_string_callback(self, handle_map_key_method, str, len);
}

static int
fj_handle_end_map(void * self) {
    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_end_map_method, NULL);
    return 1;
}

static int fj_handle_start_array(void * self) {
    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_start_array_method, NULL);
    return 1;
}

static int fj_handle_end_array(void * self) {
    PyObject_CallMethodObjArgs(
                               ((struct fj_parser *) self)->callbacks,
                               handle_end_array_method, NULL);
    return 1;
}

static yajl_callbacks callbacks = {
    fj_handle_null,
    fj_handle_boolean,
    NULL,
    NULL,
    fj_handle_number,
    fj_handle_string,
    fj_handle_start_map,
    fj_handle_map_key,
    fj_handle_end_map,
    fj_handle_start_array,
    fj_handle_end_array
};

int
fj_parser_init(PyObject * pyself, PyObject * args)
{
    struct fj_parser * self = (struct fj_parser *) pyself;
    if( ! PyArg_ParseTuple(args, "O", &(self->callbacks)) ) {
        return -1;
    }
    Py_INCREF(self->callbacks);

    self->hand = yajl_alloc(&callbacks, NULL, (void *) self);
    yajl_config(self->hand, yajl_allow_partial_values, 1);
    yajl_config(self->hand, yajl_allow_trailing_garbage, 0);

    return 0;
}

PyObject *
fj_parser_parse(PyObject *pyself, PyObject *args)
{
    yajl_status status;
    unsigned char * json_string = NULL;
    int json_string_length;

    if( ! PyArg_ParseTuple(args, "es#", ENCODING, &json_string, &json_string_length) ) {
        return NULL;
    }

    if(!json_string) {
        return Py_None;
    }

    struct fj_parser * self = (struct fj_parser *) pyself;

    status = yajl_parse(self->hand, json_string, json_string_length);

    if ( status != yajl_status_ok ) {
        unsigned char * err = yajl_get_error(self->hand, 0, json_string,
                                             json_string_length);
        PyErr_SetString(PyExc_ValueError, (char *) err);
        yajl_free_error(self->hand, err);
        return NULL;
    }
    PyMem_Free(json_string);
    if ( PyErr_Occurred() ) {
        return NULL;
    }
    return Py_None;
}

void
fj_parser_dealloc(struct fj_parser *self)
{
    yajl_free(self->hand);
    Py_DECREF(self->callbacks);

    self->ob_type->tp_free((PyObject*)self);
}
