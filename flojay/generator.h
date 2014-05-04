#ifndef FLOJAY_GENERATOR_H
#define FLOJAY_GENERATOR_H

#include <Python.h>

#include <api/yajl_gen.h>
#include "stack.h"
#include "encoder.h"


typedef struct {
  PyObject_HEAD
  struct dfs_stack * head;
  yajl_gen gen;
  flojay_JSONEncoderObject * encoder;
} flojay_generator;

PyObject *
flojay_generator_new(PyTypeObject * type, PyObject * args, PyObject * kwargs);

int
flojay_generator_init(flojay_generator * self, PyObject * encoder, PyObject * args);

void
flojay_generator_dealloc(flojay_generator * self);

PyObject *
flojay_generator_next(flojay_generator * self);

extern PyTypeObject flojay_generator_type;


#endif
