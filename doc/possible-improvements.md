# botvinnik: Possible improvements for future development

* Threaded command processing: Currently, all commands are processed in one
  single thread - including those that need to reach out to other sites. That
  is suboptimal and could be improved by (at least partial) use of threads.
* Asynchronous message processing: At the moment all messages are processed
  synchronously. Therefore, if one command hangs for a few seconds, all other
  bot commands will be delayed for that amount of time.
* Rate-limit commands: At the moment users are free to spam as much commands as
  they please. While that may be fine for them, it may not be fine for the bot
  user. Like any other Matrix user the bot user has to obey rate limits for
  sending messages. If users send too much requests, the bot cannot answer all
  of them, because the Matrix server will just stop accepting messages from the
  bot user for a certain period of time.
* Inform users about missed answers due to rate limits: When the Matrix server
  denies sending a message due to rate limits, it will usually reply by sending
  a response with HTTP status code 429 ("Too Many Requests", also see the
  [MDN page](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/429)) and
  a JSON containing the amount of time after which the user may try again:

    {
        "errcode": "M_LIMIT_EXCEEDED",
        "error": "Too Many Requests",
        "retry_after_ms":2402
    }

  That information could be used to send a notification about the rate limit to
  spamming users after the `retry_after_ms` period.
