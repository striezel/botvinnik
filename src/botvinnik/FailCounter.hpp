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

#ifndef BVN_FAILCOUNTER_HPP
#define BVN_FAILCOUNTER_HPP

#include <bitset>
#include <cstdint>

namespace bvn
{

/** \brief Counts failures of an operation.
 */
class FailCounter
{
  public:
    /** \brief number of operations that are kept before they are discarded
     */
    static constexpr unsigned int N = 32;


    /** \brief Constructor.
     *
     * \param limit   maximum number of allowed failures
     */
    FailCounter(const uint_fast8_t limit);


    /** \brief Indicates whether an operation has succeeded and sets internal counter.
     *
     * \param success  true, if operation was successful; false otherwise
     */
    void next(const bool success);


    /** \brief Gets the number of known failures.
     *
     * \return Returns the number of failures.
     */
    unsigned int count() const;


    /** \brief Gets the maximum number of allowed failures.
     *
     * \return Returns the maximum number of allowed failures.
     */
    unsigned int limit() const;


    /** \brief Determines whether there have been too much failures.
     *
     * \return Returns true, if the limit has been exceeded.
     *         Returns false otherwise.
     */
    bool limitExceeded() const;
  private:
    std::bitset<N> status;
    uint_fast8_t offset;
    uint_fast8_t limit_;
}; // class

} // namespace

#endif // BVN_FAILCOUNTER_HPP
