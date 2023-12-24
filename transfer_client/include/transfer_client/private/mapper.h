//
// Created by draen on 17.12.23.
//

#ifndef LLP_LAB_TRANSFER_CLIENT_INCLUDE_TRANSFER_CLIENT_PRIVATE_MAPPER_H
#define LLP_LAB_TRANSFER_CLIENT_INCLUDE_TRANSFER_CLIENT_PRIVATE_MAPPER_H

#include "database/public/database/domain/record/record_group.h"
#include "parser/public/ast_node_interface.h"
#include "transfer/public/service_types.h"

result_t map_stmt(struct i_ast_node *node, Statement **result);
struct record_group *map_into_record_group(StatementResult *result);

#endif // LLP_LAB_TRANSFER_CLIENT_INCLUDE_TRANSFER_CLIENT_PRIVATE_MAPPER_H
