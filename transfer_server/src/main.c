//
// Created by draen on 23.12.23.
//
#include <glib-object.h>

#include "transfer_server/private/handler.h"
#include <stdio.h>
#include <thrift/c_glib/protocol/thrift_binary_protocol_factory.h>
#include <thrift/c_glib/protocol/thrift_protocol_factory.h>
#include <thrift/c_glib/server/thrift_server.h>
#include <thrift/c_glib/server/thrift_simple_server.h>
#include <thrift/c_glib/thrift.h>
#include <thrift/c_glib/transport/thrift_buffered_transport_factory.h>
#include <thrift/c_glib/transport/thrift_server_socket.h>
#include <thrift/c_glib/transport/thrift_server_transport.h>

ThriftServer *server = NULL;
gboolean sigint_received = FALSE;

static void sigint_handler(int signal_number) {
  THRIFT_UNUSED_VAR(signal_number);

  /* Take note we were called */
  sigint_received = TRUE;

  /* Shut down the server gracefully */
  if (server != NULL)
    thrift_server_stop(server);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: ./server <file_name> <port>");
    return 1;
  }
  char *file_name = argv[1];
  int port = atoi(argv[2]);

  DatabaseServiceHandlerImpl *handler;
  DatabaseServiceProcessor *processor;

  ThriftServerTransport *server_transport;
  ThriftTransportFactory *transport_factory;
  ThriftProtocolFactory *protocol_factory;

  struct sigaction sigint_action;

  GError *error = NULL;
  int exit_status = 0;

#if (!GLIB_CHECK_VERSION(2, 36, 0))
  g_type_init();
#endif

  struct database_manager *database_manager = database_manager_new();
  database_manager_ctor(database_manager, file_name);

  handler = database_service_handler_new(database_manager);
  processor =
      g_object_new(TYPE_DATABASE_SERVICE_PROCESSOR, "handler", handler, NULL);
  server_transport =
      g_object_new(THRIFT_TYPE_SERVER_SOCKET, "port", port, NULL);
  transport_factory =
      g_object_new(THRIFT_TYPE_BUFFERED_TRANSPORT_FACTORY, NULL);
  protocol_factory = g_object_new(THRIFT_TYPE_BINARY_PROTOCOL_FACTORY, NULL);
  server = g_object_new(
      THRIFT_TYPE_SIMPLE_SERVER, "processor", processor, "server_transport",
      server_transport, "input_transport_factory", transport_factory,
      "output_transport_factory", transport_factory, "input_protocol_factory",
      protocol_factory, "output_protocol_factory", protocol_factory, NULL);

  memset(&sigint_action, 0, sizeof(sigint_action));
  sigint_action.sa_handler = sigint_handler;
  sigint_action.sa_flags = SA_RESETHAND;
  sigaction(SIGINT, &sigint_action, NULL);

  printf("Starting the server...");
  thrift_server_serve(server, &error);

  if (!sigint_received) {
    g_message("thrift_server_serve: %s",
              error != NULL ? error->message : "(null)");
    g_clear_error(&error);
  }

  printf("done.");

  g_object_unref(server);
  g_object_unref(transport_factory);
  g_object_unref(protocol_factory);
  g_object_unref(server_transport);

  g_object_unref(processor);
  g_object_unref(handler);

  return exit_status;
}
