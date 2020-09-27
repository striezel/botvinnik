/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
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

#include "FailCounter.hpp"

namespace bvn
{

FailCounter::FailCounter(const uint_fast8_t limit)
: status(0), offset(0), limit_(limit < N ? limit : N - 1)
{
}

void FailCounter::next(const bool success)
{
  status.set(offset, !success);
  offset = (offset + 1) % N;
}

unsigned int FailCounter::count() const
{
  return status.count();
}

unsigned int FailCounter::limit() const
{
  return limit_;
}

bool FailCounter::limitExceeded() const
{
  return status.count() > limit_;
}

} // namespace
