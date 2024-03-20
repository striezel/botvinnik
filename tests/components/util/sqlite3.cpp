/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2024  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#include <filesystem>
#include "../../locate_catch.hpp"
#include "../../FileGuard.hpp"
#include "../../../src/util/sqlite3.hpp"

TEST_CASE("bvn::sql")
{
  using namespace bvn::sql;
  namespace fs = std::filesystem;

  SECTION("open: success")
  {
    const fs::path path{ fs::temp_directory_path() / "bvn_sql_open.db"};
    const FileGuard guard{path};
    {
      const auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
    }
  }

  SECTION("open: failure")
  {
    const fs::path path{ fs::temp_directory_path() / "does" / "not" / "exist" / "open.db"};
    const FileGuard guard{path};
    {
      const auto db = open(path.string());
      REQUIRE( db.get() == nullptr );
    }
  }

  SECTION("exec: success")
  {
    const fs::path path{ fs::temp_directory_path() / "bvn_sql_exec.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      REQUIRE( exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);") );
    }
  }

  SECTION("exec: failure")
  {
    const fs::path path{ fs::temp_directory_path() / "bvn_sql_exec_fail.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      // Syntax error in query, and table does not exist, so it should fail.
      REQUIRE_FALSE( exec(db, "SEL ECT foo FROM bar;") );
    }
  }

  SECTION("quote")
  {
    REQUIRE( quote("") == "''" );
    REQUIRE( quote(" ") == "' '" );
    REQUIRE( quote("abc\ndef") == "'abc\ndef'" );
    REQUIRE( quote("abcdefghijklmnopqrstuvwxyz") == "'abcdefghijklmnopqrstuvwxyz'" );
    REQUIRE( quote("012 3456 89") == "'012 3456 89'" );
    REQUIRE( quote("2012-10-31") == "'2012-10-31'" );
    REQUIRE( quote("-123.45") == "'-123.45'" );

    REQUIRE( quote("'") == "''''" );
    REQUIRE( quote("foo'bar") == "'foo''bar'" );
    REQUIRE( quote("'bar") == "'''bar'" );
    REQUIRE( quote("foo'") == "'foo'''" );
    REQUIRE( quote("foo''bar") == "'foo''''bar'" );
    REQUIRE( quote("foo'''bar") == "'foo''''''bar'" );
  }

  SECTION("prepare: success")
  {
    const fs::path path{ fs::temp_directory_path() / "bvn_sql_prepare.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);");
      const auto stmt = prepare(db, "SELECT c1, c2 FROM t1 WHERE c2 = :p1;");
      REQUIRE( stmt.get() != nullptr );
    }
  }

  SECTION("prepare: failure")
  {
    const fs::path path{ fs::temp_directory_path() / "bvn_sql_prepare_fail.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);");
      const auto stmt = prepare(db, "FAIL SELECT c1, c2 FROM t1 WHERE c2 = :p1;");
      REQUIRE( stmt.get() == nullptr );
    }
  }

  SECTION("bind(string): success")
  {
    const fs::path path{ fs::temp_directory_path() / "sql_bind_string.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);");
      auto stmt = prepare(db, "SELECT c1, c2 FROM t1 WHERE c2 = :p1;");
      REQUIRE( stmt.get() != nullptr );
      REQUIRE( bvn::sql::bind(stmt, 1, "foo") );
    }
  }

  SECTION("bind(string): failure")
  {
    const fs::path path{ fs::temp_directory_path() / "sql_bind_string_fail.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);");
      auto stmt = prepare(db, "SELECT c1, c2 FROM t1 WHERE c2 = :p1;");
      REQUIRE( stmt.get() != nullptr );
      REQUIRE_FALSE( bvn::sql::bind(stmt, 42, "foo") );
    }
  }

  SECTION("bind(int64): success")
  {
    const fs::path path{ fs::temp_directory_path() / "sql_bind_int64.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);");
      auto stmt = prepare(db, "SELECT c1, c2 FROM t1 WHERE c1 = :p1;");
      REQUIRE( stmt.get() != nullptr );
      REQUIRE( bvn::sql::bind(stmt, 1, 12345) );
    }
  }

  SECTION("bind(int64): failure")
  {
    const fs::path path{ fs::temp_directory_path() / "sql_bind_int64_fail.db"};
    const FileGuard guard{path};
    {
      auto db = open(path.string());
      REQUIRE( db.get() != nullptr );
      exec(db, "CREATE TABLE t1 (c1 INT, c2 TEXT);");
      auto stmt = prepare(db, "SELECT c1, c2 FROM t1 WHERE c1 = :p1;");
      REQUIRE( stmt.get() != nullptr );
      REQUIRE_FALSE( bvn::sql::bind(stmt, 5, 12345) );
    }
  }
}
