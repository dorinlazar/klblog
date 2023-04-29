#include "blogsettings.hpp"
#include "ff/poorconfig.hpp"

namespace klblog {
const kl::Text DefaultHostUrl{"https://localhost/"};
const kl::Text EmptyText{};
const kl::Text DefaultLanguage{"en"};
const kl::Text DefaultTheme{"default"};
const kl::Text FieldHostUrl{"url"};
const kl::Text FieldTitle{"title"};
const kl::Text FieldCopyright{"copyright"};
const kl::Text FieldLanguage{"language"};
const kl::Text FieldTheme{"theme"};

void Settings::parse(const kl::Text& filename) {
  auto config = kl::PoorConfig(filename);
  auto value = config.top();
  website_url = value->get_opt(FieldHostUrl).value_or(DefaultHostUrl);
  website_title = value->get_opt(FieldTitle).value_or(EmptyText);
  copyright_notice = value->get_opt(FieldCopyright).value_or(EmptyText);
  default_language = value->get_opt(FieldLanguage).value_or(DefaultLanguage);
  theme_name = value->get_opt(FieldTheme).value_or(DefaultTheme);
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
