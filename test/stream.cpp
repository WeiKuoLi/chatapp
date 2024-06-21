#include <iostream>
#include <curl/curl.h>
#include <cstdlib>  // For getenv
#include <string>
#include "json.hpp"
#include <vector>
using json = nlohmann::json;

// Utility function to trim whitespace and delimiters from the start and end of a string
std::string trim(const std::string& str)
{
    const std::string whitespace = " \n\r\t\f\v";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);

    if (start == std::string::npos || end == std::string::npos)
    {
        return ""; // string is all whitespace
    }
    return str.substr(start, end - start + 1);
}

// Callback function to handle the response data in streaming mode
size_t _WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string data((char*)contents, totalSize);

    // Output received chunk size and data
    //std::cout << "Received chunk of size: " << totalSize << std::endl;
    //std::cout << "Received data chunk: " << data << std::endl;
    //parse the json data
    std::cout << data << std::endl;
    json j = json::parse(data.substr(6, data.size()-6));
    //data: {"id":"chatcmpl-9cT3Hj3cnh5inekaNP5jpZDhCgrpI","object":"chat.completion.chunk","created":1718955167,"model":"gpt-3.5-turbo-0125","system_fingerprint":null,"choices":[{"index":0,"delta":{"content":"?"},"logprobs":null,"finish_reason":null}]}
    std::string reply= j["choices"][0]["delta"]["content"];
    std::cout << reply;

    return totalSize;
}

std::vector<std::string> split(const std::string& s, const std::string& delim) {
    std::vector<std::string> result{};
    if (s.size() == 0) return result;
    if (delim.size() == 0) {
        result.push_back(s);
        return result;
    }
    size_t start = 0, end = 0;
    while ((end = s.find(delim, start)) != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + delim.size();
    }
    result.push_back(s.substr(start));
    return result;
}

// Callback function to handle the response data in streaming mode
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string data((char*)contents, totalSize);
     // data is formatted as {JSON} data: {JSON} data: {JSON} ...
    std::string::size_type prefixPos = data.find("data: ");
    
    std::vector<std::string> chunks = split(data, "data: ");
    for (auto& chunk: chunks){
        std::cout << "**"<<chunk<<"**" << std::endl;
    }
    for (auto& chunk : chunks) {
        if (chunk.size() == 0) continue;
        std::string data = trim(chunk);
        // Output received chunk size and data
        //std::cout << "Received chunk of size: " << totalSize << std::endl;
        //std::cout << "Received data chunk: " << data << std::endl;
        //parse the json data
        std::cout << data << std::endl;
        try{
            json j = json::parse(data);
            std::string reply= j["choices"][0]["delta"]["content"];
            //std::cout << "\033[1;32m **" << data << "** \033[0m" << std::endl;
            std::cout << "\033[1;31m" << reply << "\033[0m" << std::endl;
        }catch (json::parse_error& e){
            continue;
        }
        
        
        //if parse err
        
        //data: {"id":"chatcmpl-9cT3Hj3cnh5inekaNP5jpZDhCgrpI","object":"chat.completion.chunk","created":1718955167,"model":"gpt-3.5-turbo-0125","system_fingerprint":null,"choices":[{"index":0,"delta":{"content":"?"},"logprobs":null,"finish_reason":null}]}
    }

    return totalSize;
}


int main()
{
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();

    if (curl)
    {
        // Retrieve API key from environment variable
        const char* api_key = std::getenv("OPENAI_API_KEY");
        if (api_key == nullptr)
        {
            std::cerr << "Error: OPENAI_API_KEY environment variable not set." << std::endl;
            return 1;
        }

        std::string endpoint = "https://api.openai.com/v1/chat/completions";

        // Set the request data (example prompt for completion)
        std::string post_data = R"({"model": "gpt-3.5-turbo", "messages": [{"role": "user", "content": "Hello!"}], "stream": true})";

        // Set the request headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + std::string(api_key)).c_str());

        // Set curl options
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // Enable streaming with a suitable buffer size
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 256); // Buffer size for streamed data
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects if any

        std::cout << "Sending request..." << std::endl;
        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Error handling
        if (res != CURLE_OK)
        {
            std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
        }
        //std::cout << std::endl << "<Response received.>" << std::endl;

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Error: Failed to initialize CURL." << std::endl;
    }

    curl_global_cleanup();

    return 0;
}

