#include "blog.hpp"
#include <kl/kl.hpp>
#include <kl/klfs.hpp>

namespace klblog {

Blog::Blog(std::shared_ptr<SystemSettings> sys) {
  const kl::FilePath config_file{sys->source_folder + "/blog.config"};
  if (sys->verbose()) {
    kl::log("Blog processing started: {} => {}", sys->source_folder, config_file.full_path());
  }
  m_settings.parse(config_file.full_path());
  if (sys->verbose()) {
    m_settings.log_abstract();
  }
}

void Blog::process() const {}

} // namespace klblog
