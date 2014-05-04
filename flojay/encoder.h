#ifndef FLOJAY_ENCODER_H
#define FLOJAY_ENCODER_H

#include <Python.h>

#define DEFAULT_BUFFER_SIZE 4096


typedef struct {
  PyObject_HEAD
  PyObject * default_func;
  int ensure_ascii;
  size_t bufsize;
  PyObject * beautify;
  char * indent_string;
} flojay_JSONEncoderObject;

PyObject *
flojay_JSONEncoder_default(PyObject *self, PyObject *args);

int
flojay_JSONEncoder_init(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *
flojay_JSONEncoder_iterencode(PyObject *self, PyObject *args);

void
flojay_JSONEncoder_dealloc(flojay_JSONEncoderObject *self);

extern PyTypeObject flojay_JSONEncoderType;


#endif
