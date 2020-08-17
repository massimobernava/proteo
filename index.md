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

## Example

### Server plugin
```markdown

local json=require "json"

proteo.route.get("helloworld/hi",

	function(username,permission,data,param) 
  
		local data={}
		data["hello"]="Hi "..username

		return json.encode(info)
    
	end
)

```

### Client script
```markdown

local json=require "json"

function hi_callback(res,data)

  proteo.system.console(data.hello)
  
end

function init()

  proteo.network.proteo_get("/helloworld/hi","hi_callback")

end

```
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


## Config

After installation, you need to configure the config.json file on the server and client.

[Detail] (https://github.com/massimobernava/proteo/wiki/Config-file)


## API

A few examples. For [details] (https://github.com/massimobernava/proteo/wiki/API)

### Example

```markdown

--Example Gui

proteo.system.console()

```

```markdown

--Example OpenCV

proteo.system.console()

```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

