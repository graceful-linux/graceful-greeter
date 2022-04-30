//
// Created by dingjing on 4/6/22.
//

#include "../app/model/users-model.h"

#include <iostream>

int main (int argc, char* argv[])
{
    UsersModel um;

    int userCount = um.rowCount (QModelIndex());

    for (int i = 0; i < userCount; ++i) {
        std::cout << "用户名: " << um.data (um.index (i, 0), UsersModel::RealNameRole).toString().toStdString()
        << "  用户ID: " << um.data (um.index (i, 0), UsersModel::UidRole).toString().toStdString()
        << std::endl;
    }

    return 0;
}