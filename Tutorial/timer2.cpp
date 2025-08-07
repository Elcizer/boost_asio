/*
비동기 타이머
Asynchronous timer
*/

#include <iostream>
#include <boost/asio.hpp>

void print(const boost::system::error_code& e)
{
    std::cout << "Hello World\n";
}



int main(){

    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
    //t에 work를 부여 
    t.async_wait(&print);  
    
    io.run();
    return 0;
}

/*
비동기 IO 사용 -> completion token을 넘김
completion token은 비동기 명령이 완료됐을 때
completion handler에 결과를 전달하는 방식을 결정

이 예제에서는 비동기 명령이 끝났을 때 print 함수를 호출 

asio 라이브러리는 competion handler가 
io_context::run()을 호출하는 스레드에서 호출되는 것을 보장

-> 따라서 run()이 호출되지 않는다해도 
asynchronous wait completion은 invoke되지 않음

io_context에 run()을 실행하기 전에 work를 주어야함
-> 주지 않으면 아무것도 실행하지 않게 됨 (즉시 반환됨)



completion token -> 비동기를 구현하는데 사용 
비동기 명령의 시작 함수를 library에 맞게 설정 
completion token은 비동기 명령의 시작 함수의 마지막 argument

ex) 유저가 completion token으로 람다식을 넘김
-> 명령 시작, 명령이 끝났을때는 람다식에 의해 결과가 넘어감 

1. use_future을 completion token으로 넘김
    명령이 promise와 future 페어와 같이 동작 
    -> 이게 뭔소리야
    시작함수가 실행되고, 결과를 기다리기 위한 future(객체)가 return

2. use_awaitable을 completion token으로 넘김 
    시작함수가 awaitable-based coroutine처럼 동작 
    바로 시작함수가 비동기 명령을 실행하지는 않음
    co_await-ed상태가 됐을때 실행하는 awaitable 객체를 리턴 

3. yield_context를 completion token으로 넘김
    stackful coroutine내에서 동기적으로 명령어를 수행하게 함
    비동기 명령 실행 + 끝날 때 까지 stackful coroutine의 수행을 막음 
    stackful coroutine의 관점에서는 synchonous operation

이 세가지를 통해 async_read_some 시작함수를 형성

*/