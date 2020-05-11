#include "curl_error_category.h"

namespace tpxai::dahua {

const char* CURLErrorCategory::name() const noexcept { return "CURL"; }

std::string CURLErrorCategory::message(int condition) const {
  switch (static_cast<CURLErrorCode>(condition)) {
    case CURLE_OK:
      return "OK";
    case CURLE_UNSUPPORTED_PROTOCOL:    /* 1 */
      return "UNSUPPORTED_PROTOCOL";
    case CURLE_FAILED_INIT:             /* 2 */
      return "FAILED_INIT";
    case CURLE_URL_MALFORMAT:           /* 3 */
      return "URL_MALFORMAT";
    case CURLE_NOT_BUILT_IN:            /* 4 - [was obsoleted in August 2007 for 7.17.0, reused in April 2011 for 7.21.5] */
      return "NOT_BUILT_IN";
    case CURLE_COULDNT_RESOLVE_PROXY:   /* 5 */
      return "COULDNT_RESOLVE_PROXY";
    case CURLE_COULDNT_RESOLVE_HOST:    /* 6 */
      return "COULDNT_RESOLVE_HOST";
    case CURLE_COULDNT_CONNECT:         /* 7 */
      return "COULDNT_CONNECT";
    case CURLE_WEIRD_SERVER_REPLY:      /* 8 */
      return "WEIRD_SERVER_REPLY";
    case CURLE_REMOTE_ACCESS_DENIED:    /* 9 a service was denied by the server
                                    due to lack of access - when login fails
                                    this is not returned. */
      return "REMOTE_ACCESS_DENIED";
    case CURLE_FTP_ACCEPT_FAILED:       /* 10 - [was obsoleted in April 2006 for
                                    7.15.4, reused in Dec 2011 for 7.24.0]*/
      return "FTP_ACCEPT_FAILED";
    case CURLE_FTP_WEIRD_PASS_REPLY:    /* 11 */
      return "FTP_WEIRD_PASS_REPLY";
    case CURLE_FTP_ACCEPT_TIMEOUT:      /* 12 - timeout occurred accepting server
                                    [was obsoleted in August 2007 for 7.17.0,
                                    reused in Dec 2011 for 7.24.0]*/
      return "FTP_ACCEPT_TIMEOUT";
    case CURLE_FTP_WEIRD_PASV_REPLY:    /* 13 */
      return "FTP_WEIRD_PASV_REPLY";
    case CURLE_FTP_WEIRD_227_FORMAT:    /* 14 */
      return "FTP_WEIRD_227_FORMAT";
    case CURLE_FTP_CANT_GET_HOST:       /* 15 */
      return "FTP_CANT_GET_HOST";
    case CURLE_HTTP2:                   /* 16 - A problem in the http2 framing layer.
                                    [was obsoleted in August 2007 for 7.17.0,
                                    reused in July 2014 for 7.38.0] */
      return "HTTP2";
    case CURLE_FTP_COULDNT_SET_TYPE:    /* 17 */
      return "FTP_COULDNT_SET_TYPE";
    case CURLE_PARTIAL_FILE:            /* 18 */
      return "PARTIAL_FILE";
    case CURLE_FTP_COULDNT_RETR_FILE:   /* 19 */
      return "FTP_COULDNT_RETR_FILE";
    case CURLE_OBSOLETE20:              /* 20 - NOT USED */
      return "OBSOLETE20";
    case CURLE_QUOTE_ERROR:             /* 21 - quote command failure */
      return "QUOTE_ERROR";
    case CURLE_HTTP_RETURNED_ERROR:     /* 22 */
      return "HTTP_RETURNED_ERROR";
    case CURLE_WRITE_ERROR:             /* 23 */
      return "WRITE_ERROR";
    case CURLE_OBSOLETE24:              /* 24 - NOT USED */
      return "OBSOLETE24";
    case CURLE_UPLOAD_FAILED:           /* 25 - failed upload "command" */
      return "UPLOAD_FAILED";
    case CURLE_READ_ERROR:              /* 26 - couldn't open/read from file */
      return "READ_ERROR";
    case CURLE_OUT_OF_MEMORY:           /* 27 */
      return "OUT_OF_MEMORY";
    case CURLE_OPERATION_TIMEDOUT:      /* 28 - the timeout time was reached */
      return "OPERATION_TIMEDOUT";
    case CURLE_OBSOLETE29:              /* 29 - NOT USED */
      return "OBSOLETE29";
    case CURLE_FTP_PORT_FAILED:         /* 30 - FTP PORT operation failed */
      return "FTP_PORT_FAILED";
    case CURLE_FTP_COULDNT_USE_REST:    /* 31 - the REST command failed */
      return "FTP_COULDNT_USE_REST";
    case CURLE_OBSOLETE32:              /* 32 - NOT USED */
      return "OBSOLETE32";
    case CURLE_RANGE_ERROR:             /* 33 - RANGE "command" didn't work */
      return "RANGE_ERROR";
    case CURLE_HTTP_POST_ERROR:         /* 34 */
      return "HTTP_POST_ERROR";
    case CURLE_SSL_CONNECT_ERROR:       /* 35 - wrong when connecting with SSL */
      return "SSL_CONNECT_ERROR";
    case CURLE_BAD_DOWNLOAD_RESUME:     /* 36 - couldn't resume download */
      return "BAD_DOWNLOAD_RESUME";
    case CURLE_FILE_COULDNT_READ_FILE:  /* 37 */
      return "FILE_COULDNT_READ_FILE";
    case CURLE_LDAP_CANNOT_BIND:        /* 38 */
      return "LDAP_CANNOT_BIND";
    case CURLE_LDAP_SEARCH_FAILED:      /* 39 */
      return "LDAP_SEARCH_FAILED";
    case CURLE_OBSOLETE40:              /* 40 - NOT USED */
      return "OBSOLETE40";
    case CURLE_FUNCTION_NOT_FOUND:      /* 41 - NOT USED starting with 7.53.0 */
      return "FUNCTION_NOT_FOUND";
    case CURLE_ABORTED_BY_CALLBACK:     /* 42 */
      return "ABORTED_BY_CALLBACK";
    case CURLE_BAD_FUNCTION_ARGUMENT:   /* 43 */
      return "BAD_FUNCTION_ARGUMENT";
    case CURLE_OBSOLETE44:              /* 44 - NOT USED */
      return "OBSOLETE44";
    case CURLE_INTERFACE_FAILED:        /* 45 - CURLOPT_INTERFACE failed */
      return "INTERFACE_FAILED";
    case CURLE_OBSOLETE46:              /* 46 - NOT USED */
      return "OBSOLETE46";
    case CURLE_TOO_MANY_REDIRECTS:      /* 47 - catch endless re-direct loops */
      return "TOO_MANY_REDIRECTS";
    case CURLE_UNKNOWN_OPTION:          /* 48 - User specified an unknown option */
      return "UNKNOWN_OPTION";
    case CURLE_TELNET_OPTION_SYNTAX:    /* 49 - Malformed telnet option */
      return "TELNET_OPTION_SYNTAX";
    case CURLE_OBSOLETE50:              /* 50 - NOT USED */
      return "OBSOLETE50";
    case CURLE_OBSOLETE51:              /* 51 - NOT USED */
      return "OBSOLETE51";
    case CURLE_GOT_NOTHING:             /* 52 - when this is a specific error */
      return "GOT_NOTHING";
    case CURLE_SSL_ENGINE_NOTFOUND:     /* 53 - SSL crypto engine not found */
      return "SSL_ENGINE_NOTFOUND";
    case CURLE_SSL_ENGINE_SETFAILED:    /* 54 - can not set SSL crypto engine as default */
      return "SSL_ENGINE_SETFAILED";
    case CURLE_SEND_ERROR:              /* 55 - failed sending network data */
      return "SEND_ERROR";
    case CURLE_RECV_ERROR:              /* 56 - failure in receiving network data */
      return "RECV_ERROR";
    case CURLE_OBSOLETE57:              /* 57 - NOT IN USE */
      return "OBSOLETE57";
    case CURLE_SSL_CERTPROBLEM:         /* 58 - problem with the local certificate */
      return "SSL_CERTPROBLEM";
    case CURLE_SSL_CIPHER:              /* 59 - couldn't use specified cipher */
      return "SSL_CIPHER";
    case CURLE_PEER_FAILED_VERIFICATION:/* 60 - peer's certificate or fingerprint wasn't verified fine */
      return "PEER_FAILED_VERIFICATION";
    case CURLE_BAD_CONTENT_ENCODING:    /* 61 - Unrecognized/bad encoding */
      return "BAD_CONTENT_ENCODING";
    case CURLE_LDAP_INVALID_URL:        /* 62 - Invalid LDAP URL */
      return "LDAP_INVALID_URL";
    case CURLE_FILESIZE_EXCEEDED:       /* 63 - Maximum file size exceeded */
      return "FILESIZE_EXCEEDED";
    case CURLE_USE_SSL_FAILED:          /* 64 - Requested FTP SSL level failed */
      return "USE_SSL_FAILED";
    case CURLE_SEND_FAIL_REWIND:        /* 65 - Sending the data requires a rewind that failed */
      return "SEND_FAIL_REWIND";
    case CURLE_SSL_ENGINE_INITFAILED:   /* 66 - failed to initialise ENGINE */
      return "SSL_ENGINE_INITFAILED";
    case CURLE_LOGIN_DENIED:            /* 67 - user, password or similar was not accepted and we failed to login */
      return "LOGIN_DENIED";
    case CURLE_TFTP_NOTFOUND:           /* 68 - file not found on server */
      return "TFTP_NOTFOUND";
    case CURLE_TFTP_PERM:               /* 69 - permission problem on server */
      return "TFTP_PERM";
    case CURLE_REMOTE_DISK_FULL:        /* 70 - out of disk space on server */
      return "REMOTE_DISK_FULL";
    case CURLE_TFTP_ILLEGAL:            /* 71 - Illegal TFTP operation */
      return "TFTP_ILLEGAL";
    case CURLE_TFTP_UNKNOWNID:          /* 72 - Unknown transfer ID */
      return "TFTP_UNKNOWNID";
    case CURLE_REMOTE_FILE_EXISTS:      /* 73 - File already exists */
      return "REMOTE_FILE_EXISTS";
    case CURLE_TFTP_NOSUCHUSER:         /* 74 - No such user */
      return "TFTP_NOSUCHUSER";
    case CURLE_CONV_FAILED:             /* 75 - conversion failed */
      return "CONV_FAILED";
    case CURLE_CONV_REQD:               /* 76 - caller must register conversion
                                    callbacks using curl_easy_setopt options
                                    CURLOPT_CONV_FROM_NETWORK_FUNCTION,
                                    CURLOPT_CONV_TO_NETWORK_FUNCTION, and
                                    CURLOPT_CONV_FROM_UTF8_FUNCTION */
      return "CONV_REQD";
    case CURLE_SSL_CACERT_BADFILE:      /* 77 - could not load CACERT file, missing
                                    or wrong format */
      return "SSL_CACERT_BADFILE";
    case CURLE_REMOTE_FILE_NOT_FOUND:   /* 78 - remote file not found */
      return "REMOTE_FILE_NOT_FOUND";
    case CURLE_SSH:                     /* 79 - error from the SSH layer, somewhat
                                    generic so the error message will be of
                                    interest when this has happened */

      return "SSH";
    case CURLE_SSL_SHUTDOWN_FAILED:     /* 80 - Failed to shut down the SSL connection */
      return "SSL_SHUTDOWN_FAILED";
    case CURLE_AGAIN:                   /* 81 - socket is not ready for send/recv,
                                    wait till it's ready and try again (Added
                                    in 7.18.2) */
      return "AGAIN";
    case CURLE_SSL_CRL_BADFILE:         /* 82 - could not load CRL file, missing or wrong format (Added in 7.19.0) */
      return "SSL_CRL_BADFILE";
    case CURLE_SSL_ISSUER_ERROR:        /* 83 - Issuer check failed.  (Added in 7.19.0) */
      return "SSL_ISSUER_ERROR";
    case CURLE_FTP_PRET_FAILED:         /* 84 - a PRET command failed */
      return "FTP_PRET_FAILED";
    case CURLE_RTSP_CSEQ_ERROR:         /* 85 - mismatch of RTSP CSeq numbers */
      return "RTSP_CSEQ_ERROR";
    case CURLE_RTSP_SESSION_ERROR:      /* 86 - mismatch of RTSP Session Ids */
      return "RTSP_SESSION_ERROR";
    case CURLE_FTP_BAD_FILE_LIST:       /* 87 - unable to parse FTP file list */
      return "FTP_BAD_FILE_LIST";
    case CURLE_CHUNK_FAILED:            /* 88 - chunk callback reported error */
      return "CHUNK_FAILED";
    case CURLE_NO_CONNECTION_AVAILABLE: /* 89 - No connection available, the session will be queued */
      return "NO_CONNECTION_AVAILABLE";
    case CURLE_SSL_PINNEDPUBKEYNOTMATCH:/* 90 - specified pinned public key did not match */
      return "SSL_PINNEDPUBKEYNOTMATCH";
    case CURLE_SSL_INVALIDCERTSTATUS:   /* 91 - invalid certificate status */
      return "SSL_INVALIDCERTSTATUS";
    case CURLE_HTTP2_STREAM:            /* 92 - stream error in HTTP/2 framing layer */
      return "HTTP2_STREAM";
    case CURLE_RECURSIVE_API_CALL:      /* 93 - an api function was called from inside a callback */
      return "RECURSIVE_API_CALL";
    default:
      return "UNKNOWN ERROR";
  }
}

std::error_code make_error_code(const CURLErrorCode& e) {
  static const CURLErrorCategory error_category;
  return {static_cast<int>(e), error_category};
}

} // namespace tpxai::dahua
