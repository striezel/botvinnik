/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2022, 2023  Dirk Stolle

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

std::vector<std::string> Conversion::commands() const
{
  return { "bin2dec", "bin2hex", "dec2bin", "dec2hex", "hex2bin", "hex2dec" };
}

Message Conversion::handleCommand(const std::string_view& command, const std::string_view& message,
                                  const std::string_view& userId,
                                  [[maybe_unused]] const std::string_view& roomId,
                                  [[maybe_unused]] const std::chrono::milliseconds& server_ts)
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

Message Conversion::helpExtended(const std::string_view& command,
                [[maybe_unused]] const std::string_view& prefix) const
{
  using namespace std::string_literals;

  if (command == "bin2dec")
  {
    return Message("converts a binary number to a decimal number. For example, `"s
                   .append(prefix) + "bin2dec 1101` will convert the binary "
                   + "number 1101 to its decimal representation, which is 13.",
                   "converts a binary number to a decimal number. For example, <code>"s
                   .append(prefix) + "bin2dec 1101</code> will convert the binary "
                   + "number 1101 to its decimal representation, which is 13.");
  }
  if (command == "bin2hex")
  {
    return Message("converts a binary number to a hexadecimal number. "
        + "For example, `"s .append(prefix) + "bin2hex 1101` will convert the "
        + "binary number 1101 to its hexadecimal representation.",
        "converts a binary number to a hexadecimal number. For example, <code>"s
        .append(prefix) + "bin2hex 1101</code> will convert the binary number "
        + "1101 to its hexadecimal representation, which is d<sub>(16)</sub>.");
  }
  if (command == "dec2bin")
  {
    return Message("converts a decimal number to a binary number. "
        + " For example, `"s .append(prefix) + "dec2bin 12` will convert the"
        + " decimal number 12 to its binary representation, which is 1100.",
        "converts a decimal number to a binary number. For example, <code>"s
        .append(prefix) + "dec2bin 12</code> will convert the decimal number"
        + " 12 to its binary representation, which is 1100<sub>(2)</sub>.");
  }
  if (command == "dec2hex")
  {
    return Message("converts a decimal number to a hexadecimal number. "
        + " For example, `"s .append(prefix) + "dec2hex 123` will convert the"
        + " decimal number 123 to its hexadecimal representation, which is 7b.",
        "converts a decimal number to a hexadecimal number. For example, <code>"s
        .append(prefix) + "dec2hex 123</code> will convert the decimal number"
        + " 123 to its hexadecimal representation, which is 7b<sub>(16)</sub>.");
  }
  if (command == "hex2bin")
  {
    return Message("converts a hexadecimal number to a binary number. "
        + " For example, `"s .append(prefix) + "hex2bin c2` will convert the"
        + " hexadecimal number c2 to its binary representation, which is "
        + "11000010.",
        "converts a hexadecimal number to a binary number. For example, <code>"s
        .append(prefix) + "hex2bin c2</code> will convert the hexadecimal "
        + "number c2 to its binary representation,"
        + " which is 11000010<sub>(2)</sub>.");
  }
  if (command == "hex2dec")
  {
    return Message("converts a hexadecimal number to a decimal number. "
        + " For example, `"s .append(prefix) + "hex2dec c2` will convert the"
        + " hexadecimal number c2 to its decimal representation, which is 194.",
        "converts a hexadecimal number to a decimal number. For example, <code>"s
        .append(prefix) + "hex2dec c2</code> will convert the hexadecimal "
        + "number c2 to its decimal representation, which is 194.");
  }

  return Message();
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
  const auto converted = ConvertBases<unsigned long long, unsigned int>::get(value, newBase);
  return Message(
    number + " (base " + std::to_string(origBase) + ") = "
           + converted + " (base " + std::to_string(newBase) + ")",
    number + "<sub>(" + std::to_string(origBase) + ")</sub> = "
           + converted + "<sub>(" + std::to_string(newBase) + ")</sub>");
}

} // namespace
