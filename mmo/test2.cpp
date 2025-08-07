/*
간단한 socket connect 연결 뒤
데이터 송신
데이터 수신
*/


#include <chrono>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

// 데이터가 얼마나 들어올지 모르므로
// 어느정도 합리적인 크기의 버퍼 생성 
std::vector<char> vBuffer(20 * 1024);

// reading을 수행할 function

int main(){
    
    boost::system::error_code ec;

    boost::asio::io_context context;

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1",ec),80);

    boost::asio::ip::tcp::socket socket(context);


    socket.connect(endpoint, ec);

    if(!ec) std::cout << "connected\n"; 
    else std::cout << "Failed to connect to address\n" << ec.message() <<"\n";

    if(socket.is_open()){ 
        std::string sRequest = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";
        
        socket.write_some(boost::asio::buffer(sRequest.data(),sRequest.size()),ec);

        // socket에 읽을 수 있는 데이터가 들어올 떄 까지 스레드를 멈춤
        // 대신 의도된대로 바이트를 받지는 않음
        // transport delay에 의해서 읽는 데이터의 양이 계속 바뀔 수 있음

        socket.wait(socket.wait_read);
        
        // read 가능한 바이트 수 반환
        size_t bytes = socket.available();
        std::cout << "Bytes Avalilabe: " << bytes <<"\n";

        if(bytes > 0){
            std::vector<char> vBuffer(bytes);
            socket.read_some(boost::asio::buffer(vBuffer.data(),vBuffer.size()),ec);
        
            for(auto c:vBuffer){
                std::cout<<c;
            }   
            std::cout<<"\n";
        }

    } 
    system("pause");
    return 0;
}

/*
네트워크의 문제점
1. 송수신이 언제 발생할지 모름 (시간))
2. 서버가 어느정도 크기의 데이터를 보냈는지 모름

-> asynchoronous를 통해 어느정도 해결 가능 

*/