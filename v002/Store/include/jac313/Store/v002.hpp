#pragma once

/**
 * jac313::Store::v002 — core time-series event store.
 *
 *   #include <jac313/Store/v002.hpp>
 *   using Config = jac313::Store::v002::ts_store_config<...>;
 *   jac313::Store::v002::ts_store<Config> store(threads, events_per_thread);
 *
 * Module consumers: import jac313.store.v002;
 */

#include <jac313/Store/v002/headers/ts_store.hpp>
#include <jac313/Store/v002/headers/metric_view.hpp>