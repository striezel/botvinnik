/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2020, 2021, 2022  Dirk Stolle

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
#include <optional>
#include <string>
#include <vector>
#include "../conf/Configuration.hpp"
#include "Message.hpp"
#include "Room.hpp"
#include "events/PowerLevels.hpp"

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


    /** \brief Gets the current configuration.
     *
     * \return Returns the configuration.
     */
    const Configuration& configuration() const;


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
     * \return Returns an optional containing the name, if name retrieval was successful.
     *         Note that this may also contain an empty string, if no name is set.
     *         Returns an empty optional, if the retrieval failed.
     */
    std::optional<std::string> roomName(const std::string& roomId);


    /** \brief Joins the room with the given room id.
     *
     * \param roomId    id of the room to join
     * \return Returns true, if the room was joined.
     *         Returns false otherwise.
     */
    bool joinRoom(const std::string& roomId);


    /** \brief Leaves the room with the given room id.
     *
     * \param roomId    id of the room to leave
     * \return Returns true, if the room was left.
     *         Returns false otherwise.
     */
    bool leaveRoom(const std::string& roomId);


    /** \brief Forgets the room with the given room id.
     *
     * Matrix specification states that joined rooms cannot be forgotten.
     * Only rooms that have been left (via leaveRoom()) can be forgotten.
     * \param roomId    id of the room to forget
     * \return Returns true, if the room was forgotten.
     *         Returns false otherwise.
     */
    bool forgetRoom(const std::string& roomId);


    /** \brief Gets the power levels of the room with the given room id.
     *
     * \param roomId   id of the for which to get the power levels
     * \return Returns an optional containing the rooms power levels.
     *         Returns an empty optional, if the request failed.
     */
    std::optional<matrix::PowerLevels> powerLevels(const std::string& roomId);


    /** \brief Gets room events for the user.
     *
     * \param nextBatch   string that will be used to store the "next_batch" (if any)
     * \param rooms       events of Matrix rooms
     * \param invites     vector to store ids of room where user has been invited to join
     * \param since       if set, the request will only retrieve the data since that batch
     * \return Returns true, if the request succeeded.
     *         Returns false otherwise.
     */
    bool sync(std::string& nextBatch, std::vector<matrix::Room>& rooms, std::vector<std::string>& invites, const std::string& since = "");


    /** \brief Sends a message to a given room.
     *
     * \param roomId   id of the room to send the message in
     * \param message  the actual message
     * \return Returns true, if the message was sent successfully.
     *         Returns false otherwise.
     */
    bool sendMessage(const std::string& roomId, const Message& message);


    /** \brief Gets the upload size limit for the server's content repository.
     *
     * \return Returns the maximum upload size in bytes.
     *         If the request failed, the returned optional will be empty.
     *         If the server did not disclose its limit, the returned value is
     *         negative one (-1).
     */
    std::optional<int64_t> getUploadLimit();


    /** \brief Gets the version of the Synapse homeserver.
     *
     * \return Returns the version of the Synapse homeserver.
     *         If the request failed, the returned optional will be empty.
     * \remark This will probably not work with alternative server
     *         implementations, such as dendrite or Conduit.
     */
    std::optional<std::string> getSynapseVersion();


    /** \brief Uploads string contents as file to the content repository.
     *
     * \param data        the file data to be uploaded as string
     * \param contentType content type of the file (e. g.  "image/jpeg")
     * \param fileName    name of the file (e. g. "cat.jpeg")
     * \return Returns the Matrix Content URI (MXC URI) for the uploaded file.
     *         Returns an empty optional, if the upload failed.
     */
    std::optional<std::string> uploadString(const std::string& data, const std::string& contentType = "application/octet-stream", const std::string& fileName = "file.dat");


    /** \brief Uploads a web image to the matrix content repository.
       *
       * \param imgUrl   web URL of the image
       * \return Returns an optional containing the Matrix Content URI for the
       *         uploaded image.
       *         Returns an empty optional, if the operation failed.
       */
    std::optional<std::string> uploadImage(const std::string& imgUrl);


    /** \brief Gets the encryption algorithm used in a room.
     *
     * \param roomId  id of the room to get the algorithm for
     * \return Returns an optional containing the name of the algorithm, if the
     *         room uses encryption.
     *         Returns an optional containing an empty string, if the room is
     *         not encrypted.
     *         Returns an empty optional, if the request failed.
     */
    std::optional<std::string> encryptionAlgorithm(const std::string& roomId);
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

    /** \brief Joins, leaves or forgets the room with the given room id.
     *
     * Matrix specification states that joined rooms cannot be forgotten.
     * Only rooms that have been left (via leaveRoom()) can be forgotten.
     * \param roomId    id of the room to join, leave or forget
     * \param change    action to perform, must be one of "join", "leave" or "forget"
     * \return Returns true, if the room was joined / left / forgotten.
     *         Returns false otherwise.
     */
    bool roomMembershipChange(const std::string& roomId, const std::string& change);

    Configuration conf;
    std::string accessToken; /**< the access token for Matrix */
    std::atomic<uint_least32_t> transactionId;/**< id of the transaction */
}; // class

} // namespace

#endif // BVN_MATRIX_HPP
