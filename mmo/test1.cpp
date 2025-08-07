/*
간단한 socket connect 연결 뒤
데이터 송신
데이터 수신
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

int main(){
    
    // 12개의 에러 검출에 사용
    boost::system::error_code ec;

    // unique instace of asio
    boost::asio::io_context context;

    //연결하고 싶은 곳의 주소를 얻음 
    // tcp스타일의 address를 불러옴 
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1",ec),80);

    // 소켓 만들기
    boost::asio::ip::tcp::socket socket(context);

    // 해당 소켓을 통해 위에서 만든 주소에 connect 시도 
    socket.connect(endpoint, ec);

    if(!ec) std::cout << "connected\n"; // 오류 X
    else std::cout << "Failed to connect to address\n" << ec.message() <<"\n";
    // 오류 발생 

    if(socket.is_open()){ // 소켓이 active, a live connection이면 true 반환 
        std::string sRequest = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";
        
            // asio 버퍼를 통해 write,read
            // 인자로는 data의 바이트수, data의 사이즈?
        socket.write_some(boost::asio::buffer(sRequest.data(),sRequest.size()),ec);

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
    // 지금 해당 문장은 이 문장의 기능 보다 이러한 방식을 통해
    // 서버에 데이터를 전달하는 방법을 파악  





    system("pause");
    return 0;
}
