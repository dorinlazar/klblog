#include "klexcept.hpp"

namespace kl {

OperationNotSupported::OperationNotSupported(const Text& op, const Text& reason)
    : std::logic_error(("Operation not supported"_t + op + ": " + reason).to_text().to_string()) {}
InvalidInputData::InvalidInputData(const Text& received, const Text& expected)
    : std::logic_error(("Invalid input data: [" + received + "], expected: " + expected).to_text().to_string()) {}

IOException::IOException(const Text& why) : _why(why) {}
IOException IOException::currentStandardError() { return {strerror(errno)}; }
const Text& IOException::what() { return _why; }

} // namespace kl
