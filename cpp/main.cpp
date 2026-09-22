#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")

struct Book { int id; std::string title, author, category; bool issued; };
struct Member { int id; std::string name, email; };

std::vector<Book> books = {
 {1,"The Alchemist","Paulo Coelho","Fiction",false},
 {2,"Clean Code","Robert C. Martin","Programming",true},
 {3,"Atomic Habits","James Clear","Self Help",false},
 {4,"Introduction to Algorithms","Cormen","Computer Science",false}
};
std::vector<Member> members = {
 {1,"Rohan Sharma","rohan@example.com"},
 {2,"Aman Verma","aman@example.com"},
 {3,"Priya Singh","priya@example.com"}
};

std::string jsonEscape(const std::string&s){std::string r;for(char c:s){if(c=='"')r+="\\\"";else if(c=='\\')r+="\\\\";
else r+=c;}return r;}
std::string booksJson(){std::ostringstream o;o<<"[";for(size_t i=0;i<books.size();++i){if(i)o<<",";auto&b=books[i];o<<"{\"id\":"<<b.id<<",\"title\":\""<<jsonEscape(b.title)<<"\",\"author\":\""<<jsonEscape(b.author)<<"\",\"category\":\""<<jsonEscape(b.category)<<"\",\"issued\":"<<(b.issued?"true":"false")<<"}";}o<<"]";return o.str();}
std::string membersJson(){std::ostringstream o;o<<"[";for(size_t i=0;i<members.size();++i){if(i)o<<",";auto&m=members[i];o<<"{\"id\":"<<m.id<<",\"name\":\""<<jsonEscape(m.name)<<"\",\"email\":\""<<jsonEscape(m.email)<<"\"}";}o<<"]";return o.str();}
std::string statsJson(){int issued=0;for(auto&b:books)if(b.issued)issued++;std::ostringstream o;o<<"{\"totalBooks\":"<<books.size()<<",\"available\":"<<books.size()-issued<<",\"issued\":"<<issued<<",\"members\":"<<members.size()<<"}";return o.str();}
std::string urlDecode(std::string s){std::replace(s.begin(),s.end(),'+',' ');return s;}
std::string param(const std::string&body,const std::string&key){std::string k=key+"=";auto p=body.find(k);if(p==std::string::npos)return"";p+=k.size();auto e=body.find('&',p);return urlDecode(body.substr(p,e==std::string::npos?std::string::npos:e-p));}
std::string response(const std::string&body,const std::string&type="application/json"){std::ostringstream o;o<<"HTTP/1.1 200 OK\r\nContent-Type: "<<type<<"; charset=utf-8\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: "<<body.size()<<"\r\nConnection: close\r\n\r\n"<<body;return o.str();}
std::string notFound(){return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";}
std::string handle(const std::string&req){
 auto first=req.find("\r\n");std::string line=req.substr(0,first);bool post=line.rfind("POST ",0)==0;
 auto a=line.find(' ')+1;auto b=line.find(' ',a);std::string path=line.substr(a,b-a);
 auto bodyPos=req.find("\r\n\r\n");std::string body=bodyPos==std::string::npos?"":req.substr(bodyPos+4);
 if(!post&&path=="/api/books")return response(booksJson());
 if(!post&&path=="/api/members")return response(membersJson());
 if(!post&&path=="/api/stats")return response(statsJson());
 if(post&&path=="/api/toggle"){int id=std::stoi(param(body,"id"));for(auto&book:books)if(book.id==id)book.issued=!book.issued;return response("{\"success\":true}");}
 if(post&&path=="/api/books"){int id=books.empty()?1:books.back().id+1;books.push_back({id,param(body,"title"),param(body,"author"),param(body,"category"),false});return response("{\"success\":true}");}
 if(post&&path=="/api/members"){int id=members.empty()?1:members.back().id+1;members.push_back({id,param(body,"name"),param(body,"email")});return response("{\"success\":true}");}
 if(path=="/")path="/index.html";
 if(!post&&(path=="/index.html"||path=="/style.css"||path=="/script.js")){
  std::ifstream in("web"+path,std::ios::binary);if(!in)return notFound();std::string content((std::istreambuf_iterator<char>(in)),{});
  std::string type=path.size()>=5&&path.substr(path.size()-5)==".html"?"text/html":path.size()>=4&&path.substr(path.size()-4)==".css"?"text/css":"application/javascript";
  return response(content,type);
 }
 return notFound();
}
int main(){
 WSADATA wsa;if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){std::cerr<<"Winsock startup failed.\n";return 1;}
 SOCKET server=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=inet_addr("127.0.0.1");addr.sin_port=htons(8080);
 if(bind(server,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR||listen(server,10)==SOCKET_ERROR){std::cerr<<"Could not start server on port 8080.\n";closesocket(server);WSACleanup();return 1;}
 std::cout<<"Library Management System running at http://localhost:8080\n";std::cout<<"Keep this window open while using the website.\n";
 while(true){SOCKET client=accept(server,nullptr,nullptr);if(client==INVALID_SOCKET)continue;char buffer[65536]{};int n=recv(client,buffer,sizeof(buffer)-1,0);if(n>0){std::string req(buffer,n);std::string res=handle(req);send(client,res.c_str(),(int)res.size(),0);}closesocket(client);}
}