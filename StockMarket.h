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
        int increaseTime();
        void startMarket();
        void stopMarket();
        void placeOrder(Order& order);
        void printPrices();

        void startBuyOrder();
        void startSellOrder();

        float money;
        float usableMoney;


    private:
        StockMarket(int maxStocks) : maxStocks(maxStocks), dayCounter(0), closed(true), money(1000.00f), usableMoney(1000.00f){};
        StockMarket(const StockMarket&) = delete;
        StockMarket& operator= (const StockMarket&) = delete;
        

        bool closed;
        int dayCounter;
        int hourCounter;
        int maxStocks;

        bool pause;

        std::unordered_map<std::string, Stock> stockMap;
        std::vector<std::string> stockNames;
        double fullBias;
        std::vector<Order> pendingOrders;
        std::vector<Order> purchasedStocks;


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
        void checkPendingOrders();

        void calcProfit();




        


};