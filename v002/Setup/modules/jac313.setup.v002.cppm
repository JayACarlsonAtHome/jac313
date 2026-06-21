module;

#include "jac313/Setup/v002.hpp"

export module jac313.setup.v002;

// Re-export the public toolchain-sensing API under jac313::Setup::v002.
export using jac313::Setup::v002::CaptureResult;
export using jac313::Setup::v002::Toolchain;
export using jac313::Setup::v002::ResolvedToolchain;
export using jac313::Setup::v002::EnvSnapshot;
export using jac313::Setup::v002::run_capture;
export using jac313::Setup::v002::load_registry;
export using jac313::Setup::v002::resolve_toolchain;
export using jac313::Setup::v002::resolve_registry;
export using jac313::Setup::v002::find_toolchain_by_label;
export using jac313::Setup::v002::find_toolchain_by_flag;
export using jac313::Setup::v002::parse_version_major;
export using jac313::Setup::v002::HostSnapshot;
export using jac313::Setup::v002::sense_host;
export using jac313::Setup::v002::ReadinessReport;
export using jac313::Setup::v002::assess_readiness;
export using jac313::Setup::v002::write_filecheck_list;
export using jac313::Setup::v002::ProvisionRecipe;
export using jac313::Setup::v002::load_recipes;
export using jac313::Setup::v002::ProvisionStep;
export using jac313::Setup::v002::ProvisionPlan;
export using jac313::Setup::v002::plan_provision;
export using jac313::Setup::v002::write_setup_script;
export using jac313::Setup::v002::scrub_exported_shell_functions;
export using jac313::Setup::v002::snapshot_env;
export using jac313::Setup::v002::apply_env_delta;
export using jac313::Setup::v002::restore_env;
