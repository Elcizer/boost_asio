# boost_asio
practice boost_asio

실행시에 아래 코드를 통해 boost 설치   
sudo apt update   
sudo apt install libboost-all-dev   

`boost::thread`를 사용 할 때는
뒤에 옵션으로 -lboost_thread를 붙여야함   
 +혹시 모르니 -lboost_system도 추가하자

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
## DayTime
    boost::asio의 tcp를 통해서 
    daytime 포트(13)를 통해 데이터 송수신
    이걸 기반으로 채팅서버로 확장 시켜보면 될듯