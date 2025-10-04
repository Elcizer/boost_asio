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
## Chatting 09/09
    boost::asio를 통해 기본적인 에코 프로그램 작성
    현재는 서버만 비동기적으로 작동
    클라이언트는 에코 클라이언트와 동일한 기능을 수행
    이름 설정 단계를 추가해서 서버의 로그에서 각 클라이언트를 구분할 수 있게 만듬
## Chatting 09/13
    클라이언트의 read/write를 서브스레드를 두어 동시에 실행할 수 있게함
    아직 정상작동하는지 확인은 못함 (서버에서 여러 클라이언트로의 전송 구현 X)
## Chatting 09/19
    만약 채팅을 입력하다가 끊기면 그 내용이 버퍼에 저장되고
    새로 입력한 내용의 뒤에 붙여서 전송되는 현상이 있었음.   

    우선 1. 표준 입출력 스트림, 2. 소켓의 입출력 스트림 
    두 가지에 대해서 기존 입력 내용이 저장되었는지 확인함
    -> 두 스트림을 clear하는 방식으로 확인해봤지만 문제가 해결되지 않음   
    
    그 이후에 더 찾아본 결과 OS의 입출력 스트림에 저장될 수 있다는 사실을 알았음.
    따라서 리눅스의 os의 입출력과 관련된 termios 라이브러리를 통해서
    OS의 입력스트림을 flush 해보니 문제가 해결됨.
    기존 getline()이 끊기면 OS의 입력스트림에 기존 내용이 저장된다는 것을 알 수 있었음.

