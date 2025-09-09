#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;


class tcp_connection{
    public:
        tcp_connection(boost::asio::io_context &io_context) : socket_(io_context)
        {

        }
        tcp::socket &socket(){
            return socket_;
        }
        void set_name(){
            boost::asio::async_read(socket_,boost::asio::buffer(name_,32),
                boost::bind(&tcp_connection::set_name_handler,this,boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        void start_chat(){ // 계속해서 Client: 만 출력되는 오류 read가 데이터가 들어오지 않았는데도 핸들러로 넘어감
            // transmitted byte가 0인데도 넘어감 -> 아마 버퍼가 string 이라 그런걸 지도
            boost::asio::async_read(socket_,boost::asio::buffer(buffer_,128),
                boost::bind(&tcp_connection::read_handler,this,boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }

    private:
        void read_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted){
            std::cout << "bytes tranitted : " << bytes_transmitted <<"\n";
            std::cout <<name_.data()<<" :" << buffer_.data() <<"\n";
            
            boost::asio::async_write(socket_,boost::asio::buffer(buffer_),
                boost::bind(&tcp_connection::write_handler,this,
                boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
        }
        void set_name_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted){
            std::cout << "New Client !\nName : " <<name_.data() <<"\n";
            start_chat();
        }

        void write_handler(const boost::system::error_code& ec,std::size_t bytes_transferred){
            start_chat();
        }

        tcp::socket socket_;
        boost::array<char,128> buffer_;
        boost::array<char,32> name_;
};



class tcp_server{

    public:
        tcp_server(boost::asio::io_context &io_context,int port_num)
            : io_context_(io_context),
              acceptor_(io_context_,tcp::endpoint(tcp::v4(),port_num)) 
        {
            // 생성된 이후에는 계속 연결 요청을 
            // accept 하는 역할을 수행해야 함.
            start_accept();
        }

    

    private:
        void start_accept(){
            // 새로운 커넥션을 만들고
            // 다시 새로운 커넥션 연결을 대기하는 상태로 가야함.
            tcp_connection *new_connection = new tcp_connection(io_context_);
            acceptor_.async_accept(new_connection->socket(),boost::bind(&tcp_server::accept_handler,this,
                new_connection,boost::asio::placeholders::error)); // 여기서 accept를 비동기 실행 (이벤트가 발생시 처리 후 핸들러로 넘어감)
        }
        void accept_handler(tcp_connection *new_connection,const boost::system::error_code& ec){
            
            if(!ec){ // 에러 발생 X
                std::cout << "Connected!\n";
                new_connection->set_name(); // 이름 설정 시작 (설정 뒤 에코 시작)
            }
            start_accept(); // 다시 accept을 받음 
        }
        
    boost::asio::io_context &io_context_;
    tcp::acceptor acceptor_;

};

int main(int argv,char * argc[]){

    if(argv!=2){
        std::cerr << "Usage : [PORT NUM]" <<"\n"; // 일단 localhost로 사용할거라서 
        exit(1);
    }

    try{
        boost::asio::io_context io_context;
        tcp_server server(io_context,std::stoi(argc[1]));

        io_context.run();
    }
    catch(std::exception &E){
        std::cerr << E.what() <<"\n";
    }
}