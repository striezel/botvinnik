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

#ifndef BVN_MATRIX_HPP
#define BVN_MATRIX_HPP

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>
#include "../conf/Configuration.hpp"

namespace bvn
{

/** Handles communication with Matrix server.
 */
class Matrix
{
  public:
    /** \brief Constructor.
     *
     * \param conf  complete configuration data
     */
    Matrix(const Configuration& conf);


    /** \brief Destructor.
     */
    ~Matrix();


    /** \brief Performs login on the Matrix server.
     *
     * \return Returns whether login was successful.
     */
    bool login();


    /** \brief Performs logout on the Matrix server.
     *
     * \return Returns whether logout was successful.
     */
    bool logout();


    /** \brief Gets a list of the user's current rooms.
     *
     * \param roomIds  vector that will be used to store ids of all joined rooms.
     * \return Returns true, if list retrieval was successful.
     *         Returns false otherwise.
     */
    bool joinedRooms(std::vector<std::string>& roomIds);


    /** \brief Gets the name of a room.
     *
     * \param roomId   id of the room
     * \param name     string that will be used to store the name
     * \return Returns true, if name retrieval was successful.
     *         Returns false otherwise.
     */
    bool roomName(const std::string& roomId, std::string& name);


    /** \brief Gets room events for the user.
     *
     * \param events      string that will be used to store the events as JSON (if any)
     * \param nextBatch   string that will be used to store the "next_batch" (if any)
     * \param since       if set, the request will only retrieve the data since that batch
     * \return Returns true, if the request succeeded.
     *         Returns false otherwise.
     */
    bool sync(std::string& events, std::string& nextBatch, const std::string& since = "");


    /** \brief Sends a message to a given room.
     *
     * \param roomId   id of the room to send the message in
     * \param message  the actual message text
     * \return Returns true, if the message was sent successfully.
     *         Returns false otherwise.
     */
    bool sendMessage(const std::string& roomId, const std::string& message);
  private:
    /** \brief Determines whether the user is logged in to Matrix.
     *
     * \return Returns true, if user is logged in. Returns false otherwise.
     */
    bool isLoggedIn() const;

    /** \brief Encodes a room id for use in URLs.
     *
     * \param roomId   the id of the room
     * \return Returns the URL-encoded room id.
     * \remarks This method is not safe for encoding other content than room ids.
     */
    std::string encodeRoomId(const std::string& roomId);

    Configuration conf;
    std::string accessToken; /**< the access token for Matrix */
    std::atomic<uint_least32_t> transactionId;/**< id of the transaction */
}; // class

} // namespace

#endif // BVN_MATRIX_HPP
