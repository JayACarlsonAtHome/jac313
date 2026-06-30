//File:    persist_keeper.sql
//Date:    2026-06-18
//Purpose: SQL Schema File
//Related: type=ts_store table=persist_keeper
//
CREATE TABLE IF NOT EXISTS persist_keeper (
    id BIGINT PRIMARY KEY
  , thread_id BIGINT
  , per_thread_event_id BIGINT
  , flags_raw BIGINT
  , category TEXT
  , payload TEXT
  , timestamp_us BIGINT
);

