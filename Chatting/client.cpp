#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>


using boost::asio::ip::tcp;



class tcp_connection{
    public:
        tcp_connection(boost::asio::io_context &ic,char *ip, char *port)
            :resolver_(ic),socket_(ic),isclose(false)
        {
            endpoint_ = resolver_.resolve(ip,port);
            boost::asio::connect(socket_,endpoint_);
            std::cout << "Connect Success\n";
            set_name();
            start_read();
            start_write();
        }
    private:
        void set_name(){
            boost::array<char,32> name;

            std::cout << "Please Type Name :"; 
            std::cin.getline(name.data(),32);
            boost::system::error_code ignored_error;
            socket_.write_some(boost::asio::buffer(name,32),ignored_error);
        }
        void start_read(){
            boost::array<boost::asio::mutable_buffer, 2> buf_= 
                {boost::asio::buffer(name_buf_),boost::asio::buffer(read_buf_)};

            boost::asio::async_read(socket_,buf_,
                boost::bind(&tcp_connection::read_handler,this,boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
        void read_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted){
            if(handle_ec(ec)){
                if(!isclose){
                    std::cout << "서버와의 연결이 종료되었습니다\n엔터를 누르면 프로그램이 종료됩니다.\n";
                    socket_.close();
                    isclose = true;
                }
                return;
            }
            std::cout << name_buf_.data()<<" : " << read_buf_.data() <<"\n";
            start_read();
        }
        void start_write(){
            std::cin.getline(write_buf_.data(),128); // 여기서 메인 스레드 블로킹
            boost::asio::async_write(socket_,boost::asio::buffer(write_buf_),
                boost::bind(&tcp_connection::write_handler,this,
                boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
        }
        void write_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted){
            if(handle_ec(ec)){
                if(!isclose){
                    socket_.close();
                    isclose = true;
                }
                return;
            }
            start_write();
        }
        bool handle_ec(const boost::system::error_code& ec){ // 오류 -> false 
            if(ec == boost::asio::error::eof){ // eof 전송 보통 close() 이용 
                return true;
            }
            else if(ec == boost::asio::error::connection_reset){
                return true;
            }
            else if(ec == boost::asio::error::connection_aborted){
                return true;
            }
            else if(ec == boost::asio::error::operation_aborted){
                return true;
            }
            else if(ec == boost::asio::error::bad_descriptor){
                return true;
            }
            return false;
        }


        bool isclose;
        tcp::resolver::results_type endpoint_;
        tcp::resolver resolver_;
        tcp::socket socket_;
        boost::array<char,32> name_buf_;
        boost::array<char,128> write_buf_;
        boost::array<char,128> read_buf_;

};
int main(int argc,char * argv[]){
    try{
            if(argc!=3){
            std::cerr << "Usage : server [IP] [PORT] \n";
            exit(1);
        }
        boost::asio::io_context io_context_;

        // tcp_connection에 모든 작동을 넘김
        tcp_connection connection(io_context_,argv[1],argv[2]);

        boost::thread thr(boost::bind(&boost::asio::io_context::run,&io_context_));
        io_context_.run();
        thr.join();
    }
    catch(std::exception &e){
        std::cout << e.what() <<"\n";
    }
}