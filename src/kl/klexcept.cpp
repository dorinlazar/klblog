#include "klexcept.hpp"

namespace kl {

OperationNotSupported::OperationNotSupported(const Text& op, const Text& reason)
    : std::logic_error(("Operation not supported"_t + op + ": " + reason).to_text().to_string()) {}
InvalidInputData::InvalidInputData(const Text& received, const Text& expected)
    : std::logic_error(("Invalid input data: [" + received + "], expected: " + expected).to_text().to_string()) {}

IOException::IOException(const Text& why) : m_why(why) {}
IOException IOException::current_standard_error() { return IOException{strerror(errno)}; }
const Text& IOException::what() { return m_why; }

} // namespace kl
