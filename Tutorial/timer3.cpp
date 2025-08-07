#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
// handler function에 추가적인 파라미터를 넘기는 방법

// 1초마다 타이머가 수행되게 하려고함
// 1초마다 타이머가 expire되고 completion handler는 다시 타이머를 수행해야함.
// completion handler가 timer에 접근할 수 있어야함
// 따라서 print에 timer 포인터와 6번 타이머를 멈추게하는 카운터를 추가 


void print(const boost::system::error_code& e, 
    boost::asio::steady_timer *t,int *count)
{
    if(*count < 5){
        std::cout << *count << "\n";
        ++(*count);
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));

        // async_wait는 handler function을 인자로 받음
        // bind가 print를 additional parameter와 결합하여 전달
        t->async_wait(boost::bind(print,
            boost::asio::placeholders::error, t, count));

    }
    // io_context를 명시적으로 멈추지 않는 이유
    // timer2와 마찬가지로 io_context는 work가 없으면 반환하게 됨.
    // 따라서 6번째때 아무런 명령을 수행하지 않으면 반환
}


int main(){
    boost::asio::io_context io;
    int count =0;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
    t.async_wait(boost::bind(print,
        boost::asio::placeholders::error, &t,&count));
    io.run();
    std::cout << "Final count is "<< count <<"\n";
    return 0;
}


/*
예제의 잘못된 점
std::bind와 boost::bind의 placeholder가 다름
예제에서는
std::bind와 boost::asio::placeholder를 사용해서 
정상적인 동작 X

그래서 작성할때는
1. include <boost/bind/bind.hpp>를 추가하고
2. std::bind() -> boost::bind()로 바꿈 

*/