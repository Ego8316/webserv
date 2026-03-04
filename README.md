<p align="center">
  <img src="https://github.com/Ego8316/Ego8316/blob/main/42-badges/webserv.png" height="150" alt="42 webserv Badge"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/42-Project-blue" height="32"/>
  <img src="https://img.shields.io/github/languages/repo-size/Ego8316/webserv?color=5BCFFF" height="32"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Linux-0a97f5?style=for-the-badge&logo=linux&logoColor=white" height="32"/>
  <img src="https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=apple&logoColor=white" height="32"/>
  <img src="https://img.shields.io/badge/WSL-0a97f5?style=for-the-badge&logo=linux&logoColor=white" height="32"/>
</p>

# webserv

### 🎓 42 School – Final Grade: **125/100**
A custom HTTP/1.1 server written in C++98 with non-blocking sockets and a `poll()`-driven architecture.

---

## 📦 Features
- HTTP/1.1 request parsing and response pipeline.
- Multi-method support: `GET`, `POST`, `DELETE`.
- Single event loop for all clients with `poll()`.
- Keep-Alive management and request timeouts.
- Chunked request body handling.
- Static file serving and index resolution.
- File upload endpoints.
- CGI support (`fork`/`exec`) with pipes.
- Configurable error pages and redirects.
- Multi-server and per-location configuration.
- Cookie parsing and simple session handling.
- Limiters for request size and allowed methods.

---

## 🛠️ Building
```bash
make              # build ./webserv
make clean        # remove object files
make fclean       # remove binaries + objects
make re           # full rebuild
make LOG_LEVEL=3  # optional: set logging verbosity
```

Requirements:
- GCC/Clang (`-std=c++98` compatible)
- `make`
- Optional CGI binaries for dynamic routes (`php-cgi`, `python`, ...)

---

## 🚀 Usage
```bash
./webserv config.conf
```

### Example configuration
```nginx
server {
    listen 0.0.0.0:8080;
    server_name webserv.local;
    root ./www/site1;
    index index.html;
    client_max_body_size 15M;
    timeout 3s;

    error_pages {
        404 /errors/404.html;
    }

    location / {
        root ./www/site1;
        limit_except GET;
        autoindex off;
    }

    location /upload {
        root ./www/site1/uploads;
        upload_path ./www/site1/uploads;
        limit_except POST;
    }

    location /cgi-bin {
        cgi on;
    }
}
```

```bash
curl http://localhost:8080/
curl -X POST -d "data=test" http://localhost:8080/upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload
```

---

## 🧩 Architecture Overview
- **ServerCore**: socket creation, bind/listen, and `poll()` control.
- **Client**: per-connection state machine and lifecycle management.
- **Request**: parse and validate HTTP headers and body.
- **Response**: compose headers and body, stream files.
- **RequestHandler**: route resolution and request dispatch.
- **CGI manager**: child process execution and stdio communication.
- **Config parser/interpreter**: converts nginx-like config into runtime structures.

---

## ⚠️ Limitations
- HTTP/2 and TLS are not implemented.
- No built-in rate limiting or anti-abuse layers.
- CGI support is limited compared with production-grade stacks.

---

## 👥 Contributors
- [Ego8316](https://github.com/Ego8316)
- [victorviterbo](https://github.com/victorviterbo/)

---

## 📄 License
MIT — see `LICENSE`.


