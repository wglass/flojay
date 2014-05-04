#ifndef FLOJAY_PARSER_H
#define FLOJAY_PARSER_H

#include <Python.h>
#include <api/yajl_parse.h>


#define ENCODING "utf-8"

typedef struct {
  PyObject_HEAD
  yajl_handle hand;
  PyObject * callbacks;
} flojay_JSONEventParserObject;

int
flojay_JSONEventParser_init(PyObject * pyself, PyObject * args);

PyObject *
flojay_JSONEventParser_parse(PyObject * pyself, PyObject * args);

void
flojay_JSONEventParser_dealloc(flojay_JSONEventParserObject * self);

void
allocate_method_names(void);

extern PyTypeObject flojay_JSONEventParserType;


#endif
