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

#include "Basic.hpp"
#include "../../../util/GitInfos.hpp"
#include "../../../Version.hpp"

namespace bvn
{

Basic::Basic(Bot& b)
: theBot(b)
{
}

std::vector<std::string> Basic::commands() const
{
  return { "stop", "version" };
}

std::string Basic::handleCommand(const std::string_view& command, const std::string_view& message)
{
  if (command == "stop")
  {
    theBot.stop();
    return "Stop of bot was requested. Shutdown will be initiated.";
  }
  else if (command == "version")
  {
    GitInfos info;
    return  "botvinnik, " + bvn::version + "\n"
            + "\n"
            + "Version control commit: " + info.commit() + "\n"
            + "Version control date:   " + info.date();
  }
  else
    // unknown command
    return "";
}

} // namespace
