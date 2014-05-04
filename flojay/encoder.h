#ifndef FLOJAY_ENCODER_H
#define FLOJAY_ENCODER_H

#include <Python.h>

#define DEFAULT_BUFFER_SIZE 4096


struct fj_encoder {
  PyObject_HEAD
  PyObject * default_func;
  int ensure_ascii;
  size_t bufsize;
  PyObject * beautify;
  char * indent_string;
};

PyObject *
fj_encoder_default(PyObject *self, PyObject *args);

int
fj_encoder_init(PyObject *self, PyObject *args, PyObject *kwargs);

PyObject *
fj_encoder_iterencode(PyObject *self, PyObject *args);

void
fj_encoder_dealloc(struct fj_encoder *self);

extern PyTypeObject fj_encoder_type;


#endif
