#include <Poco/Net/SocketStream.h>

#ifndef _WORDLE_HPP
#define _WORDLE_HPP

class Wordle {
private:
  Poco::Net::SocketStream &connection;
  const std::string wordlist;

public:
  Wordle(Poco::Net::SocketStream &connection, const std::string &wordlist)
      : connection(connection), wordlist(wordlist){};
  int run();
  bool process_guess(const std::string &guess, const std::string &todays_word);
};

#endif
