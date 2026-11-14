#include "preface.h"

#ifdef RGEO_GEOS_SUPPORTED

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "globals.h"

RGEO_BEGIN_C

VALUE rb_eRGeoError;
VALUE rb_eRGeoInvalidGeometry;
VALUE rb_eRGeoParseError;
VALUE rb_eRGeoUnsupportedOperation;
VALUE rb_eGeosError;

/* No Ruby API calls are safe while GEOS is unwinding a C++ exception. */
static _Thread_local char* pending_message;
static _Thread_local int message_failure;

void
rgeo_geos_error_handler(const char* format, ...)
{
  va_list args;
  va_list copy;
  int length;

  /* Preserve the first error if GEOS reports another during cleanup. */
  if (pending_message || message_failure)
    return;

  va_start(args, format);
  va_copy(copy, args);
  length = vsnprintf(NULL, 0, format, copy);
  va_end(copy);
  if (length < 0) {
    message_failure = 1;
  } else {
    pending_message = malloc((size_t)length + 1);
    if (pending_message) {
      vsnprintf(pending_message, (size_t)length + 1, format, args);
    } else {
      message_failure = 2;
    }
  }
  va_end(args);
}

static VALUE
exception_from_message(VALUE data)
{
  char* message = (char*)data;
  char* separator = strchr(message, ':');
  char* detail;
  VALUE klass;

  if (!separator)
    return rb_exc_new_cstr(rb_eGeosError, message);

  *separator = '\0';
  detail = separator + 1;
  while (isspace((unsigned char)*detail))
    detail++;

  if (strcmp(message, "UnsupportedOperationException") == 0) {
    klass = rb_eRGeoUnsupportedOperation;
  } else if (strcmp(message, "IllegalArgumentException") == 0) {
    klass = rb_eRGeoInvalidGeometry;
  } else if (strcmp(message, "ParseException") == 0) {
    klass = rb_eRGeoParseError;
  } else {
    *separator = ':';
    return rb_exc_new_cstr(rb_eGeosError, message);
  }
  return rb_exc_new_cstr(klass, detail);
}

void
rgeo_raise_geos_error(void)
{
  char* message = pending_message;
  int failure = message_failure;
  int state = 0;
  VALUE exception;

  /* Clear before Ruby can allocate, run GC, or reenter the extension. */
  pending_message = NULL;
  message_failure = 0;
  if (failure == 2)
    rb_memerror();
  if (failure)
    rb_raise(rb_eGeosError, "Unable to format GEOS error");
  if (!message)
    return;

  /* Ruby allocation can raise too; release the native message either way. */
  exception = rb_protect(exception_from_message, (VALUE)message, &state);
  free(message);
  if (state)
    rb_jump_tag(state);
  rb_exc_raise(exception);
}

void
rgeo_init_geos_errors()
{
  VALUE error_module;

  error_module = rb_define_module_under(rgeo_module, "Error");
  rb_eRGeoError =
    rb_define_class_under(error_module, "RGeoError", rb_eRuntimeError);
  rb_eRGeoInvalidGeometry =
    rb_define_class_under(error_module, "InvalidGeometry", rb_eRGeoError);
  rb_eRGeoUnsupportedOperation =
    rb_define_class_under(error_module, "UnsupportedOperation", rb_eRGeoError);
  rb_eRGeoParseError =
    rb_define_class_under(error_module, "ParseError", rb_eRGeoError);
  rb_eGeosError =
    rb_define_class_under(error_module, "GeosError", rb_eRGeoError);
}

RGEO_END_C
#endif
