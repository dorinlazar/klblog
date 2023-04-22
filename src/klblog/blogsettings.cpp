#include "blogsettings.hpp"
#include "ff/poorconfig.hpp"

using namespace kl::literals;

namespace klblog {

void Settings::parse(const kl::Text& filename) {
  auto config = kl::PoorConfig(filename);
  auto value = config.top();
  website_url = value->get_opt("url").value_or("https://localhost/");
  website_title = value->get_opt("title").value_or(""_t);
  copyright_notice = value->get_opt("copyright").value_or(""_t);
  default_language = value->get_opt("language").value_or("en"_t);
  theme_name = value->get_opt("theme").value_or("default"_t);
}

void Settings::log_abstract() const {
  kl::log("Website: {} => {}", website_title, website_url);
  kl::log("Theme: {}", theme_name);
}

kl::Text website_url;      // url: https://example.com
kl::Text website_title;    // title: Example blog
kl::Text copyright_notice; // copyright: "© 2022 Author Name. All rights reserved."

kl::Text default_language; // language: ro
kl::Text theme_name;       // theme: theme_folder_name ; themes/theme_folder_name

} // namespace klblog
