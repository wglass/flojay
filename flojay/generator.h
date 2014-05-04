#ifndef FLOJAY_GENERATOR_H
#define FLOJAY_GENERATOR_H

#include <Python.h>

#include <api/yajl_gen.h>
#include "stack.h"
#include "encoder.h"


struct fj_generator {
  PyObject_HEAD

  yajl_gen gen;

  struct fj_stack *stack;
  struct fj_encoder *encoder;
};

PyObject *fj_generator_new(PyTypeObject *type,
                           PyObject *args, PyObject *kwargs);

int fj_generator_init(struct fj_generator *self,
                      PyObject *encoder, PyObject *args);

void fj_generator_dealloc(struct fj_generator *self);

PyObject * fj_generator_next(struct fj_generator *self);

extern PyTypeObject fj_generator_type;


#endif
