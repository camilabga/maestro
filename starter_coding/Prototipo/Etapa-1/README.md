# Etapa 1
Nesta etapa é utilizado um código em c++, que utiliza a biblioteca cURL para enviar informações para o ESP8266 através de um GET.
Há um código exemplo que executa um GET simples, e outro que faz a cada ciclo de um intervalo de 0.2s.
* [GET simples](https://github.com/camilabga/maestro/blob/master/Prot%C3%B3tipo/Etapa-1/GET_c%2B%2B/main.cpp)
* [GET em loop](https://github.com/camilabga/maestro/blob/master/Prot%C3%B3tipo/Etapa-1/GET_c%2B%2B/loop.cpp)
* [Código para ESP8266](https://github.com/camilabga/maestro/blob/master/Prot%C3%B3tipo/Etapa-1/espCode/espCode/espCode.ino)

# Passos para utilizar a libcurl no CodeBlocks

1. Abra o terminal e digite:

```
sudo apt-get install libcurl4-openssl-dev
```
2. No CodeBlocks vá em Settings -> Compiler -> Linker settings -> Other linker options, digite `-lcurl` e aperte Ok.

# Links

[Documentação ESP8266](http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html)

[Documentação libcurl](https://curl.haxx.se/libcurl/c/allfuncs.html)
