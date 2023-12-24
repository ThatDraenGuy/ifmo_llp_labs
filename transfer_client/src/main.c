//
// Created by draen on 17.12.23.
//

#include <glib-object.h>
#include <stdbool.h>
#include <stdio.h>

#include <thrift/c_glib/protocol/thrift_binary_protocol.h>
#include <thrift/c_glib/transport/thrift_buffered_transport.h>
#include <thrift/c_glib/transport/thrift_socket.h>

#include "common/public/error/error_handler.h"
#include "common/public/util/result.h"
#include "parser/public/parser.h"
#include "transfer/public/database_service.h"
#include "transfer_client/private/mapper.h"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: ./client <host> <port>");
    return 1;
  }
  char *hostname = argv[1];
  int port = atoi(argv[2]);

  ThriftSocket *socket;
  ThriftTransport *transport;
  ThriftProtocol *protocol;
  DatabaseServiceIf *client;

  GError *g_err = NULL;
  Error *database_error = NULL;

  bool working = true;
  int exit_status = 0;

#if (!GLIB_CHECK_VERSION(2, 36, 0))
  g_type_init();
#endif

  socket = g_object_new(THRIFT_TYPE_SOCKET, "hostname", hostname, "port", port,
                        NULL);
  transport =
      g_object_new(THRIFT_TYPE_BUFFERED_TRANSPORT, "transport", socket, NULL);
  protocol =
      g_object_new(THRIFT_TYPE_BINARY_PROTOCOL, "transport", transport, NULL);

  thrift_transport_open(transport, &g_err);

  client = g_object_new(TYPE_DATABASE_SERVICE_CLIENT, "input_protocol",
                        protocol, "output_protocol", protocol, NULL);

  while (working) {
    Statement *statement;
    StatementResult *result = NULL;
    struct i_ast_node *tree;
    struct queue *queue;

    TRY(parse_stdin(&tree));
    CATCH(err, {
      handle_error(err);
      continue;
    })

    ast_node_print(tree);

    TRY(map_stmt(tree, &statement));
    CATCH(err, {
      handle_error(err);
      continue;
    })

    result = g_object_new(TYPE_STATEMENT_RESULT, NULL);

    if (!g_err && database_service_if_execute(client, &result, statement,
                                              &database_error, &g_err)) {
      //      queue = map_into_queue(result);
      printf("Successfully executed statement\n");
      //      if (queue != NULL) {
      //        // TODO print select res
      //      }
    }

    if (g_err) {
      working = false;
    }
  }

  if (g_err) {
    printf("ERROR: %s\n", g_err->message);
    g_clear_error(&g_err);

    exit_status = 1;
  }

  thrift_transport_close(transport, NULL);

  g_object_unref(client);
  g_object_unref(protocol);
  g_object_unref(transport);
  g_object_unref(socket);

  return exit_status;
}