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


int StockMarket::increaseTime(){
    if(++hourCounter == 24){
        hourCounter = 0;
        newDay();
    }
    return hourCounter;
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
    
    increaseTime();
}

void StockMarket::calcProfit(){
    money = 0;
    for(Order order : this->purchasedStocks){
        float beginningPrice = order.boughtAt * order.Amount;
        float priceNow = order.stock.price * order.Amount;
        order.profit = priceNow - beginningPrice;
        money += priceNow;
    }
}

void StockMarket::printPrices(){
    std::lock_guard<std::mutex> lock(marketMutex);
    
    // Clear screen completely
    
    std::cout << "\033[1J\033[H";

    std::cout << "--- Hour " << hourCounter << " | Day " << dayCounter << "\t(Market Bias: " << std::fixed << std::setprecision(2) << fullBias << ") ---\n\n";
    
    for(std::string stockName : this->stockNames){
        Stock curStock = this->stockMap[stockName];
        std::cout << curStock.name << " | Price: $" << std::fixed << std::setprecision(2) 
                  << curStock.price << " | Bias: " << curStock.bias << "\n";
    }
    
    std::cout << "\n> ";
    std::cout.flush();
}

void StockMarket::checkPendingOrders(){
    std::lock_guard<std::mutex> lock(marketMutex);
    
    for(auto it = pendingOrders.begin(); it != pendingOrders.end(); ){
        Order& order = *it;
        Stock& curStock = stockMap[order.stock.name];
        bool shouldExecute = false;
        double executionPrice = curStock.price;
        
        switch(order.Type){
            case OrderType::LIMIT_ORDER: {
                if(order.Buy){
                    // Buy limit: execute if current price <= limit price
                    if(curStock.price <= order.sellPrice){
                        shouldExecute = true;
                    }
                } else {
                    // Sell limit: execute if current price >= limit price
                    if(curStock.price >= order.sellPrice){
                        shouldExecute = true;
                    }
                }
                break;
            }
            case OrderType::STOP_ORDER: {
                // Stop orders activate when price reaches stop level
                if((order.Buy && curStock.price >= order.sellPrice) || 
                   (!order.Buy && curStock.price <= order.sellPrice)){
                    shouldExecute = true;
                }
                break;
            }
            case OrderType::STLIM_ORDER: {
                // Stop-limit: check if stop is triggered
                bool stopTriggered = (order.Buy && curStock.price >= order.sellPrice) || 
                                     (!order.Buy && curStock.price <= order.sellPrice);
                
                if(stopTriggered){
                    // Now check limit condition (reuse the amount field for limit price temporarily)
                    // Note: You may want to add a limitPrice field to Order struct for clarity
                    shouldExecute = true;
                }
                break;
            }
            default:
                break;
        }
        
        if(shouldExecute){
            if(order.Buy){
                order.boughtAt = executionPrice;
                purchasedStocks.push_back(order);
            }
            std::cout << "\n✓ Pending Order EXECUTED\n";
            std::cout << (order.Buy ? "BUY" : "SELL") << " " << (int)order.Amount << " shares of " 
                      << order.stock.name << " at $" << std::fixed << std::setprecision(2) 
                      << executionPrice << "\n";
            std::cout << "Total value: $" << std::fixed << std::setprecision(2) 
                      << (order.Amount * executionPrice) << "\n\n";
            it = pendingOrders.erase(it);
        } else {
            ++it;
        }
    }
}

void StockMarket::marketLoop(){
    while(!closed && !pause){
        getNewPrices();
        checkPendingOrders();
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
    
    switch(order.Type){
        case OrderType::MARKET_ORDER: {
            // Market orders execute immediately at current price
            double executionPrice = order.stock.price;
            if(order.Buy){
                order.boughtAt = executionPrice;
                purchasedStocks.push_back(order);
            }
            std::cout << "\n✓ Order EXECUTED\n";
            std::cout << (order.Buy ? "BUY" : "SELL") << " " << (int)order.Amount << " shares of " 
                      << order.stock.name << " at $" << std::fixed << std::setprecision(2) 
                      << executionPrice << "\n";
            std::cout << "Total value: $" << std::fixed << std::setprecision(2) 
                      << (order.Amount * executionPrice) << "\n\n";
            break;
        }
        case OrderType::LIMIT_ORDER: {
            // Limit orders are checked every frame
            std::cout << "Enter limit price: $";
            double limitPrice;
            std::cin >> limitPrice;
            order.sellPrice = limitPrice;
            order.inEffect = true;
            pendingOrders.push_back(order);
            std::cout << "○ LIMIT order for " << order.stock.name << " placed\n";
            std::cout << "  Will execute when price reaches $" << std::fixed << std::setprecision(2) 
                      << limitPrice << "\n\n";
            break;
        }
        case OrderType::STOP_ORDER: {
            // Stop orders are checked every frame
            std::cout << "Enter stop price: $";
            double stopPrice;
            std::cin >> stopPrice;
            order.sellPrice = stopPrice;
            order.inEffect = true;
            pendingOrders.push_back(order);
            std::cout << "○ STOP order for " << order.stock.name << " placed\n";
            std::cout << "  Will execute when price reaches $" << std::fixed << std::setprecision(2) 
                      << stopPrice << "\n\n";
            break;
        }
        case OrderType::STLIM_ORDER: {
            // Stop-limit orders are checked every frame
            std::cout << "Enter stop price: $";
            double stopPrice;
            std::cin >> stopPrice;
            std::cout << "Enter limit price: $";
            double limitPrice;
            std::cin >> limitPrice;
            order.sellPrice = stopPrice;  // Using sellPrice for stop, you may want to add limitPrice field
            order.inEffect = true;
            pendingOrders.push_back(order);
            std::cout << "○ STOP-LIMIT order for " << order.stock.name << " placed\n";
            std::cout << "  Stop: $" << std::fixed << std::setprecision(2) << stopPrice 
                      << " | Limit: $" << limitPrice << "\n\n";
            break;
        }
    }
}

void StockMarket::startBuyOrder(){
    std::lock_guard<std::mutex> lock(marketMutex);

    Order order;
    order.Buy = true;


    std::cout << "What stock would you like to buy" << std::endl;
    std::string name;
    std::cin >> name;

    while(stockMap.find(name) == stockMap.end()){
        std::cout << "That stock does not exist, try again" << std::endl;
        std::cin >> name;
    }
    Stock selected = stockMap[name];

    order.stock = selected;

    system("clear");

    std::cout << selected.name << " | Price: $" << std::fixed << std::setprecision(2) 
                  << selected.price << " | Bias: " << selected.bias << "\n";

    std::cout << "What type of order?\n1) Market\n2) Limit\n3) Stop\n4) Stop-Limit\n";

    int orderChoice;
    std::cin >> orderChoice;
    
    while(!std::cin || (orderChoice > 4 && orderChoice <= 0)){
        std::cout << "Invalid\nTry again\n";
        std::cin >> orderChoice;
    }

    int amount;
    std::cout << "How much\n";

    std::cin >> amount;

    order.Amount = amount;
    
    if(orderChoice == 1){
        order.Type = OrderType::MARKET_ORDER;
    }
    else if(orderChoice == 2){
        order.Type = OrderType::LIMIT_ORDER;
    }
    else if(orderChoice == 3){
        order.Type = OrderType::STOP_ORDER;
    }
    else{
        order.Type = OrderType::STLIM_ORDER;
    }


    placeOrder(order);


    

}

void StockMarket::startSellOrder(){
    std::lock_guard<std::mutex> lock(marketMutex);
    
    if(purchasedStocks.empty()){
        std::cout << "You don't own any stocks to sell\n";
        return;
    }
    
    system("clear");
    std::cout << "Your Portfolio:\n";
    for(int i = 0; i < purchasedStocks.size(); i++){
        Order& holding = purchasedStocks[i];
        double currentPrice = stockMap[holding.stock.name].price;
        double profit = (currentPrice - holding.boughtAt) * holding.Amount;
        double profitPercent = ((currentPrice - holding.boughtAt) / holding.boughtAt) * 100;
        
        std::cout << i+1 << ") " << holding.stock.name << " | Shares: " << (int)holding.Amount 
                  << " | Bought at: $" << std::fixed << std::setprecision(2) << holding.boughtAt
                  << " | Current: $" << currentPrice << " | P/L: $" << profit 
                  << " (" << profitPercent << "%)\n";
    }
    
    std::cout << "\nWhich stock would you like to sell? (enter number): ";
    int choice;
    std::cin >> choice;
    
    while(choice < 1 || choice > purchasedStocks.size()){
        std::cout << "Invalid choice, try again: ";
        std::cin >> choice;
    }
    
    Order& selected = purchasedStocks[choice - 1];
    
    std::cout << "\nSelling " << selected.stock.name << "\n";
    std::cout << "What type of order?\n1) Market\n2) Limit\n3) Stop\n4) Stop-Limit\n";
    
    int orderChoice;
    std::cin >> orderChoice;
    
    while(!std::cin || (orderChoice > 4 || orderChoice <= 0)){
        std::cout << "Invalid\nTry again\n";
        std::cin >> orderChoice;
    }
    
    Order sellOrder;
    sellOrder.Buy = false;
    sellOrder.stock = stockMap[selected.stock.name];
    sellOrder.Amount = selected.Amount;
    
    if(orderChoice == 1){
        sellOrder.Type = OrderType::MARKET_ORDER;
    }
    else if(orderChoice == 2){
        sellOrder.Type = OrderType::LIMIT_ORDER;
    }
    else if(orderChoice == 3){
        sellOrder.Type = OrderType::STOP_ORDER;
    }
    else{
        sellOrder.Type = OrderType::STLIM_ORDER;
    }
    
    placeOrder(sellOrder);
    
    // Remove from portfolio after selling
    purchasedStocks.erase(purchasedStocks.begin() + (choice - 1));
}