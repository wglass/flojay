#include <Python.h>

typedef struct {
  PyObject_HEAD
  PyObject * root;
  PyObject * containers;
  PyObject * map_key;
} flojay_PythonDecoderCallbacks_object;

static inline flojay_PythonDecoderCallbacks_object *
_self(PyObject * pyself)
{
  return (flojay_PythonDecoderCallbacks_object *) pyself;
}

static void
append_value(flojay_PythonDecoderCallbacks_object * self,
             PyObject * value)
{
  PyObject * current_container;
  if(Py_None == self->root) {
    self->root = value;
  } else if(NULL != self->map_key) {
    current_container = PyList_GetItem(self->containers, 0);
    PyDict_SetItem(current_container, self->map_key, value);
    self->map_key = NULL;
  } else {
    current_container = PyList_GetItem(self->containers, 0);
    PyList_Append(current_container, value);
  }
}

static int
flojay_PythonDecoderCallbacks_init(PyObject * pyself)
{
  flojay_PythonDecoderCallbacks_object * self = _self(pyself);
  self->root = Py_None;
  self->map_key = Py_None;
  self->containers = PyList_New(0);
  return 0;
}

static PyObject *
flojay_PythonDecoderCallbacks_root(PyObject * pyself)
{
  return _self(pyself)->root;
}

static PyObject *
flojay_PythonDecoderCallbacks_handle_value(PyObject * pyself, PyObject * args)
{
  PyObject * value;
  if(!PyArg_ParseTuple(args, "O", &value))
    return 0;
  append_value(_self(pyself), value);
  return Py_None;
}

static PyObject *
flojay_PythonDecoderCallbacks_handle_null(PyObject * pyself, PyObject * args)
{
  static PyObject * none_tuple = NULL;
  if(NULL == none_tuple) {
    none_tuple = Py_BuildValue("(O)", Py_None);
    Py_INCREF(none_tuple);
  }
  
  return flojay_PythonDecoderCallbacks_handle_value(pyself, none_tuple);
}

static void
start_container(flojay_PythonDecoderCallbacks_object * self,
                PyObject * container)
{
  append_value(self, container);
  PyList_Insert(self->containers, 0, container);
}

static void
pop_container(flojay_PythonDecoderCallbacks_object * self)
{
  PyList_SetSlice(self->containers, 0, 1, NULL);
}

static PyObject *
flojay_PythonDecoderCallbacks_start_array(PyObject * pyself)
{
  PyObject * container = PyList_New(0);
  start_container(_self(pyself), container);
  return Py_None;
}

static PyObject *
flojay_PythonDecoderCallbacks_end_array(PyObject * pyself)
{
  pop_container(_self(pyself));
  return Py_None;
}

static PyObject *
flojay_PythonDecoderCallbacks_start_map(PyObject * pyself)
{
  PyObject * container = PyDict_New();
  start_container(_self(pyself), container);
  return Py_None;
}

static PyObject *
flojay_PythonDecoderCallbacks_map_key(PyObject * pyself, PyObject * args)
{
  PyObject * map_key;
  if(!PyArg_ParseTuple(args, "O", &map_key))
    return 0;

  _self(pyself)->map_key = map_key;

  return Py_None;
}

static PyObject *
flojay_PythonDecoderCallbacks_end_map(PyObject * pyself)
{
  pop_container(_self(pyself));
  return Py_None;
}

static PyMethodDef flojay_PythonDecoderCallbacks_methods[] = {
  {
    "__init__",
    (PyCFunction)flojay_PythonDecoderCallbacks_init, 1,
    "Init!"
  },
  {
    "root",
    (PyCFunction)flojay_PythonDecoderCallbacks_root, 1,
    "Return the root object of the Python object graph produced by the "
    "parser."
  },
  {
    "handle_number",
    (PyCFunction)flojay_PythonDecoderCallbacks_handle_value, 2,
    NULL
  },
  {
    "handle_string",
    (PyCFunction)flojay_PythonDecoderCallbacks_handle_value, 2,
    NULL
  },
  {
    "handle_boolean",
    (PyCFunction)flojay_PythonDecoderCallbacks_handle_value, 2,
    NULL
  },
  {
    "handle_null",
    (PyCFunction)flojay_PythonDecoderCallbacks_handle_null, 1,
    NULL
  },
  {
    "handle_start_array",
    (PyCFunction)flojay_PythonDecoderCallbacks_start_array, 1,
    NULL
  },
  {
    "handle_end_array",
    (PyCFunction)flojay_PythonDecoderCallbacks_end_array, 1,
    NULL
  },
  {
    "handle_start_map",
    (PyCFunction)flojay_PythonDecoderCallbacks_start_map, 1,
    NULL
  },
  {
    "handle_map_key",
    (PyCFunction)flojay_PythonDecoderCallbacks_map_key, 2,
    NULL
  },
  {
    "handle_end_map",
    (PyCFunction)flojay_PythonDecoderCallbacks_end_map, 1,
    NULL
  },
  {NULL}  /* Sentinel */
};

static PyTypeObject flojay_PythonDecoderCallbacks_type = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "flojay.PythonDecoderCallbacks", /*tp_name*/
    sizeof(flojay_PythonDecoderCallbacks_object), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
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
    "flojay callbacks for decoding JSON to Python",  /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    flojay_PythonDecoderCallbacks_methods,  /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)flojay_PythonDecoderCallbacks_init,   /* tp_init */
    PyType_GenericAlloc,       /* tp_alloc */
    PyType_GenericNew          /* tp_new */
};

void
init_flojay_decoder_callbacks(PyObject * module)
{
    if (PyType_Ready(&flojay_PythonDecoderCallbacks_type) < 0)
        return;

    Py_INCREF(&flojay_PythonDecoderCallbacks_type);

    PyModule_AddObject(module, "PythonDecoderCallbacks", 
                       (PyObject *)&flojay_PythonDecoderCallbacks_type);
}