//
// Created by Bulat Baikenov on 01.05.2024.
//

#ifndef TP_PROJECT_BLOCK_H
#define TP_PROJECT_BLOCK_H
#include <iostream>
#include <string>
class Block {
private:
    bool is_blocked_;
    bool is_ship_;
    bool is_false_;
public:
    Block();
    bool is_blocked() const noexcept;
    bool is_ship() const noexcept;
    bool is_false() const noexcept;
    void set_block() noexcept;
    void set_ship() noexcept;
    void set_false() noexcept;
    friend std::ostream& operator<< (std::ostream&, const Block&) noexcept;
    std::string handle() const noexcept;
};


#endif //TP_PROJECT_BLOCK_H
