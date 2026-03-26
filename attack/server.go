package main

import (
	"fmt"
	"io"
	"net"
)

const resp = "HTTP/1.0 200 OK\r\nContent-Length: 2\r\nContent-Type: text/plain\r\n\r\nOK\r\n"

func main() {
	lis, err := net.Listen("tcp", ":15514")
	panicErr(err)

	for {
		conn, err := lis.Accept()
		panicErr(err)
		go handleConn(conn)
	}
}

func panicErr(err error) {
	if err != nil && err != io.EOF {
		panic(err.Error())
	}
}

func handleConn(conn net.Conn) {
	defer conn.Close()
	b := make([]byte, 1024)

	n, err := conn.Read(b)
	panicErr(err)
	fmt.Printf("read:\n")
	fmt.Printf("  n: %d\n", n)
	if n == 0 {
		return
	}
	fmt.Printf("  b[:n]: %s\n", b[:n])

	n, err = conn.Write([]byte(resp))
	panicErr(err)
	fmt.Printf("write:\n")
	fmt.Printf("  n: %d\n", n)
	fmt.Printf("  resp: %s\n", resp)
}
