#ifndef STOCKMARKET
#define STOCKMARKET
class StockMarket{
    public:
        static StockMarket& getInstance(){
            static StockMarket instance;
            return instance;
        }

        

    private:
        StockMarket() {};
        StockMarket(const StockMarket&) = delete;
        StockMarket& operator= (const StockMarket&) = delete;
        
};
#endif