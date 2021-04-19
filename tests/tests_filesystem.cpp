/*
 * tests_filesystem.cpp
 *
 * Copyright 2009-2021
 * Giuseppe Penone <giuspen@gmail.com>
 * Evgenii Gurianov <https://github.com/txe>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "ct_filesystem.h"
#include "tests_common.h"
#include <fstream>

TEST(FileSystemGroup, path_stem)
{
#ifndef _WIN32
    ASSERT_STREQ("", fs::path("").stem().c_str());
    ASSERT_STREQ("file", fs::path("/root/file").stem().c_str());
    ASSERT_STREQ("file", fs::path("/root/file.txt").stem().c_str());
    ASSERT_STREQ("file.2", fs::path("/root/file.2.txt").stem().c_str());
    ASSERT_STREQ(".txt", fs::path("/root/.txt").stem().c_str());
#endif
}

TEST(FileSystemGroup, path_extension)
{
    ASSERT_STREQ("", fs::path("").extension().c_str());
    ASSERT_STREQ("", fs::path(".").extension().c_str());
    ASSERT_STREQ(".txt", fs::path("file.txt").extension().c_str());
    ASSERT_STREQ("", fs::path("file").extension().c_str());
    ASSERT_STREQ(".txt", fs::path("root/file.txt").extension().c_str());
    ASSERT_STREQ(".txt", fs::path("/root/file.txt").extension().c_str());
    ASSERT_STREQ(".txt", fs::path(".local/file.txt").extension().c_str());
    ASSERT_STREQ(".my_bar", fs::path(".local/file.txt/.....my_bar").extension().c_str());
    ASSERT_STREQ(".txt", fs::path("/home/foo/.local/file.txt/.....my_bar/txt.txt").extension().c_str());
    ASSERT_STREQ("", fs::path("/home/foo/.local/file.txt/.....my_bar/txt.").extension().c_str());
    ASSERT_STREQ(".txt", fs::path("foo..txt").extension().c_str());
    ASSERT_STREQ("", fs::path("/home/foo/.local").extension().c_str());
    ASSERT_STREQ("", fs::path("/home/foo/.config").extension().c_str());
}

TEST(FileSystemGroup, get_cherrytree_datadir)
{
    // we expect the unit test to be run from the built sources
    ASSERT_STREQ(_CMAKE_SOURCE_DIR, fs::get_cherrytree_datadir().c_str());
}

TEST(FileSystemGroup, get_cherrytree_localedir)
{
    // we expect the unit test to be run from the built sources
    ASSERT_STREQ(fs::canonical(Glib::build_filename(_CMAKE_SOURCE_DIR, "po")).c_str(), fs::get_cherrytree_localedir().c_str());
}

TEST(FileSystemGroup, is_regular_file)
{
    ASSERT_TRUE(fs::is_regular_file(UT::unitTestsDataDir + "/test.ctd"));
    ASSERT_TRUE(fs::is_regular_file(UT::unitTestsDataDir + "/test.ctb"));
    ASSERT_TRUE(fs::is_regular_file(UT::unitTestsDataDir + "/md_testfile.md"));
    ASSERT_FALSE(fs::is_regular_file(UT::unitTestsDataDir));
    ASSERT_FALSE(fs::is_regular_file(fs::absolute(UT::unitTestsDataDir)));
    ASSERT_FALSE(fs::is_regular_file(fs::absolute(UT::unitTestsDataDir).parent_path()));
}

TEST(FileSystemGroup, is_directory)
{
    ASSERT_TRUE(fs::is_directory(UT::unitTestsDataDir));
    ASSERT_FALSE(fs::is_directory(UT::unitTestsDataDir + "/test.ctd"));
    ASSERT_TRUE(fs::is_directory(fs::path(UT::unitTestsDataDir).parent_path()));
    ASSERT_FALSE(fs::is_directory(fs::path(UT::unitTestsDataDir).parent_path() / "test_consts.h"));
}

TEST(FileSystemGroup, get_doc_type)
{
    ASSERT_TRUE(CtDocType::SQLite == fs::get_doc_type(UT::unitTestsDataDir + "/test.ctb"));
    ASSERT_TRUE(CtDocType::XML == fs::get_doc_type(UT::unitTestsDataDir + "/test.ctd"));
    ASSERT_TRUE(CtDocType::None == fs::get_doc_type(UT::unitTestsDataDir + "/md_testfile.md"));
    ASSERT_TRUE(CtDocType::SQLite == fs::get_doc_type(UT::unitTestsDataDir + "/mimetype_ctb.ctb"));
    ASSERT_TRUE(CtDocType::XML == fs::get_doc_type(UT::unitTestsDataDir + "/7zr.ctz"));
    ASSERT_TRUE(CtDocType::SQLite == fs::get_doc_type("test.ctx")); // Doesnt actually exist
}

TEST(FileSystemGroup, get_doc_encrypt)
{
    ASSERT_TRUE(fs::get_doc_encrypt(UT::unitTestsDataDir + "/test.ctb") == CtDocEncrypt::False);
    ASSERT_TRUE(fs::get_doc_encrypt(UT::unitTestsDataDir + "/test.ctz") == CtDocEncrypt::True);
    ASSERT_TRUE(fs::get_doc_encrypt(UT::unitTestsDataDir + "/mimetype_txt.txt") == CtDocEncrypt::None);
}

TEST(FileSystemGroup, path_is_absolute)
{
    ASSERT_TRUE(fs::path("/home/foo").is_absolute());
    ASSERT_FALSE(fs::path("/home/foo").is_relative());
    ASSERT_TRUE(fs::path("home/foo").is_relative());
    ASSERT_FALSE(fs::path("home/foo").is_absolute());
}

TEST(FileSystemGroup, path_parent_path)
{
    ASSERT_STREQ(fs::path("/home").c_str(), fs::path("/home/foo").parent_path().c_str());
    ASSERT_STREQ(fs::path("/home/foo").c_str(), fs::path("/home/foo/bar.txt").parent_path().c_str());
    ASSERT_STREQ(fs::path("home/foo").c_str(), fs::path("home/foo/bar.txt").parent_path().c_str());
}

TEST(FileSystemGroup, path_concat)
{
    fs::path p1("/home/foo/bar");
    fs::path p2(".txt");
    p1 += p2;
    ASSERT_STREQ("/home/foo/bar.txt", p1.c_str());
}

TEST(FileSystemGroup, path_native)
{
#ifndef _WIN32
    std::string first = "/foo";
#else
    std::string first = "\\foo";
#endif
    fs::path p("/foo");
    ASSERT_STREQ(p.string_native().c_str(), first.c_str());
}

TEST(FileSystemGroup, path_unix)
{
    fs::path p("C:\\foo\\bar");
    std::string first = "C:/foo/bar";
    ASSERT_STREQ(p.string_unix().c_str(), first.c_str());
}

TEST(FileSystemGroup, remove)
{
    // file remove
    fs::path test_file_path = fs::path{UT::unitTestsDataDir} / fs::path{"test_file.txt"};
    {
        std::ofstream test_file_ofstr{test_file_path.string()};
        test_file_ofstr << "blabla";
        test_file_ofstr.close();
    }
    ASSERT_TRUE(fs::exists(test_file_path));

    ASSERT_TRUE(fs::remove(test_file_path));
    ASSERT_FALSE(fs::exists(test_file_path));
    ASSERT_FALSE(fs::remove(test_file_path));

    // empty dir remove
    fs::path test_dir_path = fs::path{UT::unitTestsDataDir} / fs::path{"test_dir"};
    ASSERT_EQ(0, g_mkdir_with_parents(test_dir_path.c_str(), 0777));
    ASSERT_TRUE(fs::exists(test_dir_path));
    ASSERT_TRUE(fs::remove(test_dir_path));
    ASSERT_FALSE(fs::exists(test_dir_path));
    ASSERT_FALSE(fs::remove(test_dir_path));

    // non empty dir remove
    ASSERT_EQ(0, g_mkdir_with_parents(test_dir_path.c_str(), 0777));
    fs::path test_file_in_dir = test_dir_path / fs::path{"test_file.txt"};
    {
        std::ofstream test_file_in_dir_ofstr{test_file_in_dir.string()};
        test_file_in_dir_ofstr << "blabla";
        test_file_in_dir_ofstr.close();
    }
    ASSERT_TRUE(fs::exists(test_file_in_dir));
    ASSERT_EQ(2, fs::remove_all(test_dir_path));
    ASSERT_FALSE(fs::exists(test_dir_path));
}

TEST(FileSystemGroup, relative)
{
#ifdef _WIN32
    ASSERT_STREQ("test.txt", fs::relative("C:\\tmp\\test.txt", "C:\\tmp").c_str());
    ASSERT_STREQ("tmp\\test.txt", fs::relative("C:\\tmp\\test.txt", "C:\\").c_str());
    ASSERT_STREQ("..\\test.txt", fs::relative("C:\\tmp\\test.txt", "C:\\tmp\\one").c_str());
    ASSERT_STREQ("..\\..\\test.txt", fs::relative("C:\\tmp\\test.txt", "C:\\tmp\\one\\two").c_str());
    ASSERT_STREQ("C:\\tmp\\test.txt", fs::relative("C:\\tmp\\test.txt", "D:\\tmp").c_str());
#else
    ASSERT_STREQ("test.txt", fs::relative("/tmp/test.txt", "/tmp").c_str());
    ASSERT_STREQ("tmp/test.txt", fs::relative("/tmp/test.txt", "/").c_str());
    ASSERT_STREQ("../test.txt", fs::relative("/tmp/test.txt", "/tmp/one").c_str());
    ASSERT_STREQ("../../test.txt", fs::relative("/tmp/test.txt", "/tmp/one/two").c_str());
#endif // _WIN32
}
