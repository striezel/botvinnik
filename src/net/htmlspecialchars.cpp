/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2021  Dirk Stolle

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

#include "htmlspecialchars.hpp"
#include <array>
#include <utility>

namespace bvn
{

std::string htmlspecialchars(const std::string_view& str)
{
  const std::array<std::pair<char, std::string>, 4> chars = {
    std::make_pair('&', "&amp;"),
    std::make_pair('<', "&lt;"),
    std::make_pair('>', "&gt;"),
    std::make_pair('"', "&quot;")
  };

  std::string s(str);

  for (const auto& p: chars)
  {
    std::string::size_type pos = s.find(p.first);
    while (pos != std::string::npos)
    {
      s.replace(pos, 1, p.second);
      pos = s.find(p.first, pos + 3);
    }
  }

  return s;
}

}
