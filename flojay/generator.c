#include <Python.h>

#include "stack.h"
#include "error.h"

#include "generator.h"


PyTypeObject flojay_generator_type = {
    PyObject_HEAD_INIT(NULL)
    0,                              /* ob_size */
    "flojay_generator",             /* tp_name */
    sizeof(flojay_generator),       /* tp_basicsize */
    0,                              /* tp_itemsize */
    (destructor)flojay_generator_dealloc, /* tp_dealloc */
    0,                              /* tp_print */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_reserved */
    0,                              /* tp_repr */
    0,                              /* tp_as_number */
    0,                              /* tp_as_sequence */
    0,                              /* tp_as_mapping */
    0,                              /* tp_hash */
    0,                              /* tp_call */
    0,                              /* tp_str */
    0,                              /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,             /* tp_flags */
    0,                              /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    (iternextfunc)flojay_generator_next, /* tp_iternext */
    0,                              /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    0,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    flojay_generator_new            /* tp_new */
};

PyObject *
flojay_generator_new(PyTypeObject * type, PyObject * args, PyObject * kwargs)
{
  flojay_generator *self = (flojay_generator *)type->tp_alloc(type, 0);

  self->head = NULL;

  return (PyObject *) self;
}

static
int flojay_encode(flojay_generator * self, PyObject * obj)
{
  char * buf;
  Py_ssize_t len;
  yajl_gen_status status = yajl_gen_status_ok;

  if(obj == Py_None) {
    status = yajl_gen_null(self->gen);
  } else if(obj == Py_True) {
    status = yajl_gen_bool(self->gen, 1);
  } else if(obj == Py_False) {
    status = yajl_gen_bool(self->gen, 0);
  } else if(PyUnicode_Check(obj)) {
    PyObject * str = PyUnicode_AsUTF8String(obj);
    PyString_AsStringAndSize(str, &buf, &len);
    status = yajl_gen_string(self->gen, (const unsigned char *) buf, (size_t) len);
    Py_XDECREF(str);
  } else if(PyString_Check(obj)) {
    PyString_AsStringAndSize(obj, &buf, &len);
    status = yajl_gen_string(self->gen, (const unsigned char *) buf, (size_t) len);
  } else if(PyInt_Check(obj)) {
    status = yajl_gen_integer(self->gen, PyInt_AS_LONG(obj));
  } else if(PyNumber_Check(obj)) {
    PyObject * str = PyObject_Str(obj);
    PyString_AsStringAndSize(str, &buf, &len);
    status = yajl_gen_number(self->gen, (const char *) buf, (size_t) len);
    Py_XDECREF(str);
  } else if(PySequence_Check(obj)) {
    status = yajl_gen_array_open(self->gen);
    self->head = dfs_stack_push_seq(self->head, obj);
  } else if(PyIter_Check(obj)) {
    status = yajl_gen_array_open(self->gen);
    self->head = dfs_stack_push_gen(self->head, obj);
  } else if(PyMapping_Check(obj)) {
    status = yajl_gen_map_open(self->gen);
    self->head = dfs_stack_push_map(self->head, obj);
  } else {
    PyObject * newobj =
      PyObject_CallFunctionObjArgs(self->encoder->default_func, obj, NULL);

    if(NULL == (void *) newobj) {
      return -1;
    }
    if (Py_EnterRecursiveCall(" while encoding a JSON object")) {
      Py_DECREF(newobj);
      return -1;
    }
    int return_value = flojay_encode(self, newobj);
    Py_LeaveRecursiveCall();
    Py_DECREF(newobj);
    return return_value;
  }

  return _flojay_handle_yajl_error(status);
}

static
PyObject * _flojay_next_element(flojay_generator * self)
{
  PyObject *key, *value;

  switch(self->head->type) {
  case STACK_TYPE_SEQ:
    if(self->head->index < self->head->length) {
      PyObject *retval = PySequence_GetItem(self->head->elt, self->head->index);
      self->head->index += 1;
      return retval;
    } else {
      return NULL;
    }
  case STACK_TYPE_MAP:
    if(PyDict_Next(self->head->elt, &self->head->index, &key, &value)) {
      flojay_encode(self, key);
      Py_INCREF(value);
      return value;
    } else {
      return NULL;
    }
  case STACK_TYPE_GEN:
    return PyIter_Next(self->head->elt);
  default:
    return Py_None;
  }
}

void
flojay_generator_dealloc(flojay_generator * self)
{
    while(self->head != NULL) {
      self->head = dfs_stack_pop(self->head);
    }

    yajl_gen_free(self->gen);
    Py_DECREF(self->encoder);

    self->ob_type->tp_free((PyObject*) self);
}

int
flojay_generator_init(flojay_generator * self, PyObject * encoder, PyObject * args)
{
  PyObject * obj;
  self->encoder = (flojay_JSONEncoderObject *) encoder;
  self->gen = yajl_gen_alloc(NULL);
  yajl_gen_config(self->gen, yajl_gen_beautify, (self->encoder->beautify == Py_True) ? 1 : 0);
  if(self->encoder->indent_string)
    yajl_gen_config(self->gen, yajl_gen_indent_string, self->encoder->indent_string);
  yajl_gen_config(self->gen, yajl_gen_validate_utf8, 1);

  if(!PyArg_ParseTuple(args, "O", &obj))
    return -1;

  yajl_gen_clear(self->gen);

  Py_INCREF(encoder);

  if(-1 == flojay_encode(self, obj)) {
    return -1;
  }

  return 0;
}

PyObject *
flojay_generator_next(flojay_generator * self)
{
  PyObject * obj;
  char * buf;
  size_t len;
  yajl_gen_status status;

  while(1) {
      if((NULL == self->head) || (self->head->type == STACK_TYPE_UNKNOWN)) {
          status = yajl_gen_get_buf(self->gen,
                                    (const unsigned char **) &buf,
                                    &len);
          if(-1 == _flojay_handle_yajl_error(status)) {
              return NULL;
          }
          if(len == 0) {
              return NULL;
          }
          PyObject * str = PyString_FromStringAndSize(buf, (Py_ssize_t) len);

          yajl_gen_clear(self->gen);

          return str;
      }

    obj = _flojay_next_element(self);
    if(NULL == obj) {
      if((self->head->type == STACK_TYPE_SEQ)
         || (self->head->type == STACK_TYPE_GEN)) {
        yajl_gen_array_close(self->gen);
      } else if(self->head->type == STACK_TYPE_MAP) {
        yajl_gen_map_close(self->gen);
      }
      self->head = dfs_stack_pop(self->head);
    } else {
      if(-1 == flojay_encode(self, obj)) {
        Py_DECREF(obj);
        return NULL;
      }
      Py_DECREF(obj);
    }

    status = yajl_gen_get_buf(self->gen, (const unsigned char **) &buf, &len);
    if(-1 == _flojay_handle_yajl_error(status))
      return NULL;
    if(len >= self->encoder->bufsize) {
      PyObject * str = PyString_FromStringAndSize(buf, (Py_ssize_t) len);
      yajl_gen_clear(self->gen);
      return str;
    }
  }
}
