#include <Python.h>

#include "generator.h"

#include "encoder.h"


static PyMethodDef flojay_JSONEncoder_methods[] = {
  {
    "__init__",
    (PyCFunction)flojay_JSONEncoder_init, METH_VARARGS | METH_KEYWORDS,
    "Init"
  },
  {
    "iterencode",
    (PyCFunction)flojay_JSONEncoder_iterencode, 1,
    "Yield hunks of JSON now and again"
  },
  {
    "default",
    (PyCFunction)flojay_JSONEncoder_default,  1,
    "Default handler for objects of unknown type"
  },
  {NULL}  /* Sentinel */
};

PyTypeObject flojay_JSONEncoderType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "flojay.JSONEncoder",             /*tp_name*/
    sizeof(flojay_JSONEncoderObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)flojay_JSONEncoder_dealloc,/*tp_dealloc*/
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
    "flojay JSON encoder, based on yajl",  /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    flojay_JSONEncoder_methods,  /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    flojay_JSONEncoder_init,   /* tp_init */
    PyType_GenericAlloc,       /* tp_alloc */
    PyType_GenericNew          /* tp_new */
};

PyObject *
flojay_JSONEncoder_default(PyObject * self, PyObject * args)
{
  PyObject * obj;
  if(!PyArg_ParseTuple(args, "O", &obj))
    return NULL;

  PyObject * repr = PyObject_Repr(obj);
  PyErr_Format(PyExc_TypeError, "%s is not JSON serializable", PyString_AsString(repr));

  return NULL;
}

int
flojay_JSONEncoder_init(PyObject * pyself, PyObject * args, PyObject * kwords)
{
  static char *kwlist[] = {"default", "buffer_size", "beautify", "indent_string", NULL};
  PyObject * default_func = Py_None;
  PyObject * beautify;
  char * indent_string;
  size_t indent_string_size;

  size_t buffer_size = DEFAULT_BUFFER_SIZE;
  flojay_JSONEncoderObject * self = (flojay_JSONEncoderObject *) pyself;

  if(!PyArg_ParseTupleAndKeywords(args, kwords, "|OiOz", kwlist,
                                  &default_func, &buffer_size, &beautify, &indent_string))
    return -1;

  self->bufsize = buffer_size;
  self->beautify = beautify;
  if(indent_string == NULL) {
    self->indent_string = NULL;
  } else {
    indent_string_size = strlen(indent_string);
    self->indent_string = PyMem_New(char, indent_string_size + 1);
    if(!self->indent_string) {
      return -1;
    }
    memcpy(self->indent_string, indent_string, indent_string_size + 1);
  }

  if(default_func && Py_None != default_func) {
    Py_INCREF(default_func);
    self->default_func = default_func;
  } else {
    self->default_func = PyObject_GetAttrString((PyObject *) self, "default");
  }

  return 0;
}

PyObject *
flojay_JSONEncoder_iterencode(PyObject * self, PyObject * args)
{
  flojay_generator * generator;
  generator = (flojay_generator *) flojay_generator_new(&flojay_generator_type, NULL, NULL);
  if(-1 == flojay_generator_init(generator, self, args))
    return NULL;

  return (PyObject *) generator;
}

void
flojay_JSONEncoder_dealloc(flojay_JSONEncoderObject * self)
{
  if(NULL != self->indent_string) {
    PyMem_Free(self->indent_string);
  }
  self->ob_type->tp_free((PyObject*)self);
}
