#pragma once

#include <iomanip>
#include <plog/Log.h>

namespace eml {
	inline const char *severityToStringLower(plog::Severity severity) {
		switch (severity) {
		case plog::fatal:
			return "fatal";
		case plog::error:
			return "error";
		case plog::warning:
			return "warn";
		case plog::info:
			return "info";
		case plog::debug:
			return "debug";
		case plog::verbose:
			return "verb";
		default:
			return "none";
		}
	}

	template <char const *shizo>
	class TxtFormatter {
	public:
		static plog::util::nstring header() {
			return plog::util::nstring();
		}

		static plog::util::nstring format(const plog::Record &record) {
			tm t;
			plog::util::localtime_s(&t, &record.getTime().time);

			plog::util::nostringstream ss;
			ss << PLOG_NSTR("[") << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
			ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << static_cast<int>(record.getTime().millitm) << PLOG_NSTR("] ");
			if (shizo) {
				ss << PLOG_NSTR("[") << shizo << PLOG_NSTR("] [") << severityToStringLower(record.getSeverity()) << PLOG_NSTR("] ");
			}
			ss << record.getMessage() << PLOG_NSTR("\n");

			return ss.str();
		}
	};
} // namespace eml