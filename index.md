# Proteo

Proteo framework is designed to develop **Serious Games** with **Deep Learning** support, it work as a general purpose glue, and wraps together several libraries::

- Lua for script
- SDL for graphics and audio
- OpenCV for computer vision
- Enet and ZMQ for networking
- Sqlite and Ejdb for database
- Tensorflow for machine learning
- Opus and x264 codecs 

The API is designed to be minimal and easy: one API for client and server.
The code is written in C to be cross-compatible: client support five operating systems (Windows, OSX, Linux, iOS, Android) and browser through Emscripten. 

Do you want to be a full stack developer? With Proteo you just need a single programming language and a minimalist API!

Proteo is distributed under the MIT License. 

## Installation
Questa è la procedura per l'installazione del client e per il server per le diverse piattaforme.

## Server

sudo apt install git cmake lua5.1 liblua5.1-0-dev libssl-dev libcurl4-openssl-dev libsqlite3-dev libmicrohttpd-dev libjson-c-dev libenet-dev

## Client 

### Linux
Testata su Ubuntu ... Raspberry ...

### OSX

### iOs

E' già presente il file prj, bisogna solo scaricare e linkare SDL

### Windows

## Folder structure

## API

### System

Function | Client | Server
------------ | ------------- | -------------
console(string) | X | X
clock() | X | X
fileExist() | | 
readFile() | |
writeFile() | |
document() | |
dir() | |
isdir() | |
basename() | |
getNode() | |
getName() | |
login() | |
loadConfig() | |
saveConfig() | |
setConfig() | |
getPlatform() | |
createTimer() | |
startTimer() | |
stopTimer() | |
system() | |





```markdown

Exxample:

proteo.system.console()



```
### Network

### Gui

### Graphics

### OpenCV

### Enet

### ZMQ

### SQLite

### EJDB

## Server

## Client 

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

