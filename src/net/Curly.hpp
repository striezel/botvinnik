/*
 -------------------------------------------------------------------------------
    This file is part of scan-tool.
    Copyright (C) 2015, 2016, 2017  Dirk Stolle

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

#ifndef SCANTOOL_CURLY_HPP
#define SCANTOOL_CURLY_HPP

#include <string>
#include <unordered_map>
#include <vector>

extern "C"
{
  /** \brief write callback for cURL functions
   */
  size_t writeCallbackString(char *ptr, size_t size, size_t nmemb, void *userdata);
} //extern C

class Curly
{
  public:
    ///default constructor
    Curly();

    /// delete copy constructor
    Curly(const Curly& other) = delete;


    /** \brief sets the new URL for the operation
     *
     * \param newURL   the new URL
     */
    void setURL(const std::string& newURL);


    /** \brief gets the current URL
     *
     * \return Returns the current URL.
     * Returns an empty string, if no URL was set yet.
     */
    const std::string& getURL() const;


    /** \brief adds a field for POST request
     *
     * \param name   name of the field
     * \param value  the field's value
     * \return Returns true, if a field with the given name was added.
     *         Returns false otherwise.
     * \remarks If a POST field with the same name already exists, its value
     *          will be overwritten.
     */
    bool addPostField(const std::string& name, const std::string& value);


    /** \brief checks, if this class instance has a POST field with the given name
     *
     * \param name   name of the field
     * \return Returns true, if a field with the given name exists.
     *         Returns false otherwise.
     */
    bool hasPostField(const std::string& name) const;


    /** \brief returns the value of a post field, if it exists
     *
     * \param name  name of the field
     * \return Returns the value of the field, if it exists.
     *         Returns empty string, if the field does not exist.
     * \remarks Note that an empty string can also be a proper return value
     * for an existing field. If you want to check the existence of a certain
     * field, use hasPostField().
     */
    std::string getPostField(const std::string& name) const;


    /** \brief removes a POST field that was previously set
     *
     * \param name   name of the field
     * \return Returns true, if a field with the given name existed and was
     * removed. Returns false, if no such field was present.
     */
    bool removePostField(const std::string& name);


    /** \brief adds a file to the request
     *
     * \param filename  name of the local file to use in the post request
     * \param field     name of the form field that contains the file
     * \return Returns true, if a file was appended.
     * Returns false, if operation failed.
     */
    bool addFile(const std::string& filename, const std::string& field);


    /** \brief gets the list of additional / custom HTTP headers
     *
     * \return Returns a vector of strings that represents the headers.
     */
    const std::vector<std::string>& getHeaders() const;


    /** \brief adds a custom HTTP header
     *
     * \param header   the header, without(!) CRLF at the end
     * \return Returns true, if the header was added.
     *         Returns false otherwise.
     */
    bool addHeader(const std::string& header);


    /** \brief define the body of a HTTP POST request directly
     *
     * \param body  the content of the HTTP body
     * \remarks This will only work, if no post fields have been set via the
     * addPostField() method and if no files were added with addFile().
     * Otherwise the fields and files from these functions will be used to
     * create the body of the HTTP post request.
     * \return Returns true, if the post body was added.
     *         Returns false, if the post body could not be added.
     */
    bool setPostBody(const std::string& body);


    /** \brief sets the path to a certificate file to verify the peer with
     *
     * \param certFile  path to the file holding one or more certificates
     * \return Returns true, if the path could be set.
     * Returns false, if the operation failed.
     */
    bool setCertificateFile(const std::string& certFile);


    /** \brief limits the speed of an upload operation
     *
     * \param maxBytesPerSecond  limit in bytes per second
     * \remarks A value of zero means no limit. (That is the default.)
     * \return Returns true, if the limit could be set.
     *         Returns false otherwise.
     */
    bool limitUpstreamSpeed(const unsigned int maxBytesPerSecond);


    /** \brief checks whether Curly will follow redirects
     *
     * \return Returns true, if Curly's perform() method will follow redirects.
     * Returns false, if redirects are ignored.
     * \remarks Default behaviour is not to follow redirects.
     * You can limit the maximum number of redirects which are followed with
     * setMaximumRedirects() function.
     */
    bool followsRedirects() const;


    /** \brief changes whether Curly::perform() shall follow redirects.
     *
     * \param follow  Set this to true, if Curly shall follow redirects.
     *                False (default) means that redirects are ignored.
     * \remarks Default behaviour is not to follow redirects.
     * You can limit the maximum number of redirects which are followed with
     * setMaximumRedirects() function.
     */
    void followRedirects(const bool follow);


    /** \brief gets the maximum number of redirects that Curly will follow
     *
     * \return Returns the maximum number of redirects that Curly will follow.
     * \remarks This setting only takes effect, if followsRedirects() returns
     *          true. Default value is -1, which means that there is no limit
     *          on the number of redirects.
     */
    long int maximumRedirects() const;


    /** \brief Set the maximum number of redirects that Curly will follow.
     *
     * \param maxRedirect  the new maximum number of redirects
     * \remarks This setting only takes effect, if followsRedirects() returns
     *          true. Negative values are interpreted as "follow (up to) an
     *          indefinite number of redirects". Preset value is -1.
     */
    void setMaximumRedirects(const long int maxRedirect);


    /** \brief performs the (POST) request
     *
     * \param response  reference to a string that will be filled with the
     *                  request's response
     * \return Returns true, if the request could be performed.
     *         Returns false, if the request was not performed properly.
     *         Note that the value of @arg response is undefined, if the
     *         request failed.
     */
    bool perform(std::string& response);


    /** \brief returns the response code of the last request, or zero (0)
     *
     * \return Returns the response code of the last request.
     *         Returns zero, if no response code was set (e.g. when the
     *         protocol does not provide such a code) or no request was
     *         performed yet.
     */
    long getResponseCode() const;


    /** \brief returns the content type of the last request, or an empty string
     *
     * \return Returns the content type of the last request.
     *         Returns an empty string, if no content type header was received
     *         or no request was performed yet.
     */
    const std::string& getContentType() const;


    /** \brief structure to hold version information about the underlying cURL
     *         library
     */
    struct VersionData
    {
      /** default constructor */
      VersionData();

      std::string cURL;  /**< cURL version */
      std::string ssl;   /**< OpenSSL version */
      std::string libz;  /**< zlib version */
      std::vector<std::string> protocols; /**< supported protocols */
      std::string ares;  /**< ares version (only for newer cURL versions) */
      std::string idn;   /**< idn version (only for newer cURL versions) */
      std::string ssh;   /**< libssh version (only for newer cURL versions) */
    }; //struct

    /** \brief gets version information about the underlying cURL library
     *
     * \return Returns a version string for the underlying cURL library.
     * \remarks The result might contain some empty strings, if the used cURL
     *          library is too old to provide version information about itself.
     */
    static VersionData curlVersion();


    /** \brief gets the list of header lines that were returned by the request
     *
     * \return Returns a vector of strings, one string for each header line.
     * \remarks This list will only be filled, after perform() was called and
     *          was successful. Otherwise, the list will be empty or contain
     *          header data from the previous request.
     *
     *          Do not mix this up with the headers that were sent (by using
     *          the addHeader() function), these are the response's headers.
     */
    const std::vector<std::string>& responseHeaders() const;
  private:
    /** \brief callback for response headers
     *
     * \param buffer   data of header (might not be NUL-terminated)
     * \param size     size of an item
     * \param nitems   number of items
     * \param userdata pointer to user data (if any)
     * \return Returns the size of the read data.
     */
    static size_t headerCallback(char* buffer, size_t size, size_t nitems, void* userdata);

    /** \brief adds a new header to the list of response headers
     *
     * \param respHeader   the new header line
     */
    void addResponseHeader(std::string respHeader);

    std::string m_URL; /**< URL for the request */
    std::unordered_map<std::string, std::string> m_PostFields; /**< post fields; key = name; value = field's value */
    std::unordered_map<std::string, std::string> m_Files; /**< added files; key = field name, value = file name */
    std::vector<std::string> m_headers; /**< additional / custom headers (HTTP only) */
    std::string m_PostBody; /**< plain post body */
    bool m_UsePostBody; /**< whether to use the explicit post body */
    std::string m_certFile; /**< the path to the certificate file to verify the peer with */
    long m_LastResponseCode; /**< response code of the last request */
    std::string m_LastContentType; /**< string that holds the last content type */
    bool m_followRedirects; /**< whether to follow redirects */
    long int m_maxRedirects; /**< maximum number of redirects that Curly will follow */
    std::vector<std::string> m_ResponseHeaders; /**< response headers returned by the last request */
    unsigned int m_MaxUpstreamSpeed; /**< limit for upstream / upload in bytes per second */
}; //class Curly

#endif // SCANTOOL_CURLY_HPP
