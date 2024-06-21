#include <iostream>
#include <string>
#include <curl/curl.h>
//json parsing lib
#include "json.hpp"
#include <vector>
using json = nlohmann::json;


namespace openai{
struct message{
    std::string role;
    std::string content;
};

// Callback function to capture response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* writer_data) {
    if (writer_data == nullptr) return 0;
    writer_data->append((char*)contents, size * nmemb);
    return size * nmemb;
};

std::string openai_chat(const std::vector<struct message>& user_input, const std::string& api_key, const std::string& openai_endpoint) {
    CURL *curl;
    CURLcode res;
    std::string completion = "";

    // Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        if (api_key.empty()) {
            std::cerr << "API key is empty" << std::endl;
            return completion;
        }

        // Set up the URL
        curl_easy_setopt(curl, CURLOPT_URL, openai_endpoint.c_str());

        // Set up the headers
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the POST request
        std::string post_fields = "{";
        post_fields += "\"model\": \"gpt-3.5-turbo\",";
        post_fields += "\"messages\": [";
        for (size_t i = 0; i < user_input.size(); ++i) {
            post_fields += "{";
            post_fields += "\"role\": \"" + user_input[i].role + "\",";
            post_fields += "\"content\": \"" + user_input[i].content + "\"";
            post_fields += "}";
            if (i < user_input.size() - 1) {
                post_fields += ",";
            }
        }
        post_fields += "]";
        post_fields += "}";
        
        const char* _post_fields_cc = post_fields.c_str();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _post_fields_cc);

        // Set up the callback function to capture response
        std::string response_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors and print the response
        if (res != CURLE_OK) {
            std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
        } else {
            //std::cout << "Curl request succeeded" << std::endl;
            //std::cout << "Response: " << response_string << std::endl;

            // Parse JSON response
            try {
                json _j = json::parse(response_string);
                completion = _j["choices"][0]["message"]["content"];
            } catch (json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
            } catch (json::type_error& e) {
                std::cerr << "JSON type error: " << e.what() << std::endl;
                std::cerr << "Raw response: " << response_string << std::endl;
            }
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();

    return completion;
}
}

void sanitize(std::string & completion){
    for (size_t i = 0; i < completion.size(); ++i) {
        if (completion[i] == '\"') {
            completion.replace(i, 1, "\\\"");
            i += 1;
        }
        else if (completion[i] == '\'') {
            completion.replace(i, 1, "\'");
            //i += 1;
        }
        else if (completion[i] == '\\') {
            completion.replace(i, 1, "\\\\");
            i += 1;
        }
        else if (completion[i] == '\n') {
            completion.replace(i, 1, "\\n");
            i += 1;
        }
        else if (completion[i] == '\r') {
            completion.replace(i, 1, "\\r");
            i += 1;
        }
        else if (completion[i] == '\t') {
            completion.replace(i, 1, "\\t");
            i += 1;
        }
        else if (completion[i] == '\b') {
            completion.replace(i, 1, "\\b");
            i += 1;
        }
        else if (completion[i] == '\f') {
            completion.replace(i, 1, "\\f");
            i += 1;
        }
        else if(completion[i] == '\0') {
            completion.replace(i, 1, "\\0");
            i += 1;
        }
        
    }
};
int main() {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
        std::cerr << "Environment variable OPENAI_API_KEY not found" << std::endl;
        return 1;
    }
    struct openai::message m0 = {"system", "You are a helpful assistant who gives accurate information."};
    std::vector<struct openai::message> user_input = {m0};

    std::string str_input = "";
    std::string openai_endpoint = "https://api.openai.com/v1/chat/completions";
    //while cin is not empty  
    
    while (true) {
        std::cout << "USER: ";
        //std::string str_input;
        std::getline(std::cin, str_input);  
        if(str_input == "exit"){
            break;
        }
        //std::cout << "USER: " << str_input << std::endl;
        sanitize(str_input);
        struct openai::message m = {"user", str_input};
        user_input.push_back(m);

        std::string completion = openai::openai_chat(user_input, api_key, openai_endpoint);
        if(completion != ""){
            //std::cout << "OPENAI: " << completion << std::endl;
            //output the completion in green text, "OPENAI" in red
            //std::cout << "\033[1; << "OPENAI: " << "\033[1;32m" << completion << "\033[0m" << std::endl;
            std::cout << "\033[1;31m" << "OPENAI: " << "\033[1;32m" << completion << "\033[0m" << std::endl;
            // replace " or ' or ''' in completion with escape character ex: \' or \" or \'''
                        // to prevent json parse error
            sanitize(completion);
            //std::cout << "\033[1;34m" << "OPENAI: " << "\033[1;32m" << completion << "\033[0m" << std::endl;
            user_input.push_back(openai::message{"assistant", completion});
        }
        else{
            std::cout << "Error in openai chat" << completion << std::endl;
        }
    }
    return 0;
}

