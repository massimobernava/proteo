# Proteo

Proteo is an open-source and cross-platform scriptable platform, highly modular and with built-in support for many useful modules for **Serious Games** implementation. Developing a cross-platform, scriptable solution for multimedia applications even with computer vision and **Deep Learning** support is a challenging task, since many different components have to be connected and tuned to reach a robust yet performative system

Proteo framework is designed to develop **Serious Games** with **Deep Learning** support, it work as a general purpose glue, and wraps together several libraries::

- Lua for script
- SDL for graphics and audio
- OpenCV for computer vision
- Enet and ZMQ for networking
- Sqlite and Ejdb for database
- TensorflowLite for machine learning
- Ffmpeg for codecs 

Proteo consists of two main applications, Proteo Server and Proteo Client, both written in C using libraries that can be ported to the main hardware and software platforms. The API is designed to be minimal and easy: one API for client and server.

![Proteo Architecture](Proteo Architecture.png)

Do you want to be a full stack developer? With Proteo you just need a single programming language and a minimalist API!

Proteo is distributed under the MIT License. 

## Example

### Server plugin
```lua

local json=require "json"

proteo.route.get("helloworld/hi",

	function(username,permission,data,param) 
  
		local data={}
		data["hello"]="Hi "..username

		return json.encode(data)
    
	end
)

```

### Client script
```lua

local json=require "json"

function hi_callback(res,data)

  proteo.system.console(data.hello)
  
end

function init()

  proteo.network.proteo_get("/helloworld/hi","hi_callback")

end

```
## Installation


## Server

sudo apt install git cmake lua5.1 liblua5.1-0-dev libssl-dev libcurl4-openssl-dev libsqlite3-dev libmicrohttpd-dev libjson-c-dev libenet-dev

## Client 

### Linux
Test: Ubuntu ... Raspberry ...

### OSX

### iOs

### Windows


## Config

After installation, you need to configure the config.json file on the server and client.

[Detail](https://github.com/massimobernava/proteo/wiki/Config-file)


## API

A few examples. For [details](https://github.com/massimobernava/proteo/wiki/API)

### Example

```lua

--Example Gui

function init()
	form_gui=proteo.gui.newForm("form_gui","SimpleForm",'Helvetica',20,"black","crimson","blanchedalmond",30,proteo.gui.FormType.Normal,"","",MID_X,MID_Y,250,250,"")
	buttonform_gui=proteo.gui.newButton('buttonform_gui',"form button",'Helvetica',15,"black","cornsilk",1,"crimson",false,20 ,50,100,25,"")
	proteo.gui.addItem(form_gui,buttonform_gui)
end

```

```lua

--Example OpenCV

proteo.system.console()

```

## Flat Themed Gui

## Gym Tetris

## Jump'n'Bump Porting
[Wiki](https://en.wikipedia.org/wiki/Jump_%27n_Bump)

![](jumpnbump.gif)

## Pose Animator Porting

![](ProteoPose_small.mov)