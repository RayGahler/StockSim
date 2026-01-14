#include "Order.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>

#pragma once



class StockMarket{
    public:
        static StockMarket& getInstance(int maxStocks){
            static StockMarket instance(maxStocks);
            return instance;
        }

        void Init();
        int newDay();
        void startMarket();
        void stopMarket();
        void placeOrder(Order& order);
        void printPrices();



    private:
        StockMarket(int maxStocks) : maxStocks(maxStocks), dayCounter(0), closed(true){};
        StockMarket(const StockMarket&) = delete;
        StockMarket& operator= (const StockMarket&) = delete;
        

        bool closed;
        int dayCounter;
        int maxStocks;

        std::unordered_map<std::string, Stock> stockMap;
        std::vector<std::string> stockNames;
        double fullBias;
        
        std::mutex marketMutex;
        std::thread marketThread;

        //inits
        double getRandomInitPrice(Stock& stock);
        double getRandomInitBias(Stock& stock);
        std::string getRandomName();

        //new Days
        double getRandomBias(Stock& stock);
        void marketLoop();
        
        //Each frame
        void getNewPrices();




        


};