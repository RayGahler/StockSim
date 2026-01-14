#include "StockMarket.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
    // Initialize market with 5 stocks
    StockMarket& market = StockMarket::getInstance(5);
    market.Init();
    market.startMarket();
    
    cout << "Market started! Type 'quit' to exit.\n";
    
    string command;
    while(cin >> command){
        if(command == "quit"){
            market.stopMarket();
            break;
        }
        // TODO: Parse order commands
        cout << "Unknown command: " << command << "\n";
    }
    
    return 0;
}
