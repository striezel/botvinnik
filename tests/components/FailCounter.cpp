/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2020  Dirk Stolle

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

#include "../locate_catch.hpp"
#include "../../src/botvinnik/FailCounter.hpp"

TEST_CASE("FailCounter constructor")
{
  using namespace bvn;

  SECTION("limit must be set")
  {
    FailCounter counter(22);
    REQUIRE( 22 == counter.limit() );
    REQUIRE( 0 == counter.count() );
  }

  SECTION("high limit must be reduced")
  {
    FailCounter counter(counter.N + 1);
    REQUIRE( counter.limit() == counter.N - 1 );
  }

  SECTION("high limit N must be reduced")
  {
    FailCounter counter(counter.N);
    REQUIRE( counter.limit() == counter.N - 1 );
  }
}

TEST_CASE("FailCounter::count()")
{
  using namespace bvn;

  SECTION("simple count sequence")
  {
    FailCounter counter(12);

    REQUIRE( counter.count() == 0 );

    for (int i = 0; i < 5; ++i)
    {
      counter.next(false);
    }

    // Count must be five, because there were five failures.
    REQUIRE( counter.count() == 5 );

    for (int i = 0; i < 5; ++i)
    {
      counter.next(true);
    }

    // Count is still five.
    REQUIRE( counter.count() == 5 );

    for (unsigned int i = 0; i < counter.N - 10; ++i)
    {
      counter.next(true);
    }

    // Count is still five.
    REQUIRE( counter.count() == 5 );
  }

  SECTION("alternating fail and success")
  {
    FailCounter counter(12);

    REQUIRE( counter.count() == 0 );

    for (unsigned int i = 0; i < counter.N / 2; ++i)
    {
      counter.next(true);
      counter.next(false);
      REQUIRE( counter.count() == i + 1 );
    }
  }

  SECTION("bitset is used ring buffer-like")
  {
    FailCounter counter(12);

    for (unsigned int i = 0; i < counter.N; ++i)
    {
      counter.next(false);
    }

    REQUIRE( counter.count() == counter.N );

    for (int i = counter.N - 1; i >= 0; --i)
    {
      counter.next(true);
      // count gets reduced by one on every iteration.
      REQUIRE( counter.count() == i );
    }
  }
}

TEST_CASE("FailCounter::limit()")
{
  using namespace bvn;

  SECTION("limit must be set")
  {
    for (unsigned int i = 0; i < FailCounter::N; ++i)
    {
      FailCounter counter(i);
      REQUIRE( counter.limit() == i );
    }
  }
}

TEST_CASE("FailCounter::limitExceeded()")
{
  using namespace bvn;

  SECTION("limit must never be exceeded when all succeeds")
  {
    FailCounter counter(0);

    for (unsigned int i = 0; i < FailCounter::N + 5; ++i)
    {
      counter.next(true);
      REQUIRE_FALSE( counter.limitExceeded() );
    }
  }

  SECTION("limit must not be exceeded when count == limit")
  {
    FailCounter counter(5);

    for (unsigned int i = 0; i < 5; ++i)
    {
      counter.next(false);
    }
    REQUIRE_FALSE( counter.limitExceeded() );
  }

  SECTION("limit must not exceeded when count > limit")
  {
    FailCounter counter(5);

    for (unsigned int i = 0; i < 5; ++i)
    {
      counter.next(false);
    }

    // Six exceeds five, so limit is exceeded.
    counter.next(false);
    REQUIRE( counter.limitExceeded() );

    for (unsigned int i = 0; i < 10; ++i)
    {
      // Further failures also exceed the limit.
      counter.next(false);
      REQUIRE( counter.limitExceeded() );
    }
  }

  SECTION("limit must not be exceeded when there are enough successes between failures")
  {
    FailCounter counter(5);

    for (unsigned int i = 0; i < 5; ++i)
    {
      counter.next(false);
    }
    REQUIRE_FALSE( counter.limitExceeded() );

    for (unsigned int i = 0; i < counter.N; ++i)
    {
      counter.next(true);
    }
    REQUIRE_FALSE( counter.limitExceeded() );

    // Five new failures should not exceed the limit.
    for (unsigned int i = 0; i < 5; ++i)
    {
      counter.next(false);
    }
    REQUIRE_FALSE( counter.limitExceeded() );
  }

}
