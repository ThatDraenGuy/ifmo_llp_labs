//
// Created by draen on 29.09.23.
//

#ifndef LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_JOIN_H
#define LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_JOIN_H

#include "public/database/domain/join.h"

struct join {
  string_t what;
  struct predicate *on;
};

#endif // LLP_LAB_INCLUDE_PRIVATE_DATABASE_DOMAIN_JOIN_H
