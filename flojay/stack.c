#include <Python.h>

#include "stack.h"


struct dfs_stack *
dfs_stack_new(void) {
    struct dfs_stack *new = PyMem_New(struct dfs_stack, 1);

    new->parent = NULL;
    new->elt = NULL;
    new->index = 0;
    new->length = 0;
    new->type = STACK_TYPE_UNKNOWN;

    return new;
}

struct dfs_stack *
dfs_stack_pop(struct dfs_stack *stack) {
    struct dfs_stack *parent = stack->parent;

    Py_XDECREF(stack->elt);
    PyMem_Free((void *) stack);

    return parent;
}

static struct dfs_stack *
dfs_stack_push(struct dfs_stack *stack, PyObject *new_elt) {
    struct dfs_stack *new_node = dfs_stack_new();

    Py_INCREF(new_elt);

    new_node->parent = stack;
    new_node->elt = new_elt;

    return new_node;
}

struct dfs_stack *
dfs_stack_push_seq(struct dfs_stack *stack, PyObject *seq) {
    struct dfs_stack *new_node = dfs_stack_push(stack, seq);

    new_node->type = STACK_TYPE_SEQ;
    new_node->length = PySequence_Size(seq);

    return new_node;
}

struct dfs_stack *
dfs_stack_push_map(struct dfs_stack *stack, PyObject *map) {
    struct dfs_stack *new_node = dfs_stack_push(stack, map);

    new_node->type = STACK_TYPE_MAP;

    return new_node;
}

struct dfs_stack *
dfs_stack_push_gen(struct dfs_stack *stack, PyObject *gen) {
    struct dfs_stack *new_node = dfs_stack_push(stack, gen);

    new_node->type = STACK_TYPE_GEN;

    return new_node;
}
