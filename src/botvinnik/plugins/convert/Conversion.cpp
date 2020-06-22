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

#include "Conversion.hpp"
#include "ConvertBases.hpp"
#include "../../../util/Strings.hpp"

namespace bvn
{

Conversion::Conversion()
{
}

std::vector<std::string> Conversion::commands() const
{
  return { "bin2dec", "bin2hex", "dec2bin", "dec2hex", "hex2bin", "hex2dec" };
}

Message Conversion::handleCommand(const std::string_view& command, const std::string_view& message, const std::string_view& userId, const std::chrono::milliseconds& server_ts)
{
  if (message.size() <= command.size() + 1)
  {
    return Message(std::string(userId)
        .append(": You have to give a number after the '")
        .append(command).append("' command."));
  }

  if (command == "bin2dec")
  {
    return convert(command, message, 2, 10);
  }
  if (command == "bin2hex")
  {
    return convert(command, message, 2, 16);
  }
  if (command == "dec2bin")
  {
    return convert(command, message, 10, 2);
  }
  if (command == "dec2hex")
  {
    return convert(command, message, 10, 16);
  }
  if (command == "hex2bin")
  {
    return convert(command, message, 16, 2);
  }
  if (command == "hex2dec")
  {
    return convert(command, message, 16, 10);
  }

  // unknown command
  return Message();
}

std::string Conversion::helpOneLine(const std::string_view& command) const
{
  if (command == "bin2dec")
  {
    return "converts a binary number to a decimal number";
  }
  if (command == "bin2hex")
  {
    return "converts a binary number to a hexadecimal number";
  }
  if (command == "dec2bin")
  {
    return "converts a decimal number to a binary number";
  }
  if (command == "dec2hex")
  {
    return "converts a decimal number to a hexadecimal number";
  }
  if (command == "hex2bin")
  {
    return "converts a hexadecimal number to a binary number";
  }
  if (command == "hex2dec")
  {
    return "converts a hexadecimal number to a decimal number";
  }

  return std::string();
}

Message Conversion::convert(const std::string_view& command, const std::string_view& message, const unsigned int origBase, const unsigned int newBase)
{
  std::string number = std::string(message.substr(command.size() + 1));
  trim(number);
  if (number.empty())
  {
    return Message(std::string("No number given for command ").append(command).append("!"));
  }

  std::size_t pos;
  unsigned long long value;
  try
  {
    value = std::stoull(number, &pos, origBase);
  }
  catch (const std::exception& ex)
  {
    return Message("Value '" + number + "' could not be converted!");
  }
  if (pos != number.size())
  {
    return Message(std::string("'" + number + "' is not a valid number for ")
                  .append(command).append("!"));
  }
  return Message(ConvertBases<unsigned long long, unsigned int>::get(value, newBase));
}

} // namespace
