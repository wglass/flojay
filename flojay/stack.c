#include <Python.h>

#include "stack.h"


struct fj_stack *
fj_stack_new(void) {
    struct fj_stack *new = PyMem_New(struct fj_stack, 1);

    new->parent = NULL;
    new->obj = NULL;
    new->index = 0;
    new->length = 0;
    new->type = STACK_TYPE_UNKNOWN;

    return new;
}

struct fj_stack *
fj_stack_pop(struct fj_stack *stack) {
    struct fj_stack *parent = stack->parent;

    Py_XDECREF(stack->obj);
    PyMem_Free((void *) stack);

    return parent;
}

static struct fj_stack *
fj_stack_push(struct fj_stack *stack, PyObject *obj) {
    struct fj_stack *new_node = fj_stack_new();

    Py_INCREF(obj);

    new_node->parent = stack;
    new_node->obj = obj;

    return new_node;
}

struct fj_stack *
fj_stack_push_seq(struct fj_stack *stack, PyObject *seq) {
    struct fj_stack *new_node = fj_stack_push(stack, seq);

    new_node->type = STACK_TYPE_SEQ;
    new_node->length = PySequence_Size(seq);

    return new_node;
}

struct fj_stack *
fj_stack_push_map(struct fj_stack *stack, PyObject *map) {
    struct fj_stack *new_node = fj_stack_push(stack, map);

    new_node->type = STACK_TYPE_MAP;

    return new_node;
}

struct fj_stack *
fj_stack_push_gen(struct fj_stack *stack, PyObject *gen) {
    struct fj_stack *new_node = fj_stack_push(stack, gen);

    new_node->type = STACK_TYPE_GEN;

    return new_node;
}
