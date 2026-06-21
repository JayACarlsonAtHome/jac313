#include "BinaryEventLogReader.hpp"
#include "JTextSplitEventLog.hpp"

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v001 {
#endif

void BinaryEventLogReader::convert_to_jtext(std::string_view output_base_name,
                                            size_t int_count,
                                            size_t dbl_count) const
{
    JTextSplitEventLog jtext_log(output_base_name, int_count, dbl_count, PersistMode::All);

    BinaryEventLogReader reader(filepath_);
    BinaryRecord rec;

    while (reader.next(rec)) {
        jtext_log.append_event(
            rec.event_id,
            rec.thread_id,
            rec.per_thread_event_id,
            rec.raw_flags,
            rec.category,
            rec.payload,
            rec.timestamp_us,
            rec.int_metrics,
            rec.dbl_metrics
        );
    }

    jtext_log.finalize();
}

#ifndef JAC313_STORE_IMPORT_STD
} // namespace jac313::Store::v001
#endif