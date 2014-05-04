#ifndef FLOJAY_STACK_H
#define FLOJAY_STACK_H


#include <Python.h>


enum dfs_stack_type {
    STACK_TYPE_SEQ = 1,
    STACK_TYPE_MAP = 2,
    STACK_TYPE_GEN = 3,
    STACK_TYPE_UNKNOWN = 0
};

struct dfs_stack {
  /*
    a simple linked list structure for
    walking the object graph in a depth first fashion
  */
  PyObject * elt;
  /* Only used for list stack elements to track the location in the list */
  /* Or the position in a map iter */
  Py_ssize_t index;
  Py_ssize_t length;
  struct dfs_stack * parent;
  enum dfs_stack_type type;
};

struct dfs_stack * dfs_stack_new(void);

struct dfs_stack * dfs_stack_pop(struct dfs_stack *stack);

struct dfs_stack * dfs_stack_push_seq(struct dfs_stack *stack, PyObject *seq);
struct dfs_stack * dfs_stack_push_map(struct dfs_stack *stack, PyObject *map);
struct dfs_stack * dfs_stack_push_gen(struct dfs_stack *stack, PyObject *gen);


#endif
