#include <iostream>
#include <string>
#include <map>
#include <curl/curl.h>

std::map<std::string, double> exchangeRates;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

void FetchExchangeRates() {
    CURL* curlHandle = curl_easy_init();
    if (curlHandle) {
        std::string responseData;

        curl_easy_setopt(curlHandle, CURLOPT_URL, "https://v6.exchangerate-api.com/v6/e2755990a7cb3ad997891e63/latest/USD");
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseData);

        CURLcode result = curl_easy_perform(curlHandle);

        if (result == CURLE_OK) {
            size_t usdPos = responseData.find("\"USD\":");
            if (usdPos != std::string::npos) {
                exchangeRates["USD"] = 1.0;

                size_t eurPos = responseData.find("\"EUR\":");
                if (eurPos != std::string::npos) {
                    size_t eurEnd = responseData.find(",", eurPos);
                    exchangeRates["EUR"] = stod(responseData.substr(eurPos + 6, eurEnd - eurPos - 6));
                }

                size_t rubPos = responseData.find("\"RUB\":");
                if (rubPos != std::string::npos) {
                    size_t rubEnd = responseData.find(",", rubPos);
                    exchangeRates["RUB"] = stod(responseData.substr(rubPos + 6, rubEnd - rubPos - 6));
                }
            }
        }
        else {
            std::cerr << "Request error: " << curl_easy_strerror(result) << std::endl;
        }

        curl_easy_cleanup(curlHandle);
    }
}

void PrintMenu() {
    std::cout << "\nCurrent exchange rates:\n";
    std::cout << "1. USD = " << exchangeRates["USD"] << std::endl;
    std::cout << "2. EUR = " << exchangeRates["EUR"] << std::endl;
    std::cout << "3. RUB = " << exchangeRates["RUB"] << std::endl;
    std::cout << "4. Exit\n";
    std::cout << "Select an option: ";
}

int main() {
    FetchExchangeRates();

    if (exchangeRates.empty()) {
        std::cerr << "Error: Failed to get exchange rates!" << std::endl;
        return 1;
    }

    while (true) {
        PrintMenu();

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            double amount;
            std::cout << "Enter amount in USD: ";
            std::cin >> amount;
            std::cout << amount << " USD = " << amount * exchangeRates["EUR"] << " EUR\n";
        }
        else if (choice == 2) {
            double amount;
            std::cout << "Enter amount in EUR: ";
            std::cin >> amount;
            std::cout << amount << " EUR = " << amount / exchangeRates["EUR"] << " USD\n";
        }
        else if (choice == 3) {
            double amount;
            std::cout << "Enter amount in RUB: ";
            std::cin >> amount;
            std::cout << amount << " RUB = " << amount / exchangeRates["RUB"] << " USD\n";
        }
        else if (choice == 4) {
            break;
        }
        else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}