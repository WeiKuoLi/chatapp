#include  <linenoise.h>
#include <iostream>
#include  <curl/curl.h>

int main() {

    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Test curl
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.example.com");
        CURLcode res = curl_easy_perform(curl);
        std::cout << "Curl result: " << res << std::endl;

        // Cleanup curl
        curl_easy_cleanup(curl);
    }

    //test linenoise
    char *line;
    linenoiseHistoryLoad("history.txt");
    while((line = linenoise("prompt>")) != NULL) {
        if (line[0] != '\0') {
            //std::cout<<std::endl<<"echo: "<<line;
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("history.txt");
        }
        free(line);
    }
    return 0;
    
}
