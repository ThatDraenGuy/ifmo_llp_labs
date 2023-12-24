//
// Created by draen on 23.12.23.
//

#include "transfer_server/private/handler.h"
#include "transfer_server/private/mapper.h"
#include <thrift/c_glib/thrift.h>

G_DEFINE_TYPE(DatabaseServiceHandlerImpl, database_service_handler_impl,
              TYPE_DATABASE_SERVICE_HANDLER)

static gboolean execute(DatabaseServiceIf *iface, StatementResult **g_result,
                        const Statement *g_stmt, Error **err, GError **g_err) {
  THRIFT_UNUSED_VAR(g_err);

  DatabaseServiceHandlerImpl *self;

  g_return_val_if_fail(IS_DATABASE_SERVICE_HANDLER_IMPL(iface), FALSE);
  self = DATABASE_SERVICE_HANDLER_IMPL(iface);

  struct i_statement *stmt;
  TRY(map_stmt(g_stmt, &stmt));
  CATCH(error, {
    map_err(error, err);
    return FALSE;
  })

  struct statement_result *result;
  TRY(database_manager_execute_statement(self->manager, stmt, &result));
  CATCH(error, {
    map_err(error, err);
    return FALSE;
  })

  TRY(map_result(result, g_result));
  CATCH(error, {
    map_err(error, err);
    return FALSE;
  })
  return TRUE;
}

static void
database_service_handler_impl_init(DatabaseServiceHandlerImpl *self) {}
static void database_service_handler_impl_finalize(GObject *object) {
  DatabaseServiceHandlerImpl *self = DATABASE_SERVICE_HANDLER_IMPL(object);

  database_manager_destroy(self->manager);

  G_OBJECT_CLASS(database_service_handler_impl_parent_class)->finalize(object);
}

static void database_service_handler_impl_class_init(
    DatabaseServiceHandlerImplClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  DatabaseServiceHandlerClass *database_service_handler_class =
      DATABASE_SERVICE_HANDLER_CLASS(klass);

  gobject_class->finalize = database_service_handler_impl_finalize;

  database_service_handler_class->execute = execute;
}

DatabaseServiceHandlerImpl *
database_service_handler_new(struct database_manager *manager) {
  DatabaseServiceHandlerImpl *handler =
      g_object_new(TYPE_DATABASE_SERVICE_HANDLER_IMPL, NULL);
  handler->manager = manager;
  return handler;
}
