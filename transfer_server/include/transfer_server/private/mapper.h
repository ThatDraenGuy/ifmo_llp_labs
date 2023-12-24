//
// Created by draen on 23.12.23.
//

#ifndef LLP_LAB_TRANSFER_SERVER_INCLUDE_TRANSFER_SERVER_PRIVATE_MAPPER_H
#define LLP_LAB_TRANSFER_SERVER_INCLUDE_TRANSFER_SERVER_PRIVATE_MAPPER_H

#include "common/public/util/result.h"
#include "database/public/database/domain/statement/statement_interface.h"
#include "transfer/public/service_types.h"

result_t map_stmt(const Statement *stmt, struct i_statement **result);
void map_err(struct error *err, Error **result);
result_t map_result(struct statement_result *res, StatementResult **result);

#endif // LLP_LAB_TRANSFER_SERVER_INCLUDE_TRANSFER_SERVER_PRIVATE_MAPPER_H
