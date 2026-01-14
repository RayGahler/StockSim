#include "Order.h"
#include <unordered_map>
#include <vector>

#ifndef STOCKMARKET
#define STOCKMARKET



class StockMarket{
    public:
        static StockMarket& getInstance(int maxStocks){
            static StockMarket instance(maxStocks);
            return instance;
        }

        void Init();
        int newDay();



    private:
        StockMarket(int maxStocks) : maxStocks(maxStocks){};
        StockMarket(const StockMarket&) = delete;
        StockMarket& operator= (const StockMarket&) = delete;
        
        ~StockMarket();

        bool closed;
        int dayCounter;
        int maxStocks;

        std::unordered_map<std::string, Stock> stockMap;
        std::vector<std::string> stockNames;
        double fullBias;

        //inits
        double getRandomInitPrice(Stock& stock);
        double getRandomInitBias(Stock& stock);
        std::string getRandomName();

        //new Days
        
        double getRandomBias(Stock& stock);
        
        //Each frame
        void getNewPrices();




        


};
#endif