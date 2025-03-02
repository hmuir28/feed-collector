#include <cassandra.h>
#include <string>

void execute_query(CassSession* session, const std::string& query) {
  CassStatement* statement = cass_statement_new(query.c_str(), 0);
  CassFuture* future = cass_session_execute(session, statement);
  cass_statement_free(statement);

  if (cass_future_error_code(future) == CASS_OK) {
    std::cout << "Query executed successfully: " << query << std::endl;
  } else {
    const char* message;
    size_t message_length;
    cass_future_error_message(future, &message, &message_length);
    std::cerr << "Query failed: " << std::string(message, message_length) << std::endl;
  }
  cass_future_free(future);
}

CassSession* connect_to_cassandra() {
  CassCluster* cluster = cass_cluster_new();
  cass_cluster_set_contact_points(cluster, "127.0.0.1");  // Cassandra host

  CassSession* session = cass_session_new();
  CassFuture* connect_future = cass_session_connect(session, cluster);

  cass_cluster_free(cluster);

  if (cass_future_error_code(connect_future) == CASS_OK) {
    cass_future_free(connect_future);
    return session;
  } else {
    std::cerr << "Failed to connect to Cassandra!" << std::endl;
    cass_future_free(connect_future);
    cass_session_free(session);
    return nullptr;
  }
}
