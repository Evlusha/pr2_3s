#include "../include/inet.h"

int main() {

    DataBase logistic;

    logistic.parse();
    logistic.mkdir();

    createServer(logistic);
    
    return 0;
}