#include "systemsettings.hpp"
#include <queue>

namespace klblog {
const kl::Text VerboseFlag{" - v "};

SystemSettings::SystemSettings(int argc, char** argv, char** envp) {
  std::deque<kl::Text> args;
  kl::check(argc > 0, "internal error: invalid number of arguments: {}", argc);
  for (int i = 1; i < argc; i++) {
    const kl::Text arg(argv[i]);
    if (VerboseFlag == arg) {
      verbosity = VerbosityLevel::Verbose;
    } else {
      arguments.add(arg);
      args.push_back(arg);
    }
  }
  while (*envp != nullptr) {
    auto [var, value] = kl::Text(*envp).split_next_char('=');
    environment.add(var, value);
    envp++;
  }

  try {
    while (!args.empty()) {
      auto arg = args.front();
      args.pop_front();
      if (arg == "-d") {
        source_folder = args.front();
        args.pop_front();
      }
      if (arg == "-o") {
        destination_folder = args.front();
        args.pop_front();
      }
    }
  } catch (...) {
    kl::fatal("usage: {} [-d <source>] [-o <target>]", argv[0]);
  }
}

bool SystemSettings::verbose() const { return verbosity == VerbosityLevel::Verbose; }

} // namespace klblog
