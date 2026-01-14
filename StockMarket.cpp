#include "StockMarket.h"
#include <ctime>
#include <iostream>
#include <iomanip>

#define CHAR_MIN 65
#define CHAR_MAX 90

void StockMarket::Init(){
    srand(time(0));
    fullBias = 0.5;  // Initialize market bias
    
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


int StockMarket::newDay(){
    for(std::string stockName : this->stockNames){
        Stock& curStock = this->stockMap[stockName];
        int curStockIntBias = (int)(curStock.bias*100);
        if(curStockIntBias < 1) curStockIntBias = 1;  // Prevent division by zero

        double increase = rand() % curStockIntBias;
        increase++;

        increase -= curStockIntBias / 2.0;
        increase /= 100;

        curStock.bias += increase;
        if(curStock.bias <= .1){
            curStock.bias = .1;
        }
        else if(curStock.bias >= .95){
            curStock.bias = .95;
        }
    }

    int curFullIntBias = (int)(fullBias * 100);
    if(curFullIntBias < 1) curFullIntBias = 1;  // Prevent division by zero

    double increase = rand() % curFullIntBias;
    increase++;

    increase -= curFullIntBias / 2.0;
    increase /= 100;

    fullBias += increase;
    if(fullBias <= .3){
        fullBias = .3;
    }
    else if(fullBias >= .75){
        fullBias = .75;
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
    double bias = randomBias / 100.0;  // Use 100.0 for floating point division
    return (bias < 0.1) ? 0.1 : bias;  // Ensure minimum bias of 0.1
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

/*
ex values

fb = .6 -> should be dampener / amplifier
sb = .4 -> intensity of change, below .5 is higher losses, vice versa for above .5

fb * sb = .24

*/

void StockMarket::getNewPrices(){
    std::lock_guard<std::mutex> lock(marketMutex);
    
    for(std::string stockName : this->stockNames){
        Stock& curStock = this->stockMap[stockName];
        int curStockIntBias = (int)(curStock.bias * 100);
        
        double priceChange = rand() % curStockIntBias;
        priceChange++;
        priceChange -= curStockIntBias / 2;
        priceChange /= 100;
        
        // Apply fullBias as a dampener/amplifier
        priceChange *= fullBias;
        
        curStock.price += priceChange;
        
        // Clamp price within bounds
        if(curStock.price < curStock.minPrice){
            curStock.price = curStock.minPrice;
        }
        else if(curStock.price > curStock.maxPrice){
            curStock.price = curStock.maxPrice;
        }
    }
    
    newDay();
}

void StockMarket::printPrices(){
    std::lock_guard<std::mutex> lock(marketMutex);
    
    std::cout << "\n--- Day " << dayCounter << " (Market Bias: " << std::fixed << std::setprecision(2) << fullBias << ") ---\n";
    
    for(std::string stockName : this->stockNames){
        Stock curStock = this->stockMap[stockName];
        std::cout << curStock.name << " | Price: $" << std::fixed << std::setprecision(2) 
                  << curStock.price << " | Bias: " << curStock.bias << "\n";
    }
}

void StockMarket::marketLoop(){
    while(!closed){
        getNewPrices();
        printPrices();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void StockMarket::startMarket(){
    closed = false;
    marketThread = std::thread(&StockMarket::marketLoop, this);
}

void StockMarket::stopMarket(){
    closed = true;
    if(marketThread.joinable()){
        marketThread.join();
    }
}

void StockMarket::placeOrder(Order& order){
    std::lock_guard<std::mutex> lock(marketMutex);
    
    // TODO: Implement order matching logic
    if(order.Buy){
        std::cout << "Buy order placed for " << order.stock.name 
                  << " at $" << std::fixed << std::setprecision(2) << order.stock.price << "\n";
    } else {
        std::cout << "Sell order placed for " << order.stock.name 
                  << " at $" << std::fixed << std::setprecision(2) << order.stock.price << "\n";
    }
}