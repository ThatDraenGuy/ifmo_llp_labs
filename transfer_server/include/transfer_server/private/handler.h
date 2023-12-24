//
// Created by draen on 23.12.23.
//

#ifndef LLP_LAB_TRANSFER_SERVER_INCLUDE_TRANSFER_SERVER_PRIVATE_HANDLER_H
#define LLP_LAB_TRANSFER_SERVER_INCLUDE_TRANSFER_SERVER_PRIVATE_HANDLER_H

#include "database/public/database/database_manager.h"
#include "transfer/public/database_service.h"
#include <glib-object.h>

G_BEGIN_DECLS
#define TYPE_DATABASE_SERVICE_HANDLER_IMPL                                     \
  (database_service_handler_impl_get_type())

#define DATABASE_SERVICE_HANDLER_IMPL(obj)                                     \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_DATABASE_SERVICE_HANDLER_IMPL,       \
                              DatabaseServiceHandlerImpl))
#define DATABASE_SERVICE_HANDLER_IMPL_CLASS(c)                                 \
  (G_TYPE_CHECK_CLASS_CAST((c), TYPE_DATABASE_SERVICE_HANDLER_IMPL,            \
                           DatabaseServiceHandlerImplClass))
#define IS_DATABASE_SERVICE_HANDLER_IMPL(obj)                                  \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYPE_DATABASE_SERVICE_HANDLER_IMPL))
#define IS_DATABASE_SERVICE_HANDLER_IMPL_CLASS(c)                              \
  (G_TYPE_CHECK_CLASS_TYPE((c), TYPE_DATABASE_SERVICE_HANDLER_IMPL))
#define DATABASE_SERVICE_HANDLER_IMPL_GET_CLASS(obj)                           \
  (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_DATABASE_SERVICE_HANDLER_IMPL,        \
                             DatabaseServiceHandlerImplClass))

struct _DatabaseServiceHandlerImpl {
  DatabaseServiceHandler *parent;
  struct database_manager *manager;
};
typedef struct _DatabaseServiceHandlerImpl DatabaseServiceHandlerImpl;

struct _DatabaseServiceHandlerImplClass {
  DatabaseServiceHandlerClass parent_class;
};
typedef struct _DatabaseServiceHandlerImplClass DatabaseServiceHandlerImplClass;

GType database_service_handler_impl_get_type();
G_END_DECLS

DatabaseServiceHandlerImpl *
database_service_handler_new(struct database_manager *manager);

#endif // LLP_LAB_TRANSFER_SERVER_INCLUDE_TRANSFER_SERVER_PRIVATE_HANDLER_H
