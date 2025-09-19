#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <set>

using boost::asio::ip::tcp;

class tcp_server;
// 여기서 돌아가는 비동기 작업은 read,write read가 사실상 대부분 
class tcp_connection : public boost::enable_shared_from_this<tcp_connection>{ // 이 클래스의 세션을 ptr로 설정하게 변경 
    public:
        tcp_connection(boost::asio::io_context &io_context,tcp_server &server);
        tcp::socket &socket();
        void set_name();
        void start_chat();
        char * get_name();

    private:
        void read_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted);
        void set_name_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted);

        void write_handler(const boost::system::error_code& ec,std::size_t bytes_transferred);
        bool handle_ec(const boost::system::error_code& ec);

        tcp_server &server_;
        tcp::socket socket_;
        boost::array<char,128> buffer_;
        boost::array<char,32> name_;
};

// -> tcp_server 클래스를 통해 세션을 관리
// 1. shared_ptr을 통해서 tcp_connection을 관리
// 2. shared_ptr을 통해서 set을 만들어 각 세션의 집합을 생성
// 3. 해제될 때 set에서도 같이 해제  


class tcp_server{ 
    public:
        tcp_server(boost::asio::io_context &io_context,int port_num)
            : io_context_(io_context),
              acceptor_(io_context_,tcp::endpoint(tcp::v4(),port_num)) 
        {
            start_accept();
        }
        void remove_connection(boost::shared_ptr<tcp_connection> connection){
            if(sessions_.erase(connection)==1) {
                std::cout << connection->get_name() << "가 퇴장하였습니다.\n";
                std::cout << "남은 인원 : "<<sessions_.size() <<"\n";
            }
            // erase된 원소 개수 1 -> 삭제됨 -> 메세지 출력
            // erase된 원소 개수 0 -> 이미 삭제된 상태 -> 메세지 출력 X 
        }
        void deliver(boost::array<boost::asio::mutable_buffer, 2> buffer){
            for(auto temp_connection: sessions_){
                boost::asio::async_write(temp_connection->socket(),buffer,
                    boost::bind(&tcp_server::write_handler,this,
                        boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
                std::cout << "send chat to " << temp_connection->get_name() <<"\n";
            }
        }
    private:
        void start_accept(){
            boost::shared_ptr<tcp_connection> new_connection(new tcp_connection(io_context_,*this));
            acceptor_.async_accept(new_connection->socket(),boost::bind(&tcp_server::accept_handler,this,
                new_connection,boost::asio::placeholders::error)); // 여기서 accept를 비동기 실행 (이벤트가 발생시 처리 후 핸들러로 넘어감)
        }
        void accept_handler(boost::shared_ptr<tcp_connection> new_connection,const boost::system::error_code& ec){
            if(!ec){ // 에러 발생 X
                std::cout << "Connected!\n";
                sessions_.insert(new_connection);
                new_connection->set_name(); // 이름 설정 시작 (설정 뒤 에코 시작)
                // 여기서 연결된 new_connection을 set에 넣음 
            }
            start_accept(); // 다시 accept을 받음 
        }
        void write_handler(const boost::system::error_code& ec,std::size_t bytes_transferred){
        }
    
    std::set<boost::shared_ptr<tcp_connection>> sessions_;
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

        boost::thread thr(boost::bind(&boost::asio::io_context::run,&io_context));
        io_context.run();
        thr.join();
    }
    catch(std::exception &E){
        std::cerr << E.what() <<"\n";
    }
}

tcp_connection::tcp_connection(boost::asio::io_context &io_context,tcp_server &server) : socket_(io_context), server_(server)
{

}
tcp::socket & tcp_connection::socket(){
            return socket_;
}
void tcp_connection::set_name(){
    boost::asio::async_read(socket_,boost::asio::buffer(name_,32),
        boost::bind(&tcp_connection::set_name_handler,shared_from_this(),boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}
void tcp_connection::start_chat(){ // 계속해서 Client: 만 출력되는 오류 read가 데이터가 들어오지 않았는데도 핸들러로 넘어감
            // transmitted byte가 0인데도 넘어감 -> 아마 버퍼가 string 이라 그런걸 지도(길이가 정해져있지않음)
            // -> 버퍼를 string 에서 boost::array로 바꿔서 해결 (어차피 나중에 메세지 만들 때 바이트 단위로 
            //      관리해야해서 이게 확실히 맞는 선택인듯)
            boost::asio::async_read(socket_,boost::asio::buffer(buffer_,128),
                boost::bind(&tcp_connection::read_handler,shared_from_this(),boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
void tcp_connection::read_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted){
    if(handle_ec(ec)){
        server_.remove_connection(shared_from_this());
        return;
    }
    std::cout << "bytes tranitted : " << bytes_transmitted <<"\n";
    std::cout <<name_.data()<<" :" << buffer_.data() <<"\n";
            
    boost::array<boost::asio::mutable_buffer, 2> buf_= 
        {boost::asio::buffer(name_),boost::asio::buffer(buffer_)};

    server_.deliver(buf_);
    start_chat();
    // 에코 코드
    // boost::asio::async_write(socket_,buf_,
    //     boost::bind(&tcp_connection::write_handler,shared_from_this(),
    //         boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));
}
void tcp_connection::set_name_handler(const boost::system::error_code& ec,std::size_t bytes_transmitted){
    std::cout << "New Client !\nName : " <<name_.data() <<"\n";
    start_chat();
}
void tcp_connection::write_handler(const boost::system::error_code& ec,std::size_t bytes_transferred){
    if(handle_ec(ec)){
        server_.remove_connection(shared_from_this());
        return;
    }
    start_chat();
}
bool tcp_connection::handle_ec(const boost::system::error_code& ec){ // 오류 -> false 
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
    return false;
    // 여기서 오류가 발생하면 
    // tcp_server 쪽으로 제어를 넘겨야하는데 어떻게 해야하지 
}
char * tcp_connection::get_name(){
    return (char *)name_.data();
}