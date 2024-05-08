//
// Created by Bulat Baikenov on 03.05.2024.
//

#ifndef TP_PROJECT_BOT_H
#define TP_PROJECT_BOT_H

#import "Map.h"

class Bot {
private:
public:
    Map map;
    size_t attack();
    bool lost() const;
};


#endif //TP_PROJECT_BOT_H
