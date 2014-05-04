#include <Python.h>

#include "generator.h"

#include "encoder.h"


static PyMethodDef fj_encoder_methods[] = {
  {
    "__init__",
    (PyCFunction)fj_encoder_init, METH_VARARGS | METH_KEYWORDS,
    "Init"
  },
  {
    "iterencode",
    (PyCFunction)fj_encoder_iterencode, 1,
    "Yield hunks of JSON now and again"
  },
  {
    "default",
    (PyCFunction)fj_encoder_default,  1,
    "Default handler for objects of unknown type"
  },
  {NULL}  /* Sentinel */
};

PyTypeObject fj_encoder_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "flojay.JSONEncoder",             /*tp_name*/
    sizeof(struct fj_encoder), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)fj_encoder_dealloc,/*tp_dealloc*/
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
    fj_encoder_methods,  /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    fj_encoder_init,   /* tp_init */
    PyType_GenericAlloc,       /* tp_alloc */
    PyType_GenericNew          /* tp_new */
};

PyObject *
fj_encoder_default(PyObject * self, PyObject * args)
{
  PyObject * obj;
  if(!PyArg_ParseTuple(args, "O", &obj))
    return NULL;

  PyObject * repr = PyObject_Repr(obj);
  PyErr_Format(PyExc_TypeError, "%s is not JSON serializable", PyString_AsString(repr));

  return NULL;
}

int
fj_encoder_init(PyObject * pyself, PyObject * args, PyObject * kwords)
{
  static char *kwlist[] = {"default", "buffer_size", "beautify", "indent_string", NULL};
  PyObject * default_func = Py_None;
  PyObject * beautify;
  char * indent_string;
  size_t indent_string_size;

  size_t buffer_size = DEFAULT_BUFFER_SIZE;
  struct fj_encoder * self = (struct fj_encoder *) pyself;

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
fj_encoder_iterencode(PyObject * self, PyObject * args)
{
  struct fj_generator * generator;
  generator = (struct fj_generator *) fj_generator_new(&fj_generator_type,
                                                       NULL, NULL);

  if(-1 == fj_generator_init(generator, self, args)) {
    return NULL;
  }

  return (PyObject *) generator;
}

void
fj_encoder_dealloc(struct fj_encoder * self)
{
  if(NULL != self->indent_string) {
    PyMem_Free(self->indent_string);
  }
  self->ob_type->tp_free((PyObject*)self);
}
