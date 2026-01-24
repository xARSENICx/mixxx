#pragma once

#include <QDateTime>
#include <QTimeZone>
#include <QVariant>

#include "util/assert.h"

namespace mixxx {

/// Date format options for library display
enum class DateFormat {
    Native = 0,       ///< System locale default (e.g. 5/20/1998)
    ISO8601 = 1,      ///< YYYY-MM-DD
    DayMonthYear = 2, ///< DD/MM/YYYY
    MonthDayYear = 3, ///< MM/DD/YYYY
    YearMonthDay = 4, ///< YYYY/MM/DD
};

/// Common utility functions for safely converting and consistently
/// displaying date time values.

/// Obtain the local date time from an UTC date time.
inline QDateTime localDateTimeFromUtc(
        const QDateTime& dt) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QDateTime(dt.date(), dt.time(), QTimeZone::UTC).toLocalTime();
#else
    return QDateTime(dt.date(), dt.time(), Qt::UTC).toLocalTime();
#endif
}

/// Extract a QDateTime from a QVariant.
inline QDateTime convertVariantToDateTime(
        const QVariant& data) {
    DEBUG_ASSERT(data.canConvert<QDateTime>());
    return data.toDateTime();
}

/// Helper to format a QDate according to DateFormat
inline QString formatDate(
        const QDate& date,
        DateFormat format) {
    if (!date.isValid()) {
        return QString();
    }
    switch (format) {
    case DateFormat::ISO8601:
        return date.toString(Qt::ISODate);
    case DateFormat::DayMonthYear:
        return date.toString(QStringLiteral("dd/MM/yyyy"));
    case DateFormat::MonthDayYear:
        // Use standard US format MM/DD/YYYY
        return date.toString(QStringLiteral("MM/dd/yyyy"));
    case DateFormat::YearMonthDay:
        return date.toString(QStringLiteral("yyyy/MM/dd"));
    case DateFormat::Native:
    default:
        return QLocale().toString(date, QLocale::ShortFormat);
    }
}

/// Helper to format a QDateTime according to DateFormat
inline QString formatDateTime(
        const QDateTime& dt,
        DateFormat format) {
    if (!dt.isValid()) {
        return QString();
    }
    switch (format) {
    case DateFormat::ISO8601:
        // Qt::ISODate includes 'T' and can include time zone, but usually we
        // just want YYYY-MM-DD HH:mm:ss for display. Let's start with a
        // comprehensive custom format closest to ISO 8601 style preference.
        return dt.toString(QStringLiteral("yyyy-MM-dd hh:mm"));
    case DateFormat::DayMonthYear:
        return dt.toString(QStringLiteral("dd/MM/yyyy hh:mm"));
    case DateFormat::MonthDayYear:
        return dt.toString(QStringLiteral("MM/dd/yyyy hh:mm"));
    case DateFormat::YearMonthDay:
        return dt.toString(QStringLiteral("yyyy/MM/dd hh:mm"));
    case DateFormat::Native:
    default:
        // Uses QLocale::ShortFormat for both Date and Time
        return QLocale().toString(dt, QLocale::ShortFormat);
    }
}

/// Format a QDateTime for display to the user using the
/// application's locale settings or specified preference.
inline QString displayLocalDateTime(
        const QDateTime& dt, DateFormat format = DateFormat::Native) {
    return formatDateTime(dt, format);
}

} // namespace mixxx
