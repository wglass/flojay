#ifndef FLOJAY_PARSER_H
#define FLOJAY_PARSER_H

#include <Python.h>
#include <api/yajl_parse.h>


#define ENCODING "utf-8"

struct fj_parser {
  PyObject_HEAD
  yajl_handle hand;
  PyObject *callbacks;
};

int fj_parser_init(PyObject *pyself, PyObject *args);

PyObject * fj_parser_parse(PyObject *pyself, PyObject *args);

void fj_parser_dealloc(struct fj_parser *self);

void allocate_method_names(void);

extern PyTypeObject fj_parser_type;


#endif
