#include <iostream>
#include <curl/curl.h>

using namespace std;

int main(void)
{

    CURL *curl;
    CURLcode res;

    char *url = "http://192.168.43.225/1";  // IP do ESP8266 para acionar o motor


    curl = curl_easy_init();    //Inicia uma sessão libcurl no modo fácil
    if(curl){      //Caso retorne null, não será possivel usar outras funções da libcurl


        curl_easy_setopt(curl, CURLOPT_URL, url);    //Define as opções para o setup fácil

        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
        }
        /* always cleanup */
        curl_easy_cleanup(curl);

    }

    return 0;
}
