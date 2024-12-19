#include "../include/menu.h"


// функция обработки команды DELETE

string DataBase::isValidDel(string& command) { 
    string table, conditions;
    int position = command.find_first_of(' ');
    if (position != -1) {
        table = command.substr(0, position);
        conditions = command.substr(position + 1);
    } else table = command;
    if (nametables.getindex(table) != -1) { // проверка таблицы
        if (conditions.empty()) { // если нет условий, удаляем все
            return del(table);
        } else {
            if (conditions.substr(0, 6) == "where ") { // проверка наличия where
                conditions.erase(0, 6);
                SinglyLinkedList<Where> cond;
                Where where;
                position = conditions.find_first_of(' '); ////
                if (position != -1) { // проверка синтаксиса
                    where.column = conditions.substr(0, position);
                    conditions.erase(0, position+1);
                    int index = nametables.getindex(table);
                    string str = stlb.getvalue(index);
                    stringstream ss(str);
                    bool check = false;
                    while (getline(ss, str, ',')) if (str == where.column) check = true;
                    if (check) { // проверка столбца
                        if (conditions[0] == '=' && conditions[1] == ' ') { // проверка синтаксиса
                            conditions.erase(0, 2);
                            position = conditions.find_first_of(' ');
                            if (position == -1) { // если нет лог. оператора
                                where.value = conditions;
                                return delWithValue(table, where.column, where.value);
                            } else { // если есть логический оператор
                                where.value = conditions.substr(0, position);
                                conditions.erase(0, position+1);
                                cond.push_back(where);
                                position = conditions.find_first_of(' ');
                                if ((position != -1) && (conditions.substr(0, 2) == "or" || conditions.substr(0, 3) == "and")) {
                                    where.logicalOP = conditions.substr(0, position);
                                    conditions.erase(0, position + 1);
                                    position = conditions.find_first_of(' ');
                                    if (position != -1) {
                                        where.column = conditions.substr(0, position);
                                        conditions.erase(0, position+1);
                                        index = nametables.getindex(table);
                                        str = stlb.getvalue(index);
                                        stringstream iss(str);
                                        bool check = false;
                                        while (getline(iss, str, ',')) if (str == where.column) check = true;
                                        if (check) { // проверка столбца
                                            if (conditions[0] == '=' && conditions[1] == ' ') { // проверка синтаксиса
                                                conditions.erase(0, 2);
                                                position = conditions.find_first_of(' ');
                                                if (position == -1) {
                                                    where.value = conditions;
                                                    cond.push_back(where);
                                                    return delWithLogic(cond, table);
                                                } else return "Ошибка, нарушен синтаксис команды!";
                                            } else return "Ошибка, нарушен синтаксис команды!";
                                        } else return "Ошибка, нет такого столбца!";
                                    } else return "Ошибка, нарушен синтаксис команды!";
                                } else return "Ошибка, нарушен синтаксис команды!";
                            }
                        } else return "Ошибка, нарушен синтаксис команды!";
                    } else return "Ошибка, нет такого столбца!";
                } else return "Ошибка, нарушен синтаксис команды!";
            } else return "Ошибка, нарушен синтаксис команды!";
        }
    } else return "Ошибка, нет такой таблицы!";
}


// функция удаления всех строк таблицы
string DataBase::del(string& table) { 
    string filepath;
    int index = nametables.getindex(table);
    if (checkLockTable(table)) {
        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "close");
        
        // очищаем все файлы
        int copy = fileindex.getvalue(index);
        while (copy != 0) {
            filepath = "../" + nameBD + "/" + table + "/" + to_string(copy) + ".csv";
            fileoutput(filepath, "");
            copy--;
        }

        fileoutput(filepath, stlb.getvalue(index)+"\n"); // добавляем столбцы в 1.csv

        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "open");
        return "Команда выполнена!";
    } else return "Ошибка, таблица используется другим пользователем!";
}

//функция  удаления строк таблицы по значению
string DataBase::delWithValue(string& table, string& stolbec, string& values) { 
    string filepath;
    int index = nametables.getindex(table);
    if (checkLockTable(table)) {
        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "close");

        // нахождение индекса столбца в файле
        string str = stlb.getvalue(index);
        stringstream ss(str);
        int stolbecindex = 0;
        while (getline(ss, str, ',')) {
            if (str == stolbec) break;
            stolbecindex++;
        }

        // удаление строк
        int copy = fileindex.getvalue(index);
        while (copy != 0) {
            filepath = "../" + nameBD + "/" + table + "/" + to_string(copy) + ".csv";
            string text = fileinput(filepath);
            stringstream stroka(text);
            string filteredlines;
            while (getline(stroka, text)) {
                stringstream iss(text);
                string token;
                int currentIndex = 0;
                bool shouldRemove = false;
                while (getline(iss, token, ',')) {
                    if (currentIndex == stolbecindex && token == values) {
                        shouldRemove = true;
                        break;
                    }
                    currentIndex++;
                }
                if (!shouldRemove) filteredlines += text + "\n"; 
            }
            fileoutput(filepath, filteredlines);
            copy--;
        }

        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "open");
        return "Команда выполнена!";
    } else return "Ошибка, таблица используется другим пользователем!";
}

// функция  удаления строк таблицы с логикой
string DataBase::delWithLogic(SinglyLinkedList<Where>& conditions, string& table) { 
    string filepath;
    int index = nametables.getindex(table);
    if (checkLockTable(table)) {
        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "close");

        // нахождение индекса столбцов в файле
        SinglyLinkedList<int> stlbindex;
        for (int i = 0; i < conditions.size; ++i) {
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

        // удаление строк
        int copy = fileindex.getvalue(index);
        while (copy != 0) {
            filepath = "../" + nameBD + "/" + table + "/" + to_string(copy) + ".csv";
            string text = fileinput(filepath);
            stringstream stroka(text);
            string filteredRows;
            while (getline(stroka, text)) {
                SinglyLinkedList<bool> shouldRemove;
                for (int i = 0; i < stlbindex.size; ++i) {
                    stringstream iss(text);
                    string token;
                    int currentIndex = 0;
                    bool check = false;
                    while (getline(iss, token, ',')) { 
                        if (currentIndex == stlbindex.getvalue(i) && token == conditions.getvalue(i).value) {
                            check = true;
                            break;
                        }
                        currentIndex++;
                    }
                    if (check) shouldRemove.push_back(true);
                    else shouldRemove.push_back(false);
                }
                if (conditions.getvalue(1).logicalOP == "and") { // Если оператор И
                    if (shouldRemove.getvalue(0) && shouldRemove.getvalue(1));
                    else filteredRows += text + "\n";
                } else { // Если оператор ИЛИ
                    if (!(shouldRemove.getvalue(0)) && !(shouldRemove.getvalue(1))) filteredRows += text + "\n";
                }
            }
            fileoutput(filepath, filteredRows);
            copy--;
        }

        filepath = "../" + nameBD + "/" + table + "/" + table + "_lock.txt";
        fileoutput(filepath, "open");
        return "Команда выполнена!";
    } else return "Ошибка, таблица используется другим пользователем!";
}
