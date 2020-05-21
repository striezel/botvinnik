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

#include "Curly.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <curl/curl.h>

size_t writeCallbackString(char *ptr, size_t size, size_t nmemb, void *userdata)
{
  const size_t actualSize = size * nmemb;
  if (userdata == nullptr)
  {
    std::cerr << "Error: write callback received null pointer!" << std::endl;
    return 0;
  }

  const auto cBufferSize = actualSize+1;
  std::unique_ptr<char[]> tmpBuffer(new char[cBufferSize]);
  std::memcpy(tmpBuffer.get(), ptr, actualSize);
  tmpBuffer.get()[actualSize] = '\0';

  std::string data = std::string(tmpBuffer.get());
  while (data.size() < actualSize)
  {
    data.append(1, '\0');
    data += tmpBuffer.get()[data.size()];
  } //while

  std::string * theString = reinterpret_cast<std::string*>(userdata);
  theString->append(data);
  return actualSize;
}

#ifdef CURLY_READ_CALLBACK_STRING
struct StringData
{
  std::string::size_type dataOffset;
  std::string * data;
};

size_t readCallbackString(char *buffer, size_t size, size_t nitems, void *instream)
{
  const auto maxSize = size * nitems;
  if (maxSize < 1)
    return 0;
  if (nullptr == instream)
  {
    std::cerr << "Error: read callback received null pointer!" << std::endl;
    return CURL_READFUNC_ABORT;
  }
  //cast it to string data
  StringData * sd = reinterpret_cast<StringData*>(instream);
  if (nullptr == sd->data)
  {
    std::cerr << "Error: read callback received null pointer for string data!"
              << std::endl;
    return CURL_READFUNC_ABORT;
  }
  if (sd->dataOffset < 0)
  {
    std::cerr << "Error: read callback received invalid data offset!"
              << std::endl;
    return CURL_READFUNC_ABORT;
  }
  const auto totalLength = sd->data->length();
  if (sd->dataOffset >= totalLength)
    return 0;

  const auto chunkSize = std::min(maxSize, totalLength - sd->dataOffset);
  std::memcpy(buffer, &(sd->data[sd->dataOffset]), chunkSize);
  sd->dataOffset = sd->dataOffset + chunkSize;
  return chunkSize;
}
#endif // CURLY_READ_CALLBACK_STRING

Curly::Curly()
: m_URL(""),
  m_PostFields(std::unordered_map<std::string, std::string>()),
  m_Files(std::unordered_map<std::string, std::string>()),
  m_headers(std::vector<std::string>()),
  m_PostBody(""),
  m_UsePostBody(false),
  m_certFile(""),
  m_LastResponseCode(0),
  m_LastContentType(""),
  m_followRedirects(false),
  m_maxRedirects(-1),
  m_ResponseHeaders(std::vector<std::string>()),
  m_MaxUpstreamSpeed(0)
{
}

void Curly::setURL(const std::string& newURL)
{
  if (!newURL.empty())
    m_URL = newURL;
}

const std::string& Curly::getURL() const
{
  return m_URL;
}

bool Curly::addPostField(const std::string& name, const std::string& value)
{
  /*Perform some checks before adding the post field:
    No empty names, avoid conflict with file field names, and do not set it, if
    we already have a plain post body. */
  if (!name.empty() && (m_Files.find(name) == m_Files.end()) && !m_UsePostBody)
  {
    m_PostFields[name] = value;
    return true;
  }
  else
    return false;
}

bool Curly::hasPostField(const std::string& name) const
{
  return (m_PostFields.find(name) != m_PostFields.end());
}

std::string Curly::getPostField(const std::string& name) const
{
  const auto iter = m_PostFields.find(name);
  if (iter != m_PostFields.end())
    return iter->second;
  return std::string();
}

bool Curly::removePostField(const std::string& name)
{
  return (m_PostFields.erase(name) > 0);
}

bool Curly::addFile(const std::string& filename, const std::string& field)
{
  //No empty field or file names!
  if (field.empty() || filename.empty())
    return false;
  //Avoid name conflict with post fields.
  if (m_PostFields.find(field) != m_PostFields.end())
    return false;
  //Avoid conflict with post body.
  if (m_UsePostBody)
    return false;
  //Add file.
  m_Files[field] = filename;
  return true;
}

const std::vector<std::string>& Curly::getHeaders() const
{
  return m_headers;
}

bool Curly::addHeader(const std::string& header)
{
  /* There are some rules for reasonable headers:
     - No empty headers.
     - Header must not be present yet.
     - Header has to contain a colon (":"), but not as first character.
     - Header must not contain CRLF.
  */
  const auto colonPos = header.find(':');
  if (!header.empty()
      && (std::find(m_headers.cbegin(), m_headers.cend(), header) == m_headers.cend())
      && (colonPos != std::string::npos) && (colonPos > 0)
      && (header.find('\n') == std::string::npos)
      && (header.find('\r') == std::string::npos))
  {
    m_headers.push_back(header);
    return true;
  } //if conditions for header are met
  //Not a valid header!
  return false;
}

bool Curly::setPostBody(const std::string& body)
{
  //avoid conflicts with post fields and files
  if (m_PostFields.empty() && m_Files.empty())
  {
    m_PostBody = body;
    /* Checking for non-empty post body would be good enough for most cases, but
       it would not allow empty bodies. Therefore we need an extra flag for the
       post body to allow empty post bodies, too. */
    m_UsePostBody = true;
    return true;
  }
  else
    return false;
}

bool Curly::setCertificateFile(const std::string& certFile)
{
  //path should not be empty and should not contain NUL bytes
  if (!certFile.empty() && (certFile.find('\0') == std::string::npos))
  {
    m_certFile = certFile;
    return true;
  }
  return false;
}

bool Curly::limitUpstreamSpeed(const unsigned int maxBytesPerSecond)
{
  if (maxBytesPerSecond > std::numeric_limits<curl_off_t>::max())
    return false;
  m_MaxUpstreamSpeed = maxBytesPerSecond;
  return true;
}

bool Curly::followsRedirects() const
{
  return m_followRedirects;
}

void Curly::followRedirects(const bool follow)
{
  m_followRedirects = follow;
}

long int Curly::maximumRedirects() const
{
  return m_maxRedirects;
}

void Curly::setMaximumRedirects(const long int maxRedirect)
{
  if (maxRedirect >= 0)
    m_maxRedirects = maxRedirect;
  else
    m_maxRedirects = -1; //map all negative values to -1
}

bool Curly::perform(std::string& response)
{
  //"minimum" URL should be something like "http://a.bc"
  if (m_URL.size() < 11)
    return false;

  //initialize cURL
  #ifdef DEBUG_MODE
  std::clog << "curl_easy_init()..." << std::endl;
  #endif
  CURL * handle = curl_easy_init();
  if (nullptr == handle)
  {
    //cURL error
    std::cerr << "cURL easy init failed!" << std::endl;;
    return false;
  }

  //set URL
  #ifdef DEBUG_MODE
  std::clog << "curl_easy_setopt(..., URL, ...)..." << std::endl;
  #endif
  CURLcode retCode = curl_easy_setopt(handle, CURLOPT_URL, m_URL.c_str());
  if (retCode != CURLE_OK)
  {
    std::cerr << "cURL error: setting URL failed!" << std::endl;
    std::cerr << curl_easy_strerror(retCode) << std::endl;
    curl_easy_cleanup(handle);
    return false;
  }

  //set header read function
  #ifdef DEBUG_MODE
  std::clog << "curl_easy_setopt(..., CURLOPT_HEADERFUNCTION, ...)..." << std::endl;
  #endif
  retCode = curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, Curly::headerCallback);
  if (retCode != CURLE_OK)
  {
    std::cerr << "cURL error: setting header function failed!" << std::endl;
    std::cerr << curl_easy_strerror(retCode) << std::endl;
    curl_easy_cleanup(handle);
    return false;
  }
  //set header data
  retCode = curl_easy_setopt(handle, CURLOPT_HEADERDATA, this);
  if (retCode != CURLE_OK)
  {
    std::cerr << "cURL error: setting header data pointer failed!" << std::endl;
    std::cerr << curl_easy_strerror(retCode) << std::endl;
    curl_easy_cleanup(handle);
    return false;
  }

  //set certificate file
  if (!m_certFile.empty())
  {
    #ifdef DEBUG_MODE
    std::clog << "curl_easy_setopt(..., CURLOPT_CAINFO, ...)..." << std::endl;
    #endif
    retCode = curl_easy_setopt(handle, CURLOPT_CAINFO, m_certFile.c_str());
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting custom certificate file failed!" << std::endl;
      switch (retCode)
      {
        case CURLE_UNKNOWN_OPTION:
             std::cerr << "Option is not supported!" << std::endl;
             break;
        case CURLE_OUT_OF_MEMORY:
             std::cerr << "Insufficient heap memory!" << std::endl;
             break;
        default:
             //should not happen, according to libcurl documentation
             break;
      } //swi
      std::cerr << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      return false;
    }
  } //if cert file was set

  //set max. upload speed
  if (m_MaxUpstreamSpeed >= 512)
  {
    #ifdef DEBUG_MODE
    std::clog << "curl_easy_setopt(..., CURLOPT_MAX_SEND_SPEED_LARGE, ...)..." << std::endl;
    #endif
    retCode = curl_easy_setopt(handle, CURLOPT_MAX_SEND_SPEED_LARGE, static_cast<curl_off_t>(m_MaxUpstreamSpeed));
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: limiting the upload speed failed!" << std::endl;
      std::cerr << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      return false;
    }
  } //if upload speed limit is above 511 bytes per second

  //set redirection parameters
  if (followsRedirects())
  {
    //make cURL follow redirects
    retCode = curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting redirection mode failed!" << std::endl;
      std::cerr << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      return false;
    }
    //set limit - but only if we are not "limited" to infinite redirects
    if (maximumRedirects() >= 0)
    {
      retCode = curl_easy_setopt(handle, CURLOPT_MAXREDIRS, maximumRedirects());
      if (retCode != CURLE_OK)
      {
        std::cerr << "cURL error: setting redirection limit failed!" << std::endl;
        std::cerr << curl_easy_strerror(retCode) << std::endl;
        curl_easy_cleanup(handle);
        return false;
      } //if cURL error
    } //if redirect limit is given
  } //if redirects are followed

  //add custom headers
  struct curl_slist * header_list = nullptr;
  if (!m_headers.empty())
  {
    #ifdef DEBUG_MODE
    std::clog << "adding headers with curl_slist_append() ..." << std::endl;
    #endif // DEBUG_MODE
    for(auto const & h: m_headers)
    {
      header_list = curl_slist_append(header_list, h.c_str());
      if (nullptr == header_list)
      {
        std::cerr << "cURL error: creation of header list failed!" << std::endl;
        std::cerr << curl_easy_strerror(retCode) << std::endl;
        curl_easy_cleanup(handle);
        return false;
      }
    } //for
    //add headers to the handle
    #ifdef DEBUG_MODE
    std::clog << "curl_easy_setopt(handle, CURLOPT_HTTPHEADER, ...)" << std::endl;
    #endif // DEBUG_MODE
    retCode = curl_easy_setopt(handle, CURLOPT_HTTPHEADER, header_list);
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting custom headers failed!" << std::endl;
      std::cerr << curl_easy_strerror(retCode) << std::endl;
      curl_slist_free_all(header_list);
      header_list = nullptr;
      curl_easy_cleanup(handle);
      return false;
    }
  } //if custom headers are given

  //construct fields for post request
  #ifdef DEBUG_MODE
  std::clog << "curl_easy_escape(...)..." << std::endl;
  #endif
  std::string postfields("");
  if (m_Files.empty())
  {
    auto iter = m_PostFields.begin();
    while (iter != m_PostFields.end())
    {
      //escape key
      char * c_str = curl_easy_escape(handle, iter->first.c_str(), iter->first.length());
      if (c_str == nullptr)
      {
        //escaping failed!
        std::cerr << "cURL error: escaping of post values failed!" << std::endl;
        curl_easy_cleanup(handle);
        curl_slist_free_all(header_list);
        header_list = nullptr;
        return false;
      }
      if (!postfields.empty())
        postfields += "&"+std::string(c_str);
      else
        postfields += std::string(c_str);
      curl_free(c_str);
      //escape value
      c_str = curl_easy_escape(handle, iter->second.c_str(), iter->second.length());
      if (c_str == nullptr)
      {
        //escaping failed!
        std::cerr << "cURL error: escaping of post values failed!" << std::endl;
        curl_easy_cleanup(handle);
        curl_slist_free_all(header_list);
        header_list = nullptr;
        return false;
      }
      postfields += "=" + std::string(c_str);
      curl_free(c_str);
      //... and go on with next field
      ++iter;
    } //while
  } //no files

  // --set post fields
  if (!postfields.empty())
  {
    retCode = curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postfields.c_str());
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting POST fields for Curly::perform failed! Error: "
                << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      curl_slist_free_all(header_list);
      header_list = nullptr;
      return false;
    }
  } //if post fields exist

  //multipart/formdata
  struct curl_httppost* formFirst = nullptr;
  struct curl_httppost* formLast = nullptr;
  if (!m_Files.empty())
  {
    auto fileIter = m_Files.begin();
    while (fileIter != m_Files.end())
    {
      CURLFORMcode errCode = curl_formadd(&formFirst, &formLast,
                             CURLFORM_COPYNAME, fileIter->first.c_str(),
                             CURLFORM_FILE, fileIter->second.c_str(),
                             CURLFORM_END);
      if (errCode != CURL_FORMADD_OK)
      {
        std::cerr << "cURL error: could not add file to multipart/formdata!"
                  << std::endl;
        curl_formfree(formFirst);
        curl_slist_free_all(header_list);
        header_list = nullptr;
        curl_easy_cleanup(handle);
        return false;
      }
      ++fileIter;
    } //while

    //add normal post fields
    auto pfIter = m_PostFields.begin();
    while (pfIter != m_PostFields.end())
    {
      CURLFORMcode errCode = curl_formadd(&formFirst, &formLast,
                             CURLFORM_COPYNAME, pfIter->first.c_str(),
                             CURLFORM_COPYCONTENTS, pfIter->second.c_str(),
                             CURLFORM_END);
      if (errCode != CURL_FORMADD_OK)
      {
        std::cerr << "cURL error: could not add file to multipart/formdata!"
                  << std::endl;
        curl_formfree(formFirst);
        curl_slist_free_all(header_list);
        header_list = nullptr;
        curl_easy_cleanup(handle);
        return false;
      }
      ++pfIter;
    } //while post fields
    retCode = curl_easy_setopt(handle, CURLOPT_HTTPPOST, formFirst);
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting multipart form data failed! Error: "
                << curl_easy_strerror(retCode) << std::endl;
      curl_formfree(formFirst);
      curl_slist_free_all(header_list);
      header_list = nullptr;
      curl_easy_cleanup(handle);
      return false;
    }
  } //if files are there

  //set plain post body - but only if other POST stuff is empty
  if (m_UsePostBody && m_PostFields.empty() && m_Files.empty())
  {
    retCode = curl_easy_setopt(handle, CURLOPT_POST, 1L);
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting POST mode for Curly::perform failed! Error: "
                << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      curl_slist_free_all(header_list);
      header_list = nullptr;
      return false;
    }
    retCode = curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, m_PostBody.size());
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting size of POST body for Curly::perform failed! Error: "
                << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      curl_slist_free_all(header_list);
      header_list = nullptr;
      return false;
    }
    retCode = curl_easy_setopt(handle, CURLOPT_POSTFIELDS, m_PostBody.c_str());
    if (retCode != CURLE_OK)
    {
      std::cerr << "cURL error: setting POST body for Curly::perform failed! Error: "
                << curl_easy_strerror(retCode) << std::endl;
      curl_easy_cleanup(handle);
      curl_slist_free_all(header_list);
      header_list = nullptr;
      return false;
    }
  } //if post body

  //set write callback
  retCode = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallbackString);
  if (retCode != CURLE_OK)
  {
    std::cerr << "curl_easy_setopt() of Curly::perform could not set write function! Error: "
              << curl_easy_strerror(retCode) << std::endl;
    curl_formfree(formFirst);
    curl_slist_free_all(header_list);
    header_list = nullptr;
    curl_easy_cleanup(handle);
    return false;
  }
  //provide string stream for the data
  std::string string_data("");
  retCode = curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&string_data);
  if (retCode != CURLE_OK)
  {
    std::cerr << "curl_easy_setopt() of Curly::perform could not set write data! Error: "
              << curl_easy_strerror(retCode) << std::endl;
    curl_formfree(formFirst);
    curl_slist_free_all(header_list);
    header_list = nullptr;
    curl_easy_cleanup(handle);
    return false;
  }

  //send
  #ifdef DEBUG_MODE
  std::clog << "calling cURL easy perform..." << std::endl;
  #endif
  retCode = curl_easy_perform(handle);
  if (retCode != CURLE_OK)
  {
    std::cerr << "curl_easy_perform() of Curly::perform failed! Error: "
              << curl_easy_strerror(retCode) << std::endl;
    curl_formfree(formFirst);
    curl_slist_free_all(header_list);
    header_list = nullptr;
    curl_easy_cleanup(handle);
    return false;
  }
  #ifdef DEBUG_MODE
  else
  {
    std::clog << "POST request data was sent to server." << std::endl;
  }
  #endif
  //free multipart/formdata, if any data was given
  curl_formfree(formFirst);
  formFirst = nullptr;

  //free header data, if any data was given
  curl_slist_free_all(header_list);
  header_list = nullptr;

  //get response code
  retCode = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &m_LastResponseCode);
  if (retCode != CURLE_OK)
  {
    std::cerr << "curl_easy_getinfo() of Curly::perform failed! Error: "
              << curl_easy_strerror(retCode) << std::endl;
    curl_easy_cleanup(handle);
    m_LastResponseCode = 0;
    return false;
  }
  //get content type
  char * contType = nullptr;
  retCode = curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &contType);
  if (retCode != CURLE_OK)
  {
    std::cerr << "curl_easy_getinfo() of Curly::perform failed! Error: "
              << curl_easy_strerror(retCode) << std::endl;
    curl_easy_cleanup(handle);
    m_LastContentType.erase();
    return false;
  }
  if (contType == nullptr)
    m_LastContentType.erase();
  else
    m_LastContentType = std::string(m_LastContentType);

  curl_easy_cleanup(handle);
  response = std::move(string_data);
  return true;
}

long Curly::getResponseCode() const
{
  return m_LastResponseCode;
}

const std::string& Curly::getContentType() const
{
  return m_LastContentType;
}

Curly::VersionData::VersionData()
: cURL(""),
  ssl(""),
  libz(""),
  protocols(std::vector<std::string>()),
  ares(""),
  idn(""),
  ssh("")
{
}

Curly::VersionData Curly::curlVersion()
{
  auto data = curl_version_info(CURLVERSION_NOW);
  VersionData vd;
  if (data->age < 0)
    return vd;
  //cURL version
  vd.cURL = std::string(data->version);
  //OpenSSL version
  if (data->ssl_version != nullptr)
    vd.ssl = std::string(data->ssl_version);
  //zlib version
  if (data->libz_version != nullptr)
    vd.libz = std::string(data->libz_version);
  //supported protocols
  if (data->protocols != nullptr)
  {
    unsigned int i;
    for (i = 0; data->protocols[i] != nullptr; ++i)
    {
      vd.protocols.push_back(std::string(data->protocols[i]));
    } //for
  } //if

  if (data->age < 1)
    return vd;
  //ares version
  if (data->ares != nullptr)
    vd.ares = std::string(data->ares);

  if (data->age < 2)
    return vd;
  //idn version
  if (data->libidn != nullptr)
    vd.idn = std::string(data->libidn);

  if (data->age < 3)
    return vd;
  //libssh version
  if (data->libssh_version != nullptr)
    vd.ssh = std::string(data->libssh_version);
  return vd;
}

size_t Curly::headerCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
  const size_t actualSize = size * nitems;
  if (buffer == nullptr)
  {
    std::cerr << "Error: header callback received null pointer as buffer!" << std::endl;
    return 0;
  }
  if (nullptr == userdata)
  {
    std::cerr << "Error: header callback received null pointer as user data!" << std::endl;
    return 0;
  }
  Curly* instance = reinterpret_cast<Curly*>(userdata);
  if (nullptr == instance)
  {
    std::cerr << "Error: header callback's user data is not a Curly instance!" << std::endl;
    return 0;
  }
  instance->addResponseHeader(std::string(buffer, actualSize));
  instance = nullptr;
  return actualSize;
}

void Curly::addResponseHeader(std::string respHeader)
{
  //erase leading whitespaces
  std::string::size_type firstNonWhitespace = std::string::npos;
  {
    std::string::size_type i;
    for (i = 0; i < respHeader.size(); ++i)
    {
      if (!std::isspace(respHeader[i]))
      {
        firstNonWhitespace = i;
        break;
      }
    } //for
  } //scope for i
  respHeader.erase(0, firstNonWhitespace);
  //erase trailing whitespaces
  std::string::size_type lastNonWhitespace = std::string::npos;
  int si;
  for (si = respHeader.size() -1; si >= 0; --si)
  {
    if (!std::isspace(respHeader[si]))
    {
      lastNonWhitespace = si;
      break;
    }
  } //for
  if ((std::string::npos != lastNonWhitespace) && (lastNonWhitespace + 1 < respHeader.size()))
    respHeader.erase(lastNonWhitespace+1);

  //only add non-empty headers
  if (!respHeader.empty())
    m_ResponseHeaders.push_back(respHeader);
}

const std::vector<std::string>& Curly::responseHeaders() const
{
  return m_ResponseHeaders;
}
