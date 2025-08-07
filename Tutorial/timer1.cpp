#include <iostream>
#include <boost/asio.hpp>

int main(){
    // asio를 사용하는 프로그램은 최소 하나의 I/O execution context가 필요
    // context는 특정 목적으로 사용하는 데이터의 집합 
    boost::asio::io_context io;
    
    // IO 기능을 제공하는 asio class들은 첫번째 인자로 실행자가 필요함 
    // (ex.io_context)
    // 두번째 인자 -> expire time 설정 
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));

    t.wait();
    std::cout << "Hello\n";
    return 0;
}
/*
boost asio의 기본적인 timer 구현 
*/