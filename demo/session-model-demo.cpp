//
// Created by dingjing on 4/6/22.
//

#include "../app/model/sessions-model.h"

#include <iostream>

int main (int argc, char* argv[])
{
    SessionsModel ss;

    int userCount = ss.rowCount (QModelIndex());

    for (int i = 0; i < userCount; ++i) {
        std::cout << "本地会话: " << ss.data (ss.index (i, 0), SessionsModel::LocalSessions).toString().toStdString() << std::endl;
    }

    return 0;
}