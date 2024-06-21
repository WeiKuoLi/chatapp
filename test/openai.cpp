#include <iostream>
#include <string>
#include <curl/curl.h>

int main() {
  CURL *curl;
  CURLcode res;

  // Initialize curl
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl) {
    const char* api_key = std::getenv("OPENAI_API_KEY");
    if (!api_key) {
      std::cerr << "Environment variable OPENAI_API_KEY not found" << std::endl;
      return 1;
    }

    // Set up the URL
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");

    // Set up the headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + std::string(api_key);
    headers = curl_slist_append(headers, auth_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set up the POST request
    const char *post_fields = R"({
      "model": "gpt-3.5-turbo",
      "messages": [
        {"role": "system", "content": "You are a poetic assistant, skilled in explaining complex programming concepts with creative flair."},
        {"role": "user", "content": "'"}
      ]
    })";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);

    // Perform the request
    //res = curl_easy_perform(curl);
    // do not print raw response to console
    //res = curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    // res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 
    
    res = curl_easy_perform(curl);
    // Check for errors
    if (res != CURLE_OK) {
      std::cerr << "Curl error: " << curl_easy_strerror(res) << std::endl;
    } else {
      std::cout << "Curl request succeeded" << std::endl;
    }
    
    // extract response JSON  and print outputJSON["choices"][0]["message"]["content"]
        // std::cout << "Response: " << response_json << std::endl;
    // std::cout << "Output: " << output << std::endl;

    // Parse the response as JSON
    // json response_json = json::parse(response);
    // std::string output = response_json["choices"][0]["message"]["content"]
   
     
    // Cleanup curl
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);  // Free the headers list
  }
  curl_global_cleanup();

  return 0;
}

