#ifndef ORDER
#define ORDER

enum OrderType{
    MARKER_ORDER,
    LIMIT_ORDER,
    STOP_ORDER,
    STLIM_ORDER
};

struct Order{
    OrderType Type;
    int Price;
    bool Buy;
    bool inEffect;
};

#endif