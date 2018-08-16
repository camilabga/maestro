/*
    Luiz Felipe Santana
    Código com loop infinito
    É definido um intervalo, e cada ciclo deste intervalo é realizado um GET.

*/


#include<iostream>
#include <curl/curl.h>
#include <ctime>

#define INTERVAL_SEC 0.2

using namespace std;

int get(char *url){

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();    //Inicia uma sessão libcurl no modo fácil
    if(curl){     //Caso retorne null, não será possivel usar outras funções da libcurl

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


int main(){

    double start = clock()/ static_cast<double>( CLOCKS_PER_SEC );  //Armazena o tempo inicial
    //char *url = "http://192.168.43.225/1";  // IP do ESP8266 para acionar o motor
    char *url = "http://192.168.0.1/0001";  // IP do ESP8266 para acionar o motor

    char temp[] = "http://192.168.0.29/0001";
    //temp[20] = '2';
    //url = temp;
    cout<<url;
    while(true){

        double timeSec = (clock()) / static_cast<double>( CLOCKS_PER_SEC );
        if(timeSec-start > INTERVAL_SEC){
            start = clock()/ static_cast<double>( CLOCKS_PER_SEC );
            cerr<<"Time: "<<timeSec<<endl;
            get(temp);
        }

    }
}
