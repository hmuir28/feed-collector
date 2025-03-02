#include <iostream>
#include "../db/db.hpp"

void insertFeed(const RSSItem& rssItem) {
  CassSession* session = connect_to_cassandra();
  if (!session) return;


  // Create Keyspace and Table
  execute_query(session, "CREATE KEYSPACE IF NOT EXISTS smart_rss WITH replication = {'class': 'SimpleStrategy', 'replication_factor': 1};");
  execute_query(session, "USE smart_rss;");
  execute_query(session, "CREATE TABLE IF NOT EXISTS rss_feed (id UUID PRIMARY KEY, title text, link text, description text);");

  execute_query(session, "INSERT INTO rss_feed (id, title, link, description) VALUES (uuid(), '" + rssItem.title + "', '" + rssItem.link + "', '" + rssItem.description + "');");

  // Close session
  cass_session_close(session);
  cass_session_free(session);
}
