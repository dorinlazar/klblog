#include "kl/kltext.hpp"
#include <gtest/gtest.h>
using namespace kl;
using namespace std::literals;
using namespace kl::literals;

TEST(kltextview, text_view_construction) {
  TextView txt("Hello world", 3);
  EXPECT_EQ(txt.size(), 3);
  EXPECT_EQ(txt.view(), "Hel");

  TextView tnull(nullptr, 2);
  EXPECT_EQ(tnull.size(), 0);
  EXPECT_EQ(tnull.view(), "");
  auto tn2 = TextView(tnull);
  EXPECT_EQ(tn2.size(), 0);
  EXPECT_EQ(tn2.view(), "");
  auto tn3 = TextView(std::move(tnull));
  EXPECT_EQ(tn3.size(), 0);
  EXPECT_EQ(tn3.view(), "");
  auto tn4 = TextView(nullptr);
  EXPECT_EQ(tn4.size(), 0);
  EXPECT_EQ(tn4.view(), "");

  TextView tempty("");
  EXPECT_EQ(tempty.size(), 0);
  EXPECT_EQ(tempty.view(), "");

  TextView tempty2;
  EXPECT_EQ(tempty2.size(), 0);
  EXPECT_EQ(tempty2.view(), "");

  TextView t1("Hello World", 25);
  EXPECT_EQ(t1.size(), 11);
  EXPECT_EQ(t1.view(), "Hello World");
  TextView t1_1(std::move(t1));
  EXPECT_EQ(t1_1.size(), 11);
  EXPECT_EQ(t1_1.view(), "Hello World");

  TextView t3("Hello World\0", 12);
  EXPECT_EQ(t3.size(), 11);
  EXPECT_EQ(t3[t3.size() - 1], 'd');

  TextView t4("Hello World\0Other World", 23);
  EXPECT_EQ(t4.size(), 11);
  EXPECT_EQ(t4[10], 'd');
}

TEST(kltextview, test_trimming) {
  TextView txt(" \t hello   \n ");
  EXPECT_EQ(txt.trim().view(), "hello");
  EXPECT_EQ(txt.trim_left().view(), "hello   \n ");
  EXPECT_EQ(txt.trim_right().view(), " \t hello"s);

  TextView txt2("hel    lo");
  EXPECT_EQ(txt2.trim().view(), "hel    lo");
  EXPECT_EQ(txt2.trim_left().view(), "hel    lo");
  EXPECT_EQ(txt2.trim_right().view(), "hel    lo");

  TextView txt3;
  EXPECT_EQ(txt3.trim().view(), "");
  EXPECT_EQ(txt3.trim_left().view(), "");
  EXPECT_EQ(txt3.trim_right().view(), "");
}
TEST(kltextview, test_starts_with) {
  TextView txt("Hello");
  EXPECT_TRUE(txt.starts_with('H'));
  EXPECT_FALSE(txt.starts_with('e'));

  EXPECT_TRUE(txt.starts_with(""));
  EXPECT_TRUE(txt.starts_with("H"));
  EXPECT_TRUE(txt.starts_with("Hell"));
  EXPECT_TRUE(txt.starts_with("Hello"));
  EXPECT_FALSE(txt.starts_with("Hello world"));
  EXPECT_FALSE(txt.starts_with("world"));

  EXPECT_TRUE(txt.starts_with(""sv));
  EXPECT_TRUE(txt.starts_with("H"sv));
  EXPECT_TRUE(txt.starts_with("Hell"sv));
  EXPECT_TRUE(txt.starts_with("Hello"sv));
  EXPECT_FALSE(txt.starts_with("Hello world"sv));
  EXPECT_FALSE(txt.starts_with("world"sv));

  EXPECT_TRUE(txt.starts_with(""_tv));
  EXPECT_TRUE(txt.starts_with("H"_tv));
  EXPECT_TRUE(txt.starts_with("Hell"_tv));
  EXPECT_TRUE(txt.starts_with("Hello"_tv));
  EXPECT_FALSE(txt.starts_with("Hello world"_tv));
  EXPECT_FALSE(txt.starts_with("world"_tv));
}

TEST(kltextview, test_ends_with) {
  TextView txt("Hello");
  EXPECT_TRUE(txt.ends_with('o'));
  EXPECT_FALSE(txt.ends_with('H'));

  EXPECT_TRUE(txt.ends_with(""));
  EXPECT_TRUE(txt.ends_with("o"));
  EXPECT_TRUE(txt.ends_with("ello"));
  EXPECT_TRUE(txt.ends_with("Hello"));
  EXPECT_FALSE(txt.ends_with("Hello world"));
  EXPECT_FALSE(txt.ends_with("world"));

  EXPECT_TRUE(txt.ends_with(""sv));
  EXPECT_TRUE(txt.ends_with("o"sv));
  EXPECT_TRUE(txt.ends_with("ello"sv));
  EXPECT_TRUE(txt.ends_with("Hello"sv));
  EXPECT_FALSE(txt.ends_with("Hello world"sv));
  EXPECT_FALSE(txt.ends_with("world"sv));

  EXPECT_TRUE(txt.ends_with(""_tv));
  EXPECT_TRUE(txt.ends_with("o"_tv));
  EXPECT_TRUE(txt.ends_with("ello"_tv));
  EXPECT_TRUE(txt.ends_with("Hello"_tv));
  EXPECT_FALSE(txt.ends_with("Hello world"_tv));
  EXPECT_FALSE(txt.ends_with("world"_tv));
}

TEST(kltextview, test_comparisons) {
  TextView hello("hello");
  TextView hfllo("hfllo");
  TextView empty;

  EXPECT_EQ(hello, "hello");
  EXPECT_TRUE(hello < "hfllo");
  EXPECT_TRUE(hello <= "hfllo");
  EXPECT_TRUE(hello != "hfllo");
  EXPECT_TRUE(hello >= "hallo");
  EXPECT_TRUE(hello >= "ahllo");
  EXPECT_TRUE(hello > "hallo");
  EXPECT_TRUE(hello > "hell");
  EXPECT_TRUE(hello < "hello world");
  EXPECT_TRUE(hello > "");
  EXPECT_TRUE(hello > nullptr);

  EXPECT_EQ(hello, "hello"s);
  EXPECT_TRUE(hello < "hfllo"s);
  EXPECT_TRUE(hello <= "hfllo"s);
  EXPECT_TRUE(hello != "hfllo"s);
  EXPECT_TRUE(hello >= "hallo"s);
  EXPECT_TRUE(hello >= "ahllo"s);
  EXPECT_TRUE(hello > "hallo"s);
  EXPECT_TRUE(hello > "hell"s);
  EXPECT_TRUE(hello < "hello world"s);
  EXPECT_TRUE(hello > ""s);

  EXPECT_EQ(hello, "hello"sv);
  EXPECT_TRUE(hello < "hfllo"sv);
  EXPECT_TRUE(hello <= "hfllo"sv);
  EXPECT_TRUE(hello != "hfllo"sv);
  EXPECT_TRUE(hello >= "hallo"sv);
  EXPECT_TRUE(hello >= "ahllo"sv);
  EXPECT_TRUE(hello > "hallo"sv);
  EXPECT_TRUE(hello > "hell"sv);
  EXPECT_TRUE(hello < "hello world"sv);
  EXPECT_TRUE(hello > ""sv);

  EXPECT_EQ(hello, "hello"_tv);
  EXPECT_TRUE(hello < "hfllo"_tv);
  EXPECT_TRUE(hello <= "hfllo"_tv);
  EXPECT_TRUE(hello != "hfllo"_tv);
  EXPECT_TRUE(hello >= "hallo"_tv);
  EXPECT_TRUE(hello >= "ahllo"_tv);
  EXPECT_TRUE(hello > "hallo"_tv);
  EXPECT_TRUE(hello > "hell"_tv);
  EXPECT_TRUE(hello < "hello world"_tv);
  EXPECT_TRUE(hello > ""_tv);
}

TEST(kltextview, test_split) {
  TextView txt("hello");
  EXPECT_EQ(get<0>(txt.split_pos(0)), "");
  EXPECT_EQ(get<1>(txt.split_pos(0)), "hello");
  EXPECT_EQ(get<0>(txt.split_pos(1)), "h");
  EXPECT_EQ(get<1>(txt.split_pos(1)), "ello");
  EXPECT_EQ(get<0>(txt.split_pos(2)), "he");
  EXPECT_EQ(get<1>(txt.split_pos(2)), "llo");
  EXPECT_EQ(get<0>(txt.split_pos(-2)), "hel");
  EXPECT_EQ(get<1>(txt.split_pos(-2)), "lo");
  EXPECT_EQ(get<0>(txt.split_pos(-1)), "hell");
  EXPECT_EQ(get<1>(txt.split_pos(-1)), "o");

  EXPECT_EQ(get<0>(txt.split_pos(-5)), "");
  EXPECT_EQ(get<1>(txt.split_pos(-5)), "hello");
  EXPECT_EQ(get<0>(txt.split_pos(-10)), "");
  EXPECT_EQ(get<1>(txt.split_pos(-10)), "hello");

  EXPECT_EQ(get<0>(txt.split_pos(5)), "hello");
  EXPECT_EQ(get<1>(txt.split_pos(5)), "");
  EXPECT_EQ(get<0>(txt.split_pos(15)), "hello");
  EXPECT_EQ(get<1>(txt.split_pos(15)), "");
  EXPECT_EQ(get<0>(""_tv.split_pos(15)), "");
  EXPECT_EQ(get<1>(""_tv.split_pos(15)), "");
}

TEST(kltextview, test_contains) {
  TextView t("hello");
  EXPECT_TRUE(t.contains('h'));
  EXPECT_TRUE(t.contains('e'));
  EXPECT_TRUE(t.contains('o'));
  EXPECT_TRUE(t.contains('l'));
  EXPECT_FALSE(t.contains('a'));
  EXPECT_FALSE(t.contains('\0'));
  EXPECT_FALSE(""_tv.contains('\0'));
}

TEST(kltextview, test_skip) {
  TextView t("hello");
  EXPECT_EQ(t.skip("hel"), "o");
  EXPECT_EQ(t.skip("el"), "hello");
  EXPECT_EQ(t.skip(""), "hello");
  EXPECT_EQ(t.skip("abcdefghijklmnopqrstuvwxyz"), "");
  EXPECT_EQ(""_tv.skip("abcdefghijklmnopqrstuvwxyz"), "");

  EXPECT_EQ(t.skip(2), "llo"_tv);
  EXPECT_EQ(t.skip(1), "ello"_tv);
  EXPECT_EQ(t.skip(4), "o"_tv);
  EXPECT_EQ(t.skip(5), ""_tv);
  EXPECT_EQ(t.skip(10), ""_tv);
  EXPECT_EQ(t.skip(100), ""_tv);
}
