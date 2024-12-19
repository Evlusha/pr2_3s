#include "../include/menu.h"

// функция  проверки ввода команды select
string DataBase::isValidSelect(string& command) { 
    Where conditions;
    SinglyLinkedList<Where> cond;

    if (command.find_first_of("from") != -1) {
        // работа со столбцами
        while (command.substr(0, 4) != "from") {
            string token = command.substr(0, command.find_first_of(' '));
            if (token.find_first_of(',') != -1) token.pop_back(); // удаляем запятую
            command.erase(0, command.find_first_of(' ') + 1);
            if (token.find_first_of('.') != -1) token.replace(token.find_first_of('.'), 1, " ");
            else {
                return "Ошибка, нарушен синтаксис команды!";
            }
            stringstream ss(token);
            ss >> conditions.table >> conditions.column;
            bool check = false;
            int i;
            for (i = 0; i < nametables.size; ++i) { // проверка, сущ. ли такая таблица
                if (conditions.table == nametables.getvalue(i)) {
                    check = true;
                    break;
                }
            }
            if (!check) {
                return "Нет такой таблицы!";
            }
            check = false;
            stringstream iss(stlb.getvalue(i));
            while (getline(iss, token, ',')) { // проверка, сущ. ли такой столбец
                if (token == conditions.column) {
                    check = true;
                    break;
                }
            }
            if (!check) {
                return "Нет такого столбца";
            }
            cond.push_back(conditions);
        }

        command.erase(0, command.find_first_of(' ') + 1); // скип from

        // работа с таблицами
        int iter = 0;
        while (!command.empty()) { // пока строка не пуста
            string token = command.substr(0, command.find_first_of(' '));
            if (token.find_first_of(',') != -1) {
                token.pop_back();
            }
            int position = command.find_first_of(' ');
            if (position != -1) command.erase(0, position + 1);
            else command.erase(0);
            if (iter + 1 > cond.size || token != cond.getvalue(iter).table) {
                return "Ошибка, указаные таблицы не совпадают или их больше!";
            }
            if (command.substr(0, 5) == "where") break; // также заканчиваем цикл если встретился WHERE
            iter++;
        }
        if (command.empty()) {
            return select(cond);
        } else {
            if (command.find_first_of(' ') != -1) {
                command.erase(0, 6);
                int position = command.find_first_of(' ');
                if (position != -1) {
                    string token = command.substr(0, position);
                    command.erase(0, position + 1);
                    if (token.find_first_of('.') != -1) {
                        token.replace(token.find_first_of('.'), 1, " ");
                        stringstream ss(token);
                        string table, column;
                        ss >> table >> column;
                        if (table == cond.getvalue(0).table) { // проверка таблицы в where
                            position = command.find_first_of(' ');
                            if ((position != -1) && (command[0] == '=')) {
                                command.erase(0, position + 1);
                                position = command.find_first_of(' ');
                                if (position == -1) { // если нет лог. операторов
                                    if (command.find_first_of('.') == -1) { // если просто значение
                                        conditions.value = command;
                                        conditions.check = true;
                                        return selectWithValue(cond, table, column, conditions);
                                    } else { // если столбец
                                        command.replace(command.find_first_of('.'), 1, " ");
                                        stringstream iss(command);
                                        iss >> conditions.table >> conditions.column;
                                        conditions.check = false;
                                        return selectWithValue(cond, table, column, conditions);
                                    }

                                } else { // если есть лог. операторы
                                    SinglyLinkedList<Where> values;
                                    token = command.substr(0, position);
                                    command.erase(0, position + 1);
                                    if (token.find_first_of('.') == -1) { // если просто значение
                                        conditions.value = token;
                                        conditions.check = true;
                                        values.push_back(conditions);
                                    } else { // если столбец
                                        token.replace(token.find_first_of('.'), 1, " ");
                                        stringstream stream(token);
                                        stream >> conditions.table >> conditions.column;
                                        conditions.check = false;
                                        values.push_back(conditions);
                                    }
                                    position = command.find_first_of(' ');
                                    if ((position != -1) && (command.substr(0, 2) == "or" || command.substr(0, 3) == "and")) {
                                        conditions.logicalOP = command.substr(0, position);
                                        command.erase(0, position + 1);
                                        position = command.find_first_of(' ');
                                        if (position != -1) {
                                            token = command.substr(0, position);
                                            command.erase(0, position + 1);
                                            if (token.find_first_of('.') != -1) {
                                                token.replace(token.find_first_of('.'), 1, " ");
                                                stringstream istream(token);
                                                SinglyLinkedList<string> tables;
                                                SinglyLinkedList<string> columns;
                                                tables.push_back(table);
                                                columns.push_back(column);
                                                istream >> table >> column;
                                                tables.push_back(table);
                                                columns.push_back(column);
                                                if (table == cond.getvalue(0).table) { // проверка таблицы в where
                                                    position = command.find_first_of(' ');
                                                    if ((position != -1) && (command[0] == '=')) {
                                                        command.erase(0, position + 1);
                                                        position = command.find_first_of(' ');
                                                        if (position == -1) { // если нет лог. операторов
                                                            if (command.find_first_of('.') == -1) { // если просто значение
                                                                conditions.value = command.substr(0, position);
                                                                conditions.check = true;
                                                                command.erase(0, position + 1);
                                                                values.push_back(conditions);
                                                                return selectWithLogic(cond, tables, columns, values);
                                                            } else { // если столбец
                                                                token = command.substr(0, position);
                                                                token.replace(token.find_first_of('.'), 1, " ");
                                                                command.erase(0, position + 1);
                                                                stringstream stream(token);
                                                                stream >> conditions.table >> conditions.column;
                                                                conditions.check = false;
                                                                values.push_back(conditions);
                                                                return selectWithLogic(cond, tables, columns, values);
                                                            }
                                                        } else return "Ошибка, нарушен синтаксис команды!";
                                                    } else return "Ошибка, нарушен синтаксис команды!";
                                                } else return "Ошибка, таблица в where не совпадает с начальной!";
                                            } else return "Ошибка, нарушен синтаксис команды!";
                                        } else return "Ошибка, нарушен синтаксис команды!";
                                    } else return "Ошибка, нарушен синтаксис команды!";
                                }
                            } else return "Ошибка, нарушен синтаксис команды!";
                        } else return "Ошибка, таблица в where не совпадает с начальной!";
                    } else return "Ошибка, нарушен синтаксис команды!";
                } else return "Ошибка, нарушен синтаксис команды!";
            } else return "Ошибка, нарушен синтаксис команды!";
        }
    } else return "Ошибка, нарушен синтаксис команды!";
}

// функция  обычного select
string DataBase::select(SinglyLinkedList<Where>& conditions) { 
    for (int i = 0; i < conditions.size; ++i) {
        bool check = checkLockTable(conditions.getvalue(i).table);
        if (!check) {
            return "Ошибка, таблица открыта другим пользователем!";
        }
    }
    string filepath;
    for (int i = 0; i < conditions.size; ++i) {
        filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + conditions.getvalue(i).table + "_lock.txt";
        fileoutput(filepath, "close");
    }

    SinglyLinkedList<int> stlbindex = findIndexStlb(conditions); // узнаем индексы столбцов после "select"
    SinglyLinkedList<string> tables = textInFile(conditions); // записываем данные из файла в переменные для дальнейшей работы

    for (int i = 0; i < conditions.size; ++i) {
        filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + conditions.getvalue(i).table + "_lock.txt";
        fileoutput(filepath, "open");
    }

    return sample(stlbindex, tables); // выборка
}

//функция селекта с where для обычного условия
string DataBase::selectWithValue(SinglyLinkedList<Where>& conditions, string& table, string& stolbec, struct Where value) { 
    for (int i = 0; i < conditions.size; ++i) {
        bool check = checkLockTable(conditions.getvalue(i).table);
        if (!check) {
            return "Ошибка, таблица открыта другим пользователем!";
        }
    }
    string filepath;
    for (int i = 0; i < conditions.size; ++i) {
        filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + conditions.getvalue(i).table + "_lock.txt";
        fileoutput(filepath, "close");
    }

    SinglyLinkedList<int> stlbindex = findIndexStlb(conditions); // узнаем индексы столбцов
    int stlbindexval = findIndexStlbCond(table, stolbec); // узнаем индекс столбца условия
    int stlbindexvalnext = findIndexStlbCond(value.table, value.column); // узнаем индекс столбца условия после '='(нужно если условиестолбец)
    SinglyLinkedList<string> tables = textInFile(conditions); // записываем данные из файла в переменные для дальнейшей работы
    SinglyLinkedList<string> column = findStlbTable(conditions, tables, stlbindexvalnext, value.table);; // записываем колонки таблицы условия после '='(нужно если условиестолбец)
    
    // фильтруем нужные строки
    for (int i = 0; i < conditions.size; ++i) {
        if (conditions.getvalue(i).table == table) { 
            stringstream stream(tables.getvalue(i));
            string str;
            string filetext;
            int iterator = 0; // нужно для условиястолбец 
            while (getline(stream, str)) {
                stringstream istream(str);
                string token;
                int currentIndex = 0;
                while (getline(istream, token, ',')) {
                    if (value.check) { // для простого условия
                        if (currentIndex == stlbindexval && token == value.value) {
                            filetext += str + '\n';
                            break;
                        }
                        currentIndex++;
                    } else { // для условиястолбец
                        if (currentIndex == stlbindexval && token == column.getvalue(iterator)) {
                        filetext += str + '\n';
                        }
                        currentIndex++;
                    }
                }
                iterator++;
            }
            tables.replace(i, filetext);
        }
    }

    for (int i = 0; i < conditions.size; ++i) {
        filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + conditions.getvalue(i).table + "_lock.txt";
        fileoutput(filepath, "open");
    }

    return sample(stlbindex, tables); // выборка
}

string DataBase::selectWithLogic(SinglyLinkedList<Where>& conditions, SinglyLinkedList<string>& table, SinglyLinkedList<string>& stolbec, SinglyLinkedList<Where>& value) {
    for (int i = 0; i < conditions.size; ++i) {
        bool check = checkLockTable(conditions.getvalue(i).table);
        if (!check) {
            return "Ошибка, таблица открыта другим пользователем!";
        }
    }
    string filepath;
    for (int i = 0; i < conditions.size; ++i) {
        filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + conditions.getvalue(i).table + "_lock.txt";
        fileoutput(filepath, "close");
    }

    SinglyLinkedList<int> stlbindex = findIndexStlb(conditions); // узнаем индексы столбцов после "select"
    SinglyLinkedList<string> tables = textInFile(conditions); // записываем данные из файла в переменные для дальнейшей работы
    SinglyLinkedList<int> stlbindexval;// узнаем индексы столбца условия
    for (int i = 0; i < stolbec.size; ++i) {
        int index = findIndexStlbCond(table.getvalue(i), stolbec.getvalue(i));
        stlbindexval.push_back(index);
    }
    SinglyLinkedList<int> stlbindexvalnext; // узнаем индекс столбца условия после '='(нужно если условиестолбец)
    for (int i = 0; i < value.size; ++i) {
        int index = findIndexStlbCond(value.getvalue(i).table, value.getvalue(i).column); // узнаем индекс столбца условия после '='(нужно если условиестолбец)
        stlbindexvalnext.push_back(index);
    }
    SinglyLinkedList<string> column;
    for (int j = 0; j < value.size; ++j) {
        if (!value.getvalue(j).check) { // если условие столбец
            column = findStlbTable(conditions, tables, stlbindexvalnext.getvalue(j), value.getvalue(j).table);
        }
    }

    // фильтрация нужной строки
    for (int i = 0; i < conditions.size; ++i) {
        if (conditions.getvalue(i).table == table.getvalue(0)) {
            stringstream stream(tables.getvalue(i));
            string str;
            string filetext;
            int iterator = 0; // нужно для условиястолбец 
            while (getline(stream, str)) {
                SinglyLinkedList<bool> checkstr;
                for (int j = 0; j < value.size; ++j) {
                    stringstream istream(str);
                    string token;
                    int currentIndex = 0;
                    bool check = false;
                    while (getline(istream, token, ',')) {
                        if (value.getvalue(j).check) { // если просто условие
                            if (currentIndex == stlbindexval.getvalue(j) && token == value.getvalue(j).value) {
                                check = true;
                                break;
                            }
                            currentIndex++;
                        } else { // если условие столбец
                            if (currentIndex == stlbindexval.getvalue(j) && token == column.getvalue(iterator)) {
                                check = true;
                                break;
                            }
                            currentIndex++;
                        }
                    }
                    checkstr.push_back(check);
                }
                if (value.getvalue(1).logicalOP == "and") { // Если оператор И
                    if (checkstr.getvalue(0) && checkstr.getvalue(1)) filetext += str + "\n";
                } else { // Если оператор ИЛИ
                    if (!checkstr.getvalue(0) && !checkstr.getvalue(1));
                    else filetext += str + "\n";
                }
                iterator++;
            }
            tables.replace(i, filetext);
        }
    }

    for (int i = 0; i < conditions.size; ++i) {
        filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + conditions.getvalue(i).table + "_lock.txt";
        fileoutput(filepath, "open");
    }

    return sample(stlbindex, tables); // выборка
}

// функция  проверки, закрыта ли таблица
bool DataBase::checkLockTable(string table) { 
    string filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
    string check = fileinput(filepath);
    if (check == "open") return true;
    else return false;
}

// функция  нахождения индекса столбцов(для select)
SinglyLinkedList<int> DataBase::findIndexStlb(SinglyLinkedList<Where>& conditions) { 
    SinglyLinkedList<int> stlbindex;
    for (int i = 0; i < conditions.size; ++i) {
        int index = nametables.getindex(conditions.getvalue(i).table);
        string str = stlb.getvalue(index);
        stringstream ss(str);
        int stolbecindex = 0;
        while (getline(ss, str, ',')) {
            if (str == conditions.getvalue(i).column) {
                stlbindex.push_back(stolbecindex);
                break;
            }
            stolbecindex++;
        }
    }
    return stlbindex;
}

// функция  нахождения индекса столбца условия(для select)
int DataBase::findIndexStlbCond(string table, string stolbec) { 
    int index = nametables.getindex(table);
    string str = stlb.getvalue(index);
    stringstream ss(str);
    int stlbindex = 0;
    while (getline(ss, str, ',')) {
        if (str == stolbec) break;
        stlbindex++;
    }
    return stlbindex;
}

// функция  инпута текста из таблиц(для select)
SinglyLinkedList<string> DataBase::textInFile(SinglyLinkedList<Where>& conditions) { 
    string filepath;
    SinglyLinkedList<string> tables;
    for (int i = 0; i < conditions.size; ++i) {
        string filetext;
        int index = nametables.getindex(conditions.getvalue(i).table);
        int iter = 0;
        do {
            iter++;
            filepath = "../" + nameBD + '/' + conditions.getvalue(i).table + '/' + to_string(iter) + ".csv";
            string text = fileinput(filepath);
            int position = text.find('\n'); // удаляем названия столбцов
            text.erase(0, position + 1);
            filetext += text + '\n';
        } while (iter != fileindex.getvalue(index));
        tables.push_back(filetext);
    }
    return tables;
}

// функция input нужных колонок из таблиц для условиястолбец(для select)
SinglyLinkedList<string> DataBase::findStlbTable(SinglyLinkedList<Where>& conditions, SinglyLinkedList<string>& tables, int stlbindexvalnext, string table) { 
    SinglyLinkedList<string> column;
    for (int i = 0; i < conditions.size; ++i) {
        if (conditions.getvalue(i).table == table) {
            stringstream stream(tables.getvalue(i));
            string str;
            while (getline(stream, str)) {
                stringstream istream(str);
                string token;
                int currentIndex = 0;
                while (getline(istream, token, ',')) {
                    if (currentIndex == stlbindexvalnext) {
                        column.push_back(token);
                        break;
                    }
                    currentIndex++;
                }
            }
        }
    }
    return column;
}

// функция выборки(для select)
string DataBase::sample(SinglyLinkedList<int>& stlbindex, SinglyLinkedList<string>& tables) { 
    string result;
    for (int i = 0; i < tables.size - 1; ++i) {
        stringstream onefile(tables.getvalue(i));
        string token;
        while (getline(onefile, token)) {
            string needstlb;
            stringstream ionefile(token);
            int currentIndex = 0;
            while (getline(ionefile, token, ',')) {
                if (currentIndex == stlbindex.getvalue(i)) {
                    needstlb = token;
                    break;
                }
                currentIndex++;
            }
            stringstream twofile(tables.getvalue(i + 1));
            while (getline(twofile, token)) {
                stringstream itwofile(token);
                currentIndex = 0;
                while (getline(itwofile, token, ',')) {
                    if (currentIndex == stlbindex.getvalue(i + 1)) {
                        result += needstlb + ' ' + token + '\n';
                        break;
                    }
                    currentIndex++;
                }
            }
        } 
    }
    return result;
}