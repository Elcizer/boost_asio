# boost_asio
practice boost_asio

실행시에 아래 코드를 통해 boost 설치   
sudo apt update   
sudo apt install libboost-all-dev   


# mmo   
## test1
    단순한 소켓 write / read 수행   
## test2
    write를 수행하고 바로 write를 수행하면   
    서버가 송신한 데이터가 아직 도착하지 않을 수도 있어서
    고의적으로 시간을 지연시킨 후 read를 수행
## test3
    멀티스레드를 이용
    또 하나의 thread가 실행하는 context에 
    async_read를 수행하는 소켓을 연결 시켜
    main과 다른 thread에서 read를 수행