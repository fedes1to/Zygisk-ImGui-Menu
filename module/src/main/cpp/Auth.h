//
// Created by lbert on 2/13/2023.
//

#ifndef ZYGISKPG_AUTH_H
#define ZYGISKPG_AUTH_H

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <list>
#include <json.h>

using namespace nlohmann;

std::string secret = OBFUSCATE("");
std::string aid = OBFUSCATE("");
std::string apikey = OBFUSCATE("");
std::string readBuffer;
std::string jsonresult;

size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* resData) {
    std::string& buf = *static_cast<std::string*>(resData);
    buf.append(ptr, std::next(ptr, nmemb * size));
    return nmemb * size;
}

bool tryAutoLogin() {
    std::string faggot = ""; //here you have to get the working dir, use Application$persistentDataPath()->getString();
    faggot += "/license.key";

    std::string username;
    std::string password;

    LOGE("Trying to enter file: %s", faggot.c_str());
    std::string line;
    std::ifstream file (faggot);
    if (file.is_open()) {
        for (int lineno = 1; getline (file,line) && lineno < 3; lineno++) {
            if (lineno == 1)
            {
                username = line;
            }
            if (lineno == 2) {
                password = line;
            }
        }
        file.close();
    } else {
        return false;
    }
    const char* hwid = ""; // here we need the hwid, use getDeviceUniqueIdentifier()->getChars();

    std::string::size_type i = 0;
    while (i < username.length()) {
        i = username.find('\n', i);
        if (i == std::string::npos) {
            break;
        }
        username.erase(i);
    }

    std::string::size_type i2 = 0;
    while (i2 < password.length()) {
        i2 = password.find('\n', i2);
        if (i2 == std::string::npos) {
            break;
        }
        password.erase(i);
    }

    // auth.gg example

    CURL *handle;
    CURLcode result;
    long http_code;
    curl_global_init(CURL_GLOBAL_ALL);

    // declare handle
    handle = curl_easy_init();

    std::unique_ptr<std::string> httpData(new std::string());

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    std::ostringstream oss;
    oss << "type=login&aid=" << aid << "&apikey=" << apikey << "&secret=" << secret << "&username=" << username.c_str() << "&password=" << password.c_str() << "&hwid=" << hwid;
    std::string var = oss.str();

    curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(handle, CURLOPT_URL, "https://api.auth.gg/v1/");

    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);

    /* pass our list of custom made headers */
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, var.c_str());

    result = curl_easy_perform(handle); /* post away! */

    curl_easy_getinfo (handle, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers); /* free the header list */

    if(result != CURLE_OK)
        LOGE("curl_easy_perform() failed: %s\n", curl_easy_strerror(result));

    if (http_code == 200 && result != CURLE_ABORTED_BY_CALLBACK) {
        json j = json::parse(readBuffer);
        jsonresult = j.dump(1);
        LOGE("%s", jsonresult.c_str());
        if (jsonresult.find("success") != std::string::npos) {
            return true;
        }
    }
    return false;
}

#endif //ZYGISKPG_AUTH_H
