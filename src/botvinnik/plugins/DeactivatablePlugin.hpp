/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2022  Dirk Stolle

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

#ifndef BVN_PLUGIN_DEACTIVATABLEPLUGIN_HPP
#define BVN_PLUGIN_DEACTIVATABLEPLUGIN_HPP

#include "Plugin.hpp"

namespace bvn
{

/** \brief Abstract plugin that allows command deactivation of all commands.
 */
class DeactivatablePlugin: public Plugin
{
  public:
    /** \brief Determines whether a command of the plugin can be deactivated.
     *
     * \param command  name of the command to deactivate
     * \return Returns true, if the command can be deactivated.
     *         Returns false, if deactivation is forbidden.
     */
    bool allowDeactivation(const std::string_view& command) const override;
}; // class

} // namespace

#endif // BVN_PLUGIN_DEACTIVATABLEPLUGIN_HPP
