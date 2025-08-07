/*
네트워크의 문제점
1. 송수신이 언제 발생할지 모름 (시간))
2. 서버가 어느정도 크기의 데이터를 보냈는지 모름

-> asynchoronous를 통해 어느정도 해결 가능 

1. 시간 -> 다른 스레드에 read를 상시 대기 
2. 버퍼 -> 버퍼가 다 차도 계속해서 받아들일 수 있음
    데이터가 아직 들어있으면 계속해서 출력 

*/


#include <chrono>
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

// 데이터가 얼마나 들어올지 모르므로
// 어느정도 합리적인 크기의 버퍼 생성 
std::vector<char> vBuffer(1024);

// reading을 수행할 function
void GrabSomeData(boost::asio::ip::tcp::socket &socket){ // context가 수행하게 될 function
    // async_ function (비동기 함수)는 콜백함수를 등록
    // thread들은 이렇게 등록된 함수들을 큐잉해서 실행함
    socket.async_read_some(boost::asio::buffer(vBuffer.data(),vBuffer.size()),
        [&](std::error_code ec,std::size_t length){
            if(!ec){
                std::cout << "\n\nRead " <<length<<" bytes\n\n";

                for(int i=0;i<length;i++)
                    std::cout<<vBuffer[i];
                // 읽은 데이터를 출력한 뒤 계속해서 재귀 호출해서 
                // 지속적으로 데이터를 read하도록 함
                GrabSomeData(socket);
            }
        }
    );
}

int main(){
    
    boost::system::error_code ec;

    boost::asio::io_context context;

    // context는 할 작업이 없으면 바로 exit
    // 따라서 아래에 있는 GrabSomeData()를 수행하기 전까지 context에 fake work를 줌 
    boost::asio::io_context::work idleWork(context);

    // 다른 스레드에서 context의 run을 수행 
    std::thread thrContext = std::thread([&]() {context.run();} );
    // 다른 스레드에서 수행하기 때문에 해당 스레드의 작업이 멈출 필요가 있다 해도 main함수는 멈추지 않음
    // context.run()은 정해진 작업이 모두 끝나면 반환됨.
    // 하지만 위에서 fake work를 부여했기 때문에 계속 수행되고 있음

    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("51.38.81.49",ec),80);

    // context라는 io_context에 연결된 소켓
    // 해당 소켓에서 async.read를 호출하면 
    // socket이 등록된 context에 콜백함수가 설정 
    boost::asio::ip::tcp::socket socket(context);


    socket.connect(endpoint, ec);

    if(!ec) std::cout << "connected\n"; 
    else std::cout << "Failed to connect to address\n" << ec.message() <<"\n";

    if(socket.is_open()){ 

        // 1. write를 수행하기 전에 read를 수행함
        GrabSomeData(socket);


        std::string sRequest = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: david-baar.co.uk\r\n"
            "Connection: close\r\n\r\n";
        
        socket.write_some(boost::asio::buffer(sRequest.data(),sRequest.size()),ec);

        // 2. write 수행 후 main에 긴 딜레이 부여 -> 그동안 read를 출력
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20000ms);

        context.stop();
        if(thrContext.joinable()) thrContext.join();
    } 
    system("pause");
    return 0;
}

/*
네트워크의 문제점
1. 송수신이 언제 발생할지 모름 (시간))
2. 서버가 어느정도 크기의 데이터를 보냈는지 모름

-> asynchoronous를 통해 어느정도 해결 함

*/