#include "StockMarket.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main(){
    // Initialize market with 5 stocks
    
    StockMarket& market = StockMarket::getInstance(5);
    market.Init();
    market.startMarket();
    system("clear");
    // Give market thread time to start printing
    this_thread::sleep_for(chrono::milliseconds(1000));
    
    string command;
    while(cin >> command){
      
        // TODO: Parse buy/sell commands
        // For now just acknowledge the input
        if(command == "1"){
            market.startBuyOrder();
        }
        else if(command == "2"){
            market.startSellOrder();
        }
        else if(command == "0"){
            market.stopMarket();
        }
        cout.flush();
    }
    
    return 0;
}
