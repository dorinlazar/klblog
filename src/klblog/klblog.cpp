#include "systemsettings.hpp"
#include "blog.hpp"
#include <ff/poorconfig.hpp>
#include <kl/kltimecounter.hpp>

int main(int argc, char** argv, char** envp) {
  auto system_settings = std::make_shared<klblog::SystemSettings>(argc, argv, envp);
  if (system_settings->verbosity == klblog::VerbosityLevel::Verbose) {
    kl::log("KLBlog v{} © 2022 Dorin Lazăr, released under GPL v2.1", system_settings->version);
  }

  const kl::TimeCounter time_counter(
      std::format("Blog generation from {}->{}", system_settings->source_folder, system_settings->destination_folder));
  const klblog::Blog blog(system_settings);

  blog.process();

  return 0;
}
