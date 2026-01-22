#include <string>
#ifndef ORDER
#define ORDER

struct Stock{
    std::string name;

    double maxPrice;
    double minPrice;

    double price;
    double bias;

};


enum OrderType{
    MARKET_ORDER,
    LIMIT_ORDER,
    STOP_ORDER,
    STLIM_ORDER
};

struct Order{
    Stock stock;
    OrderType Type;
    float Amount;
    bool Buy;
    bool inEffect;

    double sellPrice;

    float boughtAt;
    float profit;
};

#endif