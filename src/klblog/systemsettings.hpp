#pragma once

#include <kl/kltext.hpp>
#include <kl/klds.hpp>

namespace klblog {

constexpr auto KlBlogVersion{"0.0.1"};

enum class VerbosityLevel { Verbose, Quiet };

struct SystemSettings {
  SystemSettings(int argc, char** argv, char** envp);

  [[nodiscard]] bool verbose() const;

  kl::Dict<kl::Text, kl::Text> environment;
  kl::List<kl::Text> arguments;
  kl::Text version = KlBlogVersion;
  VerbosityLevel verbosity = VerbosityLevel::Quiet;
  kl::Text source_folder = ".";
  kl::Text destination_folder = "blog/";
};

} // namespace klblog
