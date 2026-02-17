# triOS syscalls

## 🧠 Custom Syscalls
### 📁 File I/O
0x01	write	Write to console or file descriptor
0x02	read	Read from stdin or file
0x03	open	Open file
0x04	close	Close file
### ⚙️ Process
0x05	exec	Run a program
0x06	exit	Exit current process
0x07	getpid	Get current process ID
### ⏱️ Time
0x08	time	Get system uptime
### 🌐 Networking
0x09	socket_open	Open socket (TCP/UDP)
0x0A	socket_send	Send data over socket
0x0B	socket_recv	Receive data from socket
0x0C	socket_close	Close socket
### 🛠️ Misc
0x0D	yield	Voluntarily yield CPU
0x0E	sleep	Sleep for N milliseconds