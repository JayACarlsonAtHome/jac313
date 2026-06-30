#include "host_hardware.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <sys/utsname.h>
#include <vector>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

std::string trim(std::string text) {
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) {
        text.erase(text.begin());
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
        text.pop_back();
    }
    return text;
}

std::optional<std::string> read_kv_file(const fs::path& path, const std::string& key) {
    std::ifstream in(path);
    if (!in) {
        return std::nullopt;
    }
    std::string line;
    const std::string prefix = key + "=";
    while (std::getline(in, line)) {
        if (line.starts_with(prefix)) {
            std::string value = line.substr(prefix.size());
            if (!value.empty() && value.front() == '"') {
                value.erase(value.begin());
            }
            if (!value.empty() && value.back() == '"') {
                value.pop_back();
            }
            return trim(std::move(value));
        }
    }
    return std::nullopt;
}

std::optional<int> cpu_core_count() {
    std::ifstream in("/proc/cpuinfo");
    if (!in) {
        return std::nullopt;
    }
    int count = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (line.starts_with("processor")) {
            ++count;
        }
    }
    return count > 0 ? std::optional<int>{count} : std::nullopt;
}

std::optional<int> max_cpu_mhz() {
    std::ifstream in("/proc/cpuinfo");
    if (!in) {
        return std::nullopt;
    }
    int max_mhz = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.starts_with("cpu MHz")) {
            continue;
        }
        const auto colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        try {
            const int mhz = static_cast<int>(std::stod(trim(line.substr(colon + 1))));
            max_mhz = std::max(max_mhz, mhz);
        } catch (const std::exception&) {
        }
    }
    return max_mhz > 0 ? std::optional<int>{max_mhz} : std::nullopt;
}

// Physical core count = (cpu cores per socket) × (distinct "physical id" sockets). Falls back to
// nullopt (caller uses the logical/thread count) when /proc/cpuinfo omits the topology.
std::optional<int> physical_core_count() {
    std::ifstream in("/proc/cpuinfo");
    if (!in) {
        return std::nullopt;
    }
    int cores_per_socket = 0;
    std::set<std::string> sockets;
    std::string line;
    while (std::getline(in, line)) {
        if (line.starts_with("cpu cores")) {
            const auto colon = line.find(':');
            if (colon != std::string::npos) {
                try { cores_per_socket = std::stoi(trim(line.substr(colon + 1))); } catch (const std::exception&) {}
            }
        } else if (line.starts_with("physical id")) {
            const auto colon = line.find(':');
            if (colon != std::string::npos) {
                sockets.insert(trim(line.substr(colon + 1)));
            }
        }
    }
    if (cores_per_socket > 0) {
        const int socket_count = sockets.empty() ? 1 : static_cast<int>(sockets.size());
        return cores_per_socket * socket_count;
    }
    return std::nullopt;
}

// Min/max clock (MHz) from sysfs cpufreq (file values are kHz). nullopt if unavailable.
std::optional<int> cpu_freq_mhz(const char* cpufreq_file) {
    std::ifstream in(std::string("/sys/devices/system/cpu/cpu0/cpufreq/") + cpufreq_file);
    if (!in) {
        return std::nullopt;
    }
    long khz = 0;
    in >> khz;
    return khz > 0 ? std::optional<int>{static_cast<int>(khz / 1000)} : std::nullopt;
}

std::optional<std::string> first_cpu_model() {
    std::ifstream in("/proc/cpuinfo");
    if (!in) {
        return std::nullopt;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.starts_with("model name")) {
            const auto colon = line.find(':');
            if (colon != std::string::npos) {
                return trim(line.substr(colon + 1));
            }
        }
    }
    return std::nullopt;
}

std::optional<std::string> mem_total_human() {
    std::ifstream in("/proc/meminfo");
    if (!in) {
        return std::nullopt;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (!line.starts_with("MemTotal:")) {
            continue;
        }
        std::istringstream stream(line.substr(9));
        long kb = 0;
        stream >> kb;
        if (kb <= 0) {
            return std::nullopt;
        }
        const long gb = (kb + 512 * 1024) / (1024 * 1024);
        if (gb >= 1) {
            return std::to_string(gb) + " GB RAM";
        }
        return std::to_string((kb + 512) / 1024) + " MB RAM";
    }
    return std::nullopt;
}

std::optional<std::string> os_pretty_name() {
    if (auto pretty = read_kv_file("/etc/os-release", "PRETTY_NAME")) {
        return pretty;
    }
    utsname info{};
    if (uname(&info) == 0) {
        return std::string(info.sysname) + " " + info.release;
    }
    return std::nullopt;
}

std::optional<std::string> hostname() {
    // Public-safety override: prefer a gitignored host_label.local (then $JAC313_HOST_LABEL),
    // so committed results carry a stable label (e.g. jac313-001) instead of the machine's
    // real hostname. Falls back to uname when neither is set.
    {
        std::ifstream in("host_label.local");
        if (in) {
            std::string label;
            std::getline(in, label);
            label = trim(std::move(label));
            if (!label.empty()) {
                return label;
            }
        }
    }
    if (const char* env = std::getenv("JAC313_HOST_LABEL")) {
        std::string label = trim(std::string(env));
        if (!label.empty()) {
            return label;
        }
    }
    utsname info{};
    if (uname(&info) == 0 && info.nodename[0] != '\0') {
        return std::string(info.nodename);
    }
    return std::nullopt;
}

std::optional<std::string> kernel_release() {
    utsname info{};
    if (uname(&info) == 0 && info.release[0] != '\0') {
        return std::string(info.release);
    }
    return std::nullopt;
}

std::string block_device_name(const std::string& device_path) {
    std::string name = fs::path(device_path).filename().string();
    if (name.starts_with("nvme")) {
        const auto partition = name.find('p', 4);
        if (partition != std::string::npos) {
            bool partition_suffix = partition + 1 < name.size();
            for (std::size_t i = partition + 1; partition_suffix && i < name.size(); ++i) {
                if (!std::isdigit(static_cast<unsigned char>(name[i]))) {
                    partition_suffix = false;
                }
            }
            if (partition_suffix) {
                name = name.substr(0, partition);
            }
        }
        return name;
    }
    while (name.size() > 3 && std::isdigit(static_cast<unsigned char>(name.back()))) {
        name.pop_back();
    }
    return name;
}

std::optional<std::string> root_block_device() {
    std::ifstream in("/proc/mounts");
    if (!in) {
        return std::nullopt;
    }
    std::string device;
    std::string mount;
    std::string fstype;
    std::string opts;
    int dump = 0;
    int pass = 0;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream stream(line);
        if (!(stream >> device >> mount >> fstype >> opts >> dump >> pass)) {
            continue;
        }
        if (mount == "/") {
            return block_device_name(device);
        }
    }
    return std::nullopt;
}

std::optional<std::string> read_block_model(const std::string& block_name) {
    const fs::path model_path = fs::path("/sys/block") / block_name / "device" / "model";
    std::ifstream in(model_path);
    if (!in) {
        return std::nullopt;
    }
    std::string model;
    std::getline(in, model);
    return trim(std::move(model));
}

std::string run_capture(const std::string& cmd) {
    std::string out;
    FILE* pipe = ::popen(cmd.c_str(), "r");
    if (!pipe) {
        return out;
    }
    char buf[256];
    while (std::fgets(buf, sizeof(buf), pipe)) {
        out += buf;
    }
    ::pclose(pipe);
    return out;
}

struct PhysDisk {
    std::string name;
    bool rotational{false};
    std::string model;
};

// Physical disk(s) backing `dir` — resolves LVM/dm down to the real devices via `lsblk -s`.
std::vector<PhysDisk> backing_disks(const std::string& dir) {
    std::vector<PhysDisk> disks;
    const std::string src = trim(run_capture("df --output=source '" + dir + "' 2>/dev/null | tail -1"));
    if (src.empty() || src == "Filesystem") {
        return disks;
    }
    std::istringstream rows(run_capture("lsblk -s -nro NAME,ROTA,TYPE,MODEL '" + src + "' 2>/dev/null"));
    std::string line;
    while (std::getline(rows, line)) {
        std::istringstream cols(line);
        std::string name, rota, type;
        if (!(cols >> name >> rota >> type)) {
            continue;
        }
        if (type != "disk") {
            continue;
        }
        std::string model;
        std::getline(cols, model);
        disks.push_back(PhysDisk{name, rota == "1", trim(std::move(model))});
    }
    return disks;
}

// Map a rotational disk to an RPM tier. smartctl is authoritative when permitted (often needs
// root); otherwise fall back to known-model heuristics, defaulting to x7k for unknown HDDs.
std::string rpm_tier(const PhysDisk& disk) {
    const std::string info = run_capture("smartctl -i /dev/" + disk.name + " 2>/dev/null");
    if (const auto p = info.find("Rotation Rate"); p != std::string::npos) {
        const std::string seg = info.substr(p, 48);
        if (seg.find("15000") != std::string::npos) return "15k";
        if (seg.find("10000") != std::string::npos) return "10k";
        if (seg.find("7200") != std::string::npos) return "x7k";
    }
    if (disk.model.find("HUC") != std::string::npos) return "10k";                                   // HGST 10K SAS
    if (disk.model.rfind("ST", 0) == 0 && disk.model.find("NM") != std::string::npos) return "x7k";  // Seagate 7200
    return "x7k";
}

std::optional<std::string> storage_description(const std::string& disk_type_label) {
    std::string out;
    if (const auto disks = backing_disks("."); !disks.empty()) {
        out = disks.front().model.empty() ? disks.front().name : disks.front().model;
    } else if (const auto block = root_block_device()) {
        out = read_block_model(*block).value_or(*block);
    }
    if (!disk_type_label.empty()) {
        out = out.empty() ? (disk_type_label + " label") : (out + " (" + disk_type_label + " label)");
    }
    return out.empty() ? std::nullopt : std::optional<std::string>{out};
}

void append_segment(std::ostringstream& out, bool& first, const std::optional<std::string>& segment) {
    if (!segment || segment->empty()) {
        return;
    }
    if (!first) {
        out << " · ";
    }
    out << *segment;
    first = false;
}

} // namespace

std::string detect_disk_type(const std::string& dir) {
    const auto disks = backing_disks(dir.empty() ? std::string(".") : dir);
    if (disks.empty()) {
        return "";
    }
    for (const auto& d : disks) {
        if (!d.rotational) {
            return "ssd";  // any non-rotating member -> treat as SSD
        }
    }
    return rpm_tier(disks.front());
}

HostHardwareRecord collect_host_hardware_record(const std::string& disk_type_label,
                                                const std::string& os_id)
{
    HostHardwareRecord record;
    record.disk_type_label = disk_type_label;
    record.os_id = os_id;
    if (const auto host = hostname()) {
        record.hostname = *host;
    }
    if (const auto cpu = first_cpu_model()) {
        record.cpu_model = *cpu;
    }
    if (const auto cores = cpu_core_count()) {
        record.cpu_cores = *cores;          // logical/thread count (T.Cores)
    }
    record.p_cores = physical_core_count().value_or(record.cpu_cores);  // physical (P.Cores); fall back to threads
    if (const auto mhz = max_cpu_mhz()) {
        record.cpu_mhz = *mhz;
    }
    if (const auto lo = cpu_freq_mhz("cpuinfo_min_freq")) {
        record.cpu_mhz_min = *lo;
    }
    if (const auto hi = cpu_freq_mhz("cpuinfo_max_freq")) {
        record.cpu_mhz_max = *hi;
    }
    // Fall back to the /proc/cpuinfo clock when sysfs cpufreq is absent (common on VMs): a single
    // momentary reading beats a "-". With no scaling range, min and max collapse to that value.
    if (record.cpu_mhz_min == 0 && record.cpu_mhz > 0) record.cpu_mhz_min = record.cpu_mhz;
    if (record.cpu_mhz_max == 0 && record.cpu_mhz > 0) record.cpu_mhz_max = record.cpu_mhz;
    if (const auto mem = mem_total_human()) {
        std::istringstream stream(*mem);
        int value = 0;
        std::string unit;
        stream >> value >> unit;
        if (unit.starts_with("GB")) {
            record.ram_gb = value;
        }
    }
    if (const auto storage = storage_description(disk_type_label)) {
        record.storage_model = *storage;
    }
    if (const auto os = os_pretty_name()) {
        record.os_pretty = *os;
    }
    if (const auto kernel = kernel_release()) {
        record.kernel_release = *kernel;
    }
    record.hardware_specs = collect_hardware_specs(disk_type_label);
    record.os_specs = collect_os_specs(os_id);
    return record;
}

std::string collect_hardware_specs(const std::string& disk_type_label) {
    std::ostringstream out;
    bool first = true;

    append_segment(out, first, hostname());
    append_segment(out, first, first_cpu_model());
    append_segment(out, first, mem_total_human());
    append_segment(out, first, storage_description(disk_type_label));

    return out.str();
}

std::string collect_os_specs(const std::string& os_id) {
    std::ostringstream out;
    bool first = true;

    if (const auto os = os_pretty_name()) {
        std::ostringstream os_line;
        os_line << *os;
        if (const auto kernel = kernel_release()) {
            os_line << " (" << *kernel << ")";
        }
        append_segment(out, first, std::optional<std::string>{os_line.str()});
    }

    if (!os_id.empty()) {
        append_segment(out, first, std::optional<std::string>{"os_id=" + os_id});
    }

    return out.str();
}

std::string collect_host_hardware_line(const std::string& disk_type_label) {
    return format_group_environment_line(
        collect_hardware_specs(disk_type_label),
        collect_os_specs());
}

std::string format_group_environment_line(const std::string& hardware_specs,
                                          const std::string& os_specs)
{
    if (hardware_specs.empty()) {
        return os_specs;
    }
    if (os_specs.empty()) {
        return hardware_specs;
    }

    const auto sep = hardware_specs.find(" · ");
    if (sep == std::string::npos) {
        return os_specs + " · " + hardware_specs;
    }

    std::string out = hardware_specs.substr(0, sep);
    out += " · ";
    out += os_specs;
    out += hardware_specs.substr(sep);
    return out;
}

std::string hardware_display_line(const std::string& hardware_specs,
                                  const std::string& os_specs,
                                  const std::string& disk_type)
{
    if (!hardware_specs.empty() || !os_specs.empty()) {
        return format_group_environment_line(hardware_specs, os_specs);
    }
    return collect_host_hardware_line(disk_type);
}

std::string html_escape(const std::string& text) {
    std::string out;
    out.reserve(text.size());
    for (char ch : text) {
        switch (ch) {
        case '&': out += "&amp;"; break;
        case '<': out += "&lt;"; break;
        case '>': out += "&gt;"; break;
        case '"': out += "&quot;"; break;
        default: out += ch; break;
        }
    }
    return out;
}

} // namespace jac313::test_cli