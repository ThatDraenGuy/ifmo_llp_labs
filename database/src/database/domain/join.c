//
// Created by draen on 29.09.23.
//

#include "database/private/database/domain/join.h"
#include <malloc.h>

struct join *join_of(str_t what, struct predicate *on) {
  struct join *self = malloc(sizeof(struct join));
  self->on = on;
  self->what = str_into(what);
  return self;
}

str_t join_get_what(struct join *self) { return string_as_str(self->what); }

struct predicate *join_get_on(struct join *self) { return self->on; }

void join_destroy(struct join *self) {
  string_destroy(self->what);
  predicate_destroy(self->on);
  free(self);
}