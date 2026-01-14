#include "StockMarket.h"
#include <ctime>

#define CHAR_MIN 65
#define CHAR_MAX 90

void StockMarket::Init(){
    srand(time(0));
    for(int i = 0; i < this->maxStocks; i++){
        int randPrice = rand() % 100000;
        double maxPrice = randPrice / 100;
        Stock s;
        s.maxPrice = maxPrice;
        s.minPrice = maxPrice * .05;
        std::string name = this->getRandomName();

        s.name = name;
        s.price = getRandomInitPrice(s);
        s.bias = getRandomInitBias(s);

        this->stockMap[name] = s;

        this->stockNames.push_back(name);
        
    }
}

StockMarket::~StockMarket(){
}

int StockMarket::newDay(){
    for(std::string s : this->stockNames){
        Stock curStock = this->stockMap[s];
        int curStockIntBias = (int)(curStock.bias*100);

        double increase = rand() % curStockIntBias;
        increase++;

        increase -= curStockIntBias / 2;
        increase /= 100;

        curStock.bias += increase;
        if(curStock.bias <= .1){
            curStock.bias == .1;

        }
        else if(curStock.bias >= .95){
            curStock.bias = .95;
        }
    }

    return ++dayCounter;
}

double StockMarket::getRandomInitPrice(Stock& stock){
    double min = stock.minPrice;
    double max = stock.maxPrice;

    int randPrice = rand() % ((int)max - (int)min);

    return min + randPrice;
}

double StockMarket::getRandomInitBias(Stock& stock){
    int randomBias = rand() % 100;
    return randomBias / 100;
}

std::string StockMarket::getRandomName(){
    char name[4];
    

    for(int i = 0; i < 3; i++){
        int randNumber = rand() % (CHAR_MAX - CHAR_MIN + 1) + CHAR_MIN;
        name[i] = static_cast<char>(randNumber);
        
    }
    name[3] = '\0';

    return name;
}