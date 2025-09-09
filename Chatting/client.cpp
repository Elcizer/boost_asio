#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;

int main(int argc,char * argv[]){

    try{
            if(argc!=3){
            std::cerr << "Usage : server [IP] [PORT] \n";
            exit(1);
        }
        boost::asio::io_context io_context;
        tcp::resolver resolver_(io_context);
        tcp::resolver::results_type endpoint_ = resolver_.resolve(argv[1],argv[2]);

        tcp::socket socket_(io_context);

        boost::asio::connect(socket_,endpoint_);
        std::cout << "Connect Success\n";

        boost::array<char,32> name;

        std::cout << "Please Type Name :"; 
        std::cin.getline(name.data(),32);

        boost::system::error_code ignored_error;
        socket_.write_some(boost::asio::buffer(name,128),ignored_error);

        while(true){
            boost::array<char,128> buf;
            boost::system::error_code ec;
            
            std::cout << "Input : "; 
            std::cin.getline(buf.data(),128);

            boost::system::error_code ignored_error;
            socket_.write_some(boost::asio::buffer(buf,128),ignored_error);

            size_t len = socket_.read_some(boost::asio::buffer(buf),ec);
            
            if(ec== boost::asio::error::eof) break;
            else if(ec) throw boost::system::system_error(ec);
            
            std::cout << buf.data() << "\n";
        }
    }
    catch(std::exception &e){
        std::cout << e.what() <<"\n";
    }
}