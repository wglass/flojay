#include <Python.h>

#include <api/yajl_gen.h>

#include "error.h"


int _flojay_handle_yajl_error(yajl_gen_status status)
{
  if(status == yajl_gen_status_ok)
    return 0;

  switch(status) {
  case yajl_gen_keys_must_be_strings:
    PyErr_SetString(
                    PyExc_RuntimeError,
                    "At a point where a map key is generated, a function other "
                    "than yajl_gen_string was called");
    break;
  case yajl_max_depth_exceeded:
    PyErr_SetString(
                    PyExc_RuntimeError,
                    "YAJL's maximum generation depth was exceeded. see "
                    "YAJL_MAX_DEPTH");
    break;
  case yajl_gen_in_error_state:
    PyErr_SetString(
                    PyExc_RuntimeError,
                    "A generator function (yajl_gen_XXX) was called while in "
                    "an error state");
    break;
  case yajl_gen_generation_complete:
    PyErr_SetString(
                    PyExc_RuntimeError,
                    "A complete JSON document has been generated");
    break;
  case yajl_gen_invalid_number:
    PyErr_SetString(
                    PyExc_RuntimeError,
                    "yajl_gen_double was passed an invalid floating point "
                    "value (infinity or NaN)");
    break;
  case yajl_gen_no_buf:
    PyErr_SetString(
                    PyExc_RuntimeError,
                    "A print callback was passed in, so there is no internal "
                    "buffer to get from");
    break;
  case yajl_gen_invalid_string:
    PyErr_SetString(PyExc_RuntimeError,
                    "returned from yajl_gen_string() when the "
                    "yajl_gen_validate_utf8 option is enabled and an invalid "
                    "was passed by client code");
    break;
 default:
   PyErr_Format(PyExc_RuntimeError, "YAJL unknown error: %d", status);
  }
  return -1;
}
