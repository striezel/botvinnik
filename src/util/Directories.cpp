/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2018, 2020, 2021  Dirk Stolle

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

#include "Directories.hpp"
#include <cstring> // for std::memset()
#if defined(_WIN32)
  #include <Windows.h>
  #include <Shlobj.h> // for SHGetFolderPathA()
#elif defined(__linux__) || defined(linux)
  #include <unistd.h> // for sysconf() and getuid()
  #include <pwd.h> // for getpwuid_r()
#endif

namespace bvn
{

namespace filesystem
{

bool getHome(std::string& result)
{
  #if defined(_WIN32)
    char buffer[MAX_PATH + 1];
    std::memset(buffer, 0, MAX_PATH + 1);
    if (SHGetFolderPathA(0, CSIDL_PROFILE, NULL, 0, buffer) != S_OK)
    {
      return false;
    }
    result = std::string(buffer);
    return true;
  #elif defined(__linux__) || defined(linux)
    const long int buf_size = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (buf_size <= -1)
      return false; // -1 means: sysconf() error / EINVAL
    char * buffer = new char[buf_size];
    std::memset(buffer, 0, buf_size);
    struct passwd info;
    struct passwd * pwd_ptr = nullptr;
    const int error = getpwuid_r(getuid(), &info, buffer, buf_size, &pwd_ptr);
    // getpwuid_r() failed, if return value is non-zero. However, it can still
    // return zero, if no matching entry is found. In that case, the pointer is
    // set to null.
    if ((error != 0) || (pwd_ptr == NULL))
    {
      // getpwuid_r() failed or found no matching entry.
      delete [] buffer;
      return false;
    }
    // success
    result = std::string(info.pw_dir);
    delete [] buffer;
    return true;
  #else
    #error Unknown operating system!
  #endif
}

} // namespace

} // namespace
