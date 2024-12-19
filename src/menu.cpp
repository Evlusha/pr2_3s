#include "../include/menu.h"
#include "../src/comandINSERT.cpp"
#include "../src/comandSELECT.cpp"
#include "../src/comandDEL.cpp"

// функция фильтрации команд
string DataBase::checkcommand(string& command) { 
    if (command.substr(0, 11) == "insert into") {
        command.erase(0, 12);
        return isValidInsert(command);
    } else if (command.substr(0, 11) == "delete from") {
        command.erase(0, 12);
        return isValidDel(command);
    } else if (command.substr(0, 6) == "select") {
        command.erase(0, 7);
        return isValidSelect(command);
    } else if (command == "exit") {
        exit(0);
    } else return "Ошибка, неизвестная команда!"; 
}

// функция подсчёта строк в файле
int CountLine(string& filepath) { 
    ifstream file;
    file.open(filepath);
    int countline = 0;
    string line;
    while(getline(file, line)) {
        countline++;
    }
    file.close();
    return countline;
}

string fileinput(string& filepath) { // чтение из файла
    string result, str;
    ifstream fileinput;
    fileinput.open(filepath);
    while (getline(fileinput, str)) {
        result += str + '\n';
    }
    result.pop_back();
    fileinput.close();
    return result;
}

void fileoutput(string& filepath, string text) { // запись в файл
    ofstream fileoutput;
    fileoutput.open(filepath);
    fileoutput << text;
    fileoutput.close();
}

//парсинг
void DataBase::parse() { 
    nlohmann::json objJson;
    ifstream fileinput;
    fileinput.open("../schema.json");
    fileinput >> objJson;
    fileinput.close();

    if (objJson["names"].is_string()) {
    nameBD = objJson["names"]; // Парсим каталог 
    } else {
        cout << "Объект каталога не найден!" << endl;
        exit(-1);
    }

    tupleslimit = objJson["tuples_limit"];

    // парсим подкаталоги
    if (objJson.contains("structure") && objJson["structure"].is_object()) { // проверяем, существование объекта и является ли он объектом
        for (auto elem : objJson["structure"].items()) {
            nametables.push_back(elem.key());
            
            string kolonki = elem.key() + "_pk_sequence,"; // добавление первичного ключа
            for (auto str : objJson["structure"][elem.key()].items()) {
                kolonki += str.value();
                kolonki += ',';
            }
            kolonki.pop_back(); // удаление последней запятой
            stlb.push_back(kolonki);
            fileindex.push_back(1);
        }
    } else {
        cout << "Объект подкаталогов не найден!" << endl;
        exit(-1);
    }
}

// функция формирования директории
void DataBase::mkdir() { 
    string command;
    command = "mkdir ../" + nameBD; // каталог
    system(command.c_str());

    for (int i = 0; i < nametables.size; ++i) { // подкаталоги и файлы в них
        command = "mkdir ../" + nameBD + "/" + nametables.getvalue(i);
        system(command.c_str());
        string filepath = "../" + nameBD + "/" + nametables.getvalue(i) + "/1.csv";
        ofstream file;
        file.open(filepath);
        file << stlb.getvalue(i) << endl;
        file.close();

        // Блокировка таблицы
        filepath = "../" + nameBD + "/" + nametables.getvalue(i) + "/" + nametables.getvalue(i) + "_lock.txt";
        file.open(filepath);
        file << "open";
        file.close();

        // ключ
        filepath = "../" + nameBD + "/" + nametables.getvalue(i) + "/" + nametables.getvalue(i) + "_pk_sequence.txt";
        file.open(filepath);
        file << "1";
        file.close();
    }
}

