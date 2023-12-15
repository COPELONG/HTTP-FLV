# HTTP-FLV



在HTTP层实现

所以服务器第一次要向客户端发送http_header

然后源源不断读取flv数据并且循环发送

```c++
constexpr char http_headers[] = \
	"HTTP/1.1 200 OK\r\n" \
	"Access-Control-Allow-Origin: * \r\n" \
	"Content-Type: video/x-flv\r\n" \
	"Content-Length: -1\r\n" \
	"Connection: Keep-Alive\r\n" \
	"Expires: -1\r\n" \
	"Pragma: no-cache\r\n" \
	"\r\n"
	;
```


![image-20231212210010864](https://my-figures.oss-cn-beijing.aliyuncs.com/Figures/image-20231212210010864.png)



![image-20231212210023721](https://my-figures.oss-cn-beijing.aliyuncs.com/Figures/image-20231212210023721.png)

