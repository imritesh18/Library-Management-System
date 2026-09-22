# Library Management System

C++ backend + HTML/CSS/JavaScript frontend.

## Run locally

Compile in VS Code:

```powershell
g++ cpp/main.cpp -o library_server.exe -lws2_32
.\library_server.exe
```

Then open http://localhost:8080.

You can also use VS Code Live Server for `web/index.html` while the C++ server is running.

## Features
- Dashboard statistics
- Add books
- Add members
- Search books
- Issue and return books
- C++ backend
- HTML/CSS/JavaScript frontend

## Live demo
https://imritesh18.github.io/Library-Management-System/
GitHub Pages hosts the frontend only. It cannot execute the C++ backend, so the fully functional version requires the local C++ server above.
