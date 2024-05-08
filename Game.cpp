#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include "Map.h"
#include "Bot.h"
class Game {
public:
    Game() : map_(), input_() {}

    void run() {
      initscr();
      refresh();

      while (true) {
        clear();
        map_.print();
        refresh();

        ask_user_for_input();
      }

      endwin();
    }

private:
    void ask_user_for_input() {
      int y, x;
      getyx(stdscr, y, x);
      printw("What do you want to do?\nPlace ship(type p) or start the game(type s)?\n");
      refresh();
      getstr(input_);

      if (input_[0] == 's') {
        clear();
        printw("Hehe, do you wanna fight with human or not? (type h or a)\n");
        refresh();
        getstr(input_);
        if (input_[0] == 'h') {
          //Second iteration
        } else if (input_[0] == 'a') {
          Bot bot;
          bot.map.place_random();
          print("Fight started!\n");
          int random = rand() % 2;//rand() % 2
          if (random == 0) {
            print("Bot has privilege to attack first!\n");
            size_t atk = bot.attack();
            print((std::to_string(atk)+ "\n").c_str());
            while (!map_.make_dead(atk)) {
              atk = bot.attack();
              print((std::to_string(atk) + "\n").c_str());
            }
            if (map_[atk].is_ship()) {
              random = 0;
            } else {
              random = 1;
            }
          }
          while (true) {
            if (bot.lost()) {
              print("You won!\n");
              break;
            } else if (map_.is_lost()) {
              print("You lost(\n");
              break;
            }
            if (random == 0) {
              size_t atk = bot.attack();
              while (!map_.make_dead(atk)) {
                atk = bot.attack();
              }
              if (map_[atk].is_ship()) {
                random = 0;
              } else {
                random = 1;
              }
              usleep(2500000);
            } else {
              print("Give me 'Xy' i.e. 'A0' to attack");
              getstr(input_);
              size_t first = (input_[0] - 'A') * 10 + (input_[1] - '0');
              map_.make_dead(first);
              if (map_[first].is_ship()) {
                random = 1;
              } else {
                random = 0;
              }
            }
            map_.print();
          }
        } else {
          print("Omg, you already broke it...");
        }
      } else if (input_[0] == 'p') {
        place_ship(y, x);
      } else {
        print("Wtf bro, i have already told you what to do, just press s or p!");
      }
    }

    void place_ship(int y, int x) {
      clear();
      map_.print();
      move(y, x);
      print("\n\nEnter first tile and second tile in format 'Xy Xy', where X is latin capital letter and y is number \n");
      refresh();
      getstr(input_);

      if (is_valid_input()) {
        size_t size_of_ship = calculate_ship_size();
        if (size_of_ship > 4) {
          print("Why is your ship have length more than 4 tiles...");
        } else if (map_.ships_[size_of_ship - 1] == 0) {
          print("Ships are over");
        } else {
          size_t first_point = calculate_first_point();
          size_t second_point = calculate_second_point();
          if (map_.can_place_ship(first_point, second_point)) {
            map_.place_ship(first_point, second_point);
            block_tiles(first_point, second_point);
            map_.ships_[size_of_ship - 1]--;
          } else {
            print("Are you joking? Why are you placing here ship...");
          }
        }
      } else {
        print("Something wrong with format, fix it!!");
      }
    }

    bool is_valid_input() {
      return isupper(input_[0]) && isupper(input_[3]) && isdigit(input_[1]) && isdigit(input_[4]);
    }

    size_t calculate_ship_size() {
      return std::abs(input_[0] - input_[3]) + std::abs(input_[1] - input_[4]) + 1;
    }

    size_t calculate_first_point() {
      return (std::min(input_[0], input_[3]) - 'A') * 10 + (std::min(input_[1], input_[4]) - '0');
    }

    size_t calculate_second_point() {
      return (std::max(input_[0], input_[3]) - 'A') * 10 + (std::max(input_[1], input_[4]) - '0');
    }
    void block_tiles(const size_t& first_point, const size_t& second_point) {
      size_t diff = 1;
      if (second_point - first_point >= 4) {
        diff = 10;
      }
      const int res[] {-11, -10, -9, -1, +1, +9, +10, +11};
      for (size_t i = first_point; i <= second_point; i += diff) {
        for (auto x : res) {
          if (0 <= i + x && i + x < 100 && !map_[i+x].is_ship()) {
            bool expr;
            if (i % 10 == 9 && (x == +1 || x == -9 || x == +11)) {
              continue;
            }
            if (i % 10 == 0 && (x == -11 || x == -1 || x == +9)) {
              continue;
            }
            if (0 == i && x == +9) {
              continue;
            }
            if (!map_[i+x].is_false()) {
              map_[i+x].set_false();
            }
          }
        }
      }
    }


    void print(const char* message) {
      clear();
      printw("%s\n", message);
      refresh();
      usleep(1500000);
    }

    Map map_;
    char input_[256];
};

int main() {
  srand(time(0));
  Game game;
  game.run();
  return 0;
}
//#include <iostream>
//#include <ncurses.h>
//#include <unistd.h>
//#include "Map.h"
//int main() {
//  initscr();
//  refresh();
//  Map map;
//  size_t i = 0;
//  while (true) {
//    clear();
//    map.print();
//    refresh();
//    i++;
//    char input[256];
//    int y;
//    int x;
//    getyx(stdscr, y, x);
//    printw("What do you want to do?\nPlace ship(type p) or start the game(type s)?\n");
//    refresh();
//    getstr(input);
//    if (input[0] == 's') {
//      //TODO do something
//    } else if (input[0] == 'p') {
//      memset(input, 0, sizeof input);
//      clear();
//      map.print();
//      move(y, x);
//      printw("\n\nEnter first tile and second tile in format 'Xy Xy', where X is latin capital letter and y is number \n");
//      getstr(input);
//      bool is_good_format = isupper(input[0]) && isupper(input[3]) && isdigit(input[1]) && isdigit(input[4]);
//      bool is_diag = (input[0] != input[3]) && (input[1] != input[4]);
//      if (is_good_format) {
//        clear();
//        if (is_diag) {
//          printw("Htf should i do diagonal ships?!\n");
//          refresh();
//          usleep(1000000);
//        } else {
//          size_t size_of_ship = std::abs(input[0] - input[3]) + std::abs(input[1] - input[4]) + 1;
//          if (size_of_ship > 4) {
//            printw("Why is your ship have length more than 4 tiles...\n");
//            refresh();
//            usleep(1000000);
//          } else {
//            if (map.ships_[size_of_ship - 1] == 0) {
//              printw("Ships are over\n");
//              refresh();
//              usleep(1000000);
//            } else {
//              size_t first_point = (std::min(input[0], input[3]) - 'A') * 10
//                      + (std::min(input[1], input[4]) - '0');
//              size_t second_point = (std::max(input[0], input[3]) - 'A') * 10
//                                   + (std::max(input[1], input[4]) - '0');
//              bool is_false = false;
//              for (size_t i = first_point; i <= second_point; ++i) {
//                if (map[i].is_false() or map[i].is_ship()) {
//                  printw("Are you joking? Why are you placing here ship...");
//                  is_false = true;
//                  break;
//                }
//              }
//              if (!is_false) {
//                for (size_t i = first_point; i <= second_point; ++i) {
//                  map[i].set_ship();
//                }
//              }
//            }
//          }
//        }
//      } else {
//        clear();
//        printw("Something wrong with format, fix it!!\n");
//        refresh();
//        usleep(500000);
//      }
//    } else {
//      clear();
//      printw("Wtf bro, i have already told you what to do, just press s or p!\n");
//      refresh();
//      usleep(500000);
//    }
//    usleep(1000000);
//    move(0, 0);
//  }
//  endwin();
//}
//if
//i would work in the USA if i were talking in english
//i would have travelled to the Black Sea if we had money
//if i
//if you hear the signal, open the door
//if we have lived in countryside, we would have more health
//We should spend more time on english
//You should buy another car
//You haven't to text them about that
//I wonder he had
