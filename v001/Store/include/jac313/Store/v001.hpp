#pragma once

/**
 * jac313::Store::v001 — core time-series event store.
 *
 *   #include <jac313/Store/v001.hpp>
 *   using Config = jac313::Store::v001::ts_store_config<...>;
 *   jac313::Store::v001::ts_store<Config> store(threads, events_per_thread);
 *
 * Module consumers: import jac313.store.v001;
 */

#include <jac313/Store/v001/headers/ts_store.hpp>
#include <jac313/Store/v001/headers/metric_view.hpp>