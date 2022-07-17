#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "wordle.hpp"

namespace fs = std::filesystem;

#define RESET "\033[0m"
#define RED "\033[31m"    /* Red */
#define GREEN "\033[32m"  /* Green */
#define YELLOW "\033[33m" /* Yellow */

std::vector<std::string> read_wordlist(const std::string &wordlist_file) {
  // Make sure the wordlist provided is valid
  fs::path wordlist_path{wordlist_file};
  if (!fs::exists(wordlist_path)) {
    throw std::runtime_error("The wordlist provided does not exist");
  }
  std::ifstream infile{wordlist_file};
  // Build the wordlist into a vector
  std::vector<std::string> words{};
  std::string line{};
  while (std::getline(infile, line)) {
    words.push_back(line);
  }
  return words;
}

std::string get_word(const std::vector<std::string> &wordlist) {
  // Get a random element from the wordlist
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> int_generator(
      0, wordlist.size() - 1);
  auto val = int_generator(rng);
  auto todays_word = wordlist[val];

  return todays_word;
}

std::map<char, unsigned> build_char_counts(const std::string &s) {
  std::map<char, unsigned> output{};
  for (const auto i : s) {
    if (output.find(i) == output.end()) {
      output[i] = 1;
    } else {
      output[i]++;
    }
  }
  return output;
}

bool Wordle::process_guess(const std::string &guess,
                           const std::string &todays_word) {
  if (guess == todays_word)
    return true;

  auto word_counts = build_char_counts(todays_word);
  auto guess_counts = build_char_counts(guess);

  for (int i = 0; i < 5; ++i) {
    if (guess[i] == todays_word[i]) {
      this->connection << GREEN << guess[i] << RESET << std::flush;
      guess_counts[guess[i]]--;
      word_counts[guess[i]]--;
    } else if (todays_word.find(guess[i]) != std::string::npos &&
               word_counts[guess[i]] != 0 && guess_counts[guess[i]] != 0) {
      // Indicate the letter is in the word but prevent multiple counting if not
      // valid
      this->connection << YELLOW << guess[i] << RESET << std::flush;
      word_counts[guess[i]]--;
      guess_counts[guess[i]]--;
    } else {
      this->connection << guess[i] << std::flush;
    }
  }
  this->connection << std::endl << std::flush;
  return false;
}

int Wordle::run() {
  auto wordlist_words = read_wordlist(this->wordlist);
  auto todays_word = get_word(wordlist_words);

  std::stringstream display_buffer{};
#ifdef DEBUG
  this->connection << "Word: " << RED << todays_word << RESET << "\n"
                   << std::flush;
#endif

  for (int i = 0; i < 6; ++i) {
    this->connection << i + 1 << "> " << std::flush;

    std::string guess{};
    this->connection >> guess;
    std::transform(guess.begin(), guess.end(), guess.begin(), ::toupper);

    if (guess == "QUIT") {  // User giving up
      this->connection << RED << "Quitting..." << RESET << "\n" << std::flush;
      return 0;
    } else if (guess.length() != 5) {  // Invalid length
      this->connection << RED << "Invalid word length, try again "
                       << guess.length() << RESET << "\n"
                       << std::flush;
      --i;
      continue;
    } else if (std::find(wordlist_words.begin(), wordlist_words.end(), guess) ==
               wordlist_words.end()) {  // Not in wordlist
      this->connection << RED << "Word not in wordlist, try again" << RESET
                       << "\n"
                       << std::flush;
      --i;
      continue;
    }

    bool finished = process_guess(guess, todays_word);
    if (finished) {
      this->connection << GREEN << "Congratulations!" << RESET << "\n"
                       << std::flush;
      break;
    }
  }
  this->connection << "Todays word was " << todays_word << "\n";
  return 0;
}
