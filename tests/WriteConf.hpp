/*
 -------------------------------------------------------------------------------
    This file is part of the test suite for botvinnik.
    Copyright (C) 2022, 2024  Dirk Stolle

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

#ifndef BVN_TEST_WRITE_CONF_HPP
#define BVN_TEST_WRITE_CONF_HPP

#include <fstream>

// utility function to write a configuration file for testing
bool writeConfiguration(const std::filesystem::path& path, const std::string& content)
{
  std::ofstream stream(path, std::ios::out | std::ios::binary);
  if (!stream.good())
    return false;
  if (!stream.write(content.c_str(), content.size()).good())
    return false;
  stream.close();
  return stream.good();
}

#endif // BVN_TEST_WRITE_CONF_HPP
