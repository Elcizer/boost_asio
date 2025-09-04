#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class printer{
    public:
        printer(boost::asio::io_context& io) : timer_(io,boost::asio::chrono::seconds(1)), count_(0)
        {
            // 모든 non-static 멤버 함수에는 암시적 this 파라미터가 있어서
            // 이를 bind()를 이용해서 print()를 만들어야함 
            timer_.async_wait(boost::bind(&printer::print,this));
        }
        ~printer()
        {
            std::cout << "Final count is " << count_ << "\n"; 
        }
        void print()
        {
            if(count_<5)
            {
                std::cout << count_ <<"\n";
                ++count_;

                timer_.expires_at(timer_.expiry()+boost::asio::chrono::seconds(1));
                timer_.async_wait(boost::bind(&printer::print,this));
            }
        }
    private:
        boost::asio::steady_timer timer_;
        int count_;
};

int main()
{
    boost::asio::io_context io;
    printer p(io);
    io.run();

    return 0;
}