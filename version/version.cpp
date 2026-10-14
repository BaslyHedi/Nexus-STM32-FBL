#include "version/version.hpp"

namespace nexus::fbl::version {

namespace {

// C++ does not allow `char type[3] = "FBL"` (C does): the NUL has no room. Copy without it.
template <std::size_t N>
constexpr void copyField(char (&destination)[N], const char (&source)[N + 1U]) noexcept
{
    for (std::size_t i = 0; i < N; ++i) {
        destination[i] = source[i];
    }
}

constexpr Record makeRecord() noexcept
{
    Record record{};
    copyField(record.type, kSoftwareType);
    record.separator0 = '_';
    copyField(record.platform, kPlatformName);
    record.separator1 = '_';
    copyField(record.platformVersion, kPlatformVersion);
    record.separator2 = '_';
    copyField(record.swVersion, kSoftwareVersion);
    record.terminator = '\0';
    for (std::uint8_t& byte : record.reserved) {
        byte = kReservedFill;
    }
    return record;
}

}  // namespace

// `used` and KEEP in fbl.ld: nothing references the record, the tools read it from the image.
[[gnu::section(".version"), gnu::used]] extern const Record kRecord = makeRecord();

}  // namespace nexus::fbl::version
