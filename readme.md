# Web Server in C

This project is a simple HTTP web server implemented in C. It is designed to handle basic `GET` and `POST` requests, serve static files, and process dynamic requests.

## Features

- Serves static HTML files.
- Handles `GET` and `POST` requests.
- Routes can be easily registered using a dedicated function.
- Example dynamic route to generate random numbers.
- Modular structure for better code organization.

## Requirements

- GCC (GNU Compiler Collection)
- Windows environment with Winsock2 library

## Compilation

Compile the project using the following command:

```bash
gcc main.c -o main -lws2_32
```

## Running the Server

Run the compiled binary:

```bash
./main.exe
```

The server will start listening on `127.0.0.1:8080`.

## Adding Routes

Routes are registered in the `main.c` file using the `register_routes` function. For example:

```c
register_routes(routes, &route_count, "/", "index.html", NULL);
register_routes(routes, &route_count, "/random_numbers", NULL, random_number_post_handler);
```

- **Static files**: Specify the route and file path (e.g., `index.html`).
- **Dynamic routes**: Specify the route and a handler function (e.g., `random_number_post_handler`).

## Example Dynamic Route

The `/random_numbers` route generates a list of random numbers. It accepts a `count` query parameter to specify how many random numbers to generate.

### Request:

```http
POST /random_numbers?count=5 HTTP/1.1
Host: 127.0.0.1:8080
```

### Response:

```json
{
  "random_numbers": [12345, 67890, 23456, 78901, 34567]
}
```

## Utilities

- **File type detection**: Determines the content type of files.
- **Query parameter parsing**: Parses query strings into key-value pairs.

## Known Issues

- Limited error handling.
- Not refactored

## Future Enhancements

- Support for additional HTTP methods.
- Enhanced error handling and logging.
- Multithreading for concurrent request handling.
- SSL/TLS support.

## License

This project is open-source and available under the MIT License.

