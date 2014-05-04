#ifndef FLOJAY_STACK_H
#define FLOJAY_STACK_H


#include <Python.h>


enum fj_stack_type {
    STACK_TYPE_SEQ = 1,
    STACK_TYPE_MAP = 2,
    STACK_TYPE_GEN = 3,
    STACK_TYPE_UNKNOWN = 0
};

struct fj_stack {
  /*
    a simple linked list structure for
    walking the object graph in a depth first fashion
  */
  PyObject * obj;
  /* Only used for list stack elements to track the location in the list */
  /* Or the position in a map iter */
  Py_ssize_t index;
  Py_ssize_t length;
  struct fj_stack * parent;
  enum fj_stack_type type;
};

struct fj_stack * fj_stack_new(void);

struct fj_stack * fj_stack_pop(struct fj_stack *stack);

struct fj_stack * fj_stack_push_seq(struct fj_stack *stack, PyObject *seq);
struct fj_stack * fj_stack_push_map(struct fj_stack *stack, PyObject *map);
struct fj_stack * fj_stack_push_gen(struct fj_stack *stack, PyObject *gen);


#endif
