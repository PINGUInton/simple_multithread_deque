#include <iostream>
#include <thread>
#include <mutex>
#include <deque>
#include <string>
#include <utility>

std::deque<std::pair<std::string, int>> requests;
std::mutex queue_mutex;
bool running = true;

void process_queue() {
    while (running) {
        if(!requests.empty()){    
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            std::unique_lock<std::mutex> lock(queue_mutex);

            std::pair<std::string, int> request;

            request = requests.front();

            if(std::get<1>(request) == 0){    
                std::cout << "Req: " << std::get<0>(request) << " success." << std::endl;
                requests.pop_front();
            } else {
                int new_count = std::get<1>(request);
                new_count -= 1;
                std::get<1>(request) = new_count;
                std::cout << "Req: " << std::get<0>(request) << " time: " << std::get<1>(request) << std::endl;
                requests.pop_front();
                requests.push_back(request); 
            }   
        }   
    }
}

void handle_input(){

    while(true){
        std::string request = "api/v1/";
        std::string input = "";
        
        std::cout << "Enter request (exit: 0): ";
        std::cin >> input;

        if(input == "0"){
            running = false;
            break;
        }

        request += input;

        if(input == "me" || input == "home"){
            std::lock_guard<std::mutex> lock(queue_mutex);
            requests.push_back(std::pair<std::string, int>{request,3});
            std::cout << "Req: " << request << " push to deque." << std::endl;
        }
        else {
            std::cout << "Req: " << request << " success." << std::endl;
        }
    }
}

int main(){
    std::thread queue_thread(process_queue);
    handle_input();
    queue_thread.join();
    
    return 0;
}