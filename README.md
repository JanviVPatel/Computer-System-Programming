📻 Multithreaded Process Radio Station (C – Linux)

This project simulates a radio broadcasting system using multiple processes, pipes, and signals in C (POSIX/Linux).
A parent process acts as a radio station, broadcasting songs to two listener processes.
Users can control the broadcast in real time using keyboard commands.

📌 Features

👂 Two independent listener processes

🔀 Random song broadcasting

⏸ Pause listeners using signals

▶ Resume listeners

🛑 Graceful shutdown of broadcast

📡 Inter-process communication using pipes

⌨ Non-blocking user input using select()

🛠 Technologies Used

Language: C

OS: Linux / POSIX-compliant system

System calls:

fork()
pipe()
dup2()
execl()
kill()
select()
waitpid()

📂 File Structure
.
├── radio_station.c     # Main radio broadcaster (parent process)
├── listener.c          # Listener program (child process)
├── README.md           # Project documentation


⚠️ Make sure listener.c is compiled before running the radio station.

⚙️ Compilation

Compile both programs using gcc:

gcc radio_station.c -o radio_station
gcc listener.c -o listener

▶️ How to Run
./radio_station


Once started, the radio station will begin broadcasting songs to both listeners.

🎮 Controls
Key	Action
p	Pause listeners
r	Resume listeners
s	Stop broadcast and exit

The program checks user input without blocking, so broadcasting continues smoothly.

🔁 How It Works

The parent process:

Creates two pipes

Forks two child processes

Sends song data through pipes

Each listener:

Reads from its pipe (via stdin)

Prints received songs

Signals:

SIGSTOP → Pause listeners

SIGCONT → Resume listeners

SIGTERM → Terminate listeners

select():

Allows checking keyboard input without stopping broadcast

🎵 Songs List (Sample)
"Why this kolaveri di"
"Mai hu gian"
"All is well"
"Sunshine"


Songs are chosen randomly
