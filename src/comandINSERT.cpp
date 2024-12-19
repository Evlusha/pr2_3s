
#include "../include/menu.h"

// функция проверки ввода команды insert
string DataBase::isValidInsert(string& command) { 
    string table;
    int position = command.find_first_of(' ');
    if (position != -1) { // проверка синтаксиса
        table = command.substr(0, position);
        command.erase(0, position + 1);
        if (nametables.getindex(table) != -1) { // проверка таблицы
            if (command.substr(0, 7) == "values ") { // проверка values
                command.erase(0, 7);
                position = command.find_first_of(' ');
                if (position == -1) { // проверка синтаксиса ///////
                    if (command[0] == '(' && command[command.size()-1] == ')') { // проверка синтаксиса скобок и их удаление
                        command.erase(0, 1);
                        command.pop_back();
                        position = command.find(' ');
                        while (position != -1) { // удаление пробелов
                            command.erase(position);
                            position = command.find(' ');
                        }
                        return insert(table, command);
                    } else return "Ошибка, нарушен синтаксис команды!";
                } else return "Ошибка, нарушен синтаксис команды!";
            } else return "Ошибка, нарушен синтаксис команды!";
        } else return "Ошибка, нет такой таблицы!";
    } else return "Ошибка, нарушен синтаксис команды!";
}

// функция вставки в таблицу
string DataBase::insert(string& table, string& values) { 
    string filepath = "../" + nameBD + "/" + table + "/" + table + "_pk_sequence.txt";
    int index = nametables.getindex(table); // получаем индекс таблицы(aka key)
    string val = fileinput(filepath);
    int valint = stoi(val);
    valint++;
    fileoutput(filepath, to_string(valint));

    if (checkLockTable(table)) {
        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "close");

        // вставка значений в csv, не забывая про увеличение ключа
        filepath = "../" + nameBD + "/" + table + "/1.csv";
        int countline = CountLine(filepath);
        int fileid = 1; // номер файла csv
        while (true) {
            if (countline == tupleslimit) { // если достигнут лимит, то создаем/открываем другой файл
                fileid++;
                filepath = "../" + nameBD + "/" + table + "/" + to_string(fileid) + ".csv";
                if (fileindex.getvalue(index) < fileid) {
                    fileindex.replace(index, fileid);
                }
            } else break;
            countline = CountLine(filepath);
        }

        fstream file;
        file.open(filepath, ios::app);
        file << val + ',' + values + '\n';
        file.close();

        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "open");
        return "Команда выполнена!";
    } else return "Ошибка, таблица используется другим пользователем!";
}
