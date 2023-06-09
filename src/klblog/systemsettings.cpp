#include "systemsettings.hpp"

namespace klblog {
const kl::Text VerboseFlag{" - v "};

SystemSettings::SystemSettings(int argc, char** argv, char** envp) {
  kl::check(argc > 0, "internal error: invalid number of arguments: {}", argc);
  for (int i = 1; i < argc; i++) {
    const kl::Text arg(argv[i]);
    if (VerboseFlag == arg) {
      verbosity = VerbosityLevel::Verbose;
    } else {
      arguments.add(arg);
    }
  }
  while (*envp != nullptr) {
    auto [var, value] = kl::Text(*envp).split_next_char('=');
    environment.add(var, value);
    envp++;
  }

  try {
    kl::Queue<kl::Text> args;
    args.push(arguments);
    while (!args.empty()) {
      auto arg = args.pop();
      if (arg == "-d") {
        source_folder = args.pop();
      }
      if (arg == "-o") {
        destination_folder = args.pop();
      }
    }
  } catch (...) {
    kl::fatal("usage: {} [-d <source>] [-o <target>]", argv[0]);
  }
}

bool SystemSettings::verbose() const { return verbosity == VerbosityLevel::Verbose; }

} // namespace klblog
