module;

#include "jac313/Setup/v001.hpp"

export module jac313.setup.v001;

// Re-export the public toolchain-sensing API under jac313::Setup::v001.
export using jac313::Setup::v001::CaptureResult;
export using jac313::Setup::v001::Toolchain;
export using jac313::Setup::v001::ResolvedToolchain;
export using jac313::Setup::v001::EnvSnapshot;
export using jac313::Setup::v001::run_capture;
export using jac313::Setup::v001::load_registry;
export using jac313::Setup::v001::resolve_toolchain;
export using jac313::Setup::v001::resolve_registry;
export using jac313::Setup::v001::find_toolchain_by_label;
export using jac313::Setup::v001::find_toolchain_by_flag;
export using jac313::Setup::v001::parse_version_major;
export using jac313::Setup::v001::HostSnapshot;
export using jac313::Setup::v001::sense_host;
export using jac313::Setup::v001::ReadinessReport;
export using jac313::Setup::v001::assess_readiness;
export using jac313::Setup::v001::write_filecheck_list;
export using jac313::Setup::v001::ProvisionRecipe;
export using jac313::Setup::v001::load_recipes;
export using jac313::Setup::v001::ProvisionStep;
export using jac313::Setup::v001::ProvisionPlan;
export using jac313::Setup::v001::plan_provision;
export using jac313::Setup::v001::write_setup_script;
export using jac313::Setup::v001::scrub_exported_shell_functions;
export using jac313::Setup::v001::snapshot_env;
export using jac313::Setup::v001::apply_env_delta;
export using jac313::Setup::v001::restore_env;
