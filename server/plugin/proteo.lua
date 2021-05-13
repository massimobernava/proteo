
--local dbg = require("debugger")

local json=require "json"
--local Parser = require'ParseLua'
--local ParseLua = Parser.ParseLua
--local Format_Mini = require'FormatMini'

-- INIT ===============================
local libs={}

libs["Form"]="lib/Form.lua"
libs["statemachine"]="lib/statemachine.lua"
libs["LCS"]="lib/LCS.lua"
libs["json"]="lib/json.lua"
libs["room"]="lib/room.lua"
libs["md5"]="lib/md5.lua"
libs["bit"]="lib/numberlua.lua"
libs["matrix"]="lib/matrix.lua"
libs["base64"]="lib/base64.lua"
libs["aeslua"]="lib/aeslua.lua"
libs["inspect"]="lib/inspect.lua"
libs["tfl_utility"]="lib/tfl_utility.lua"
libs["skl_utility"]="lib/skl_utility.lua"

libs["demo_lib"]="lib/demo_lib.lua"
libs["tetris_lib"]="lib/tetris_lib.lua"

libs["video_chat"]="lib/video_chat.lua"
libs["attenzione_visiva"]="lib/attenzione_visiva.lua"
libs["attenzione_uditiva"]="lib/attenzione_uditiva.lua"
libs["pose_analysis"]="lib/pose_analysis.lua"
libs["giochi_parole"]="lib/giochi_parole.lua"

local function starts_with(str, start)
   return str:sub(1, #start) == start
end

local function ends_with(str, ending)
   return ending == "" or str:sub(-#ending) == ending
end

local function ispermitted(permissions,perm)

	for i=1,#permissions do
		--print("PERMISSION: "..permission[i])
		if permissions[i]==perm then return true end
	end

	return false
end

--=====================================

proteo.route.get("proteo/info",

	function(username,permission,data,param) 

		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("proteo/permissions",

	function(username,permission,data,param) 
		local files=proteo.system.dir(BASEDIR.."script/")
		
		local permissions={}
		permissions.permission={}
		for i=1,#files do
			if ends_with(files[i],".json") then
				permissions.permission[#permissions.permission+1]="proteo:"..files[i]:gsub("%.json", "")
			end
		end

		return json.encode(permissions)
	end
)


proteo.route.get("proteo/scriptandlibs/:script",

	function(username,permission,data,param) 

		--print("GET SCRIPT PERMISSION: "..permission)

		local permitted=false
		for i=1,#permission do
			print("PERMISSION: "..permission[i])
			if permission[i]==param["script"] then permitted=true end
		end
		
		if permitted==false then
 			local fail={}
			fail["type"]="SCRIPT"
			fail["result"]="FAIL"
			fail["script"]=proteo.system.readFile(BASEDIR.."login.lua")
			
			return json.encode(fail)
 		end
 		
		local script_info=json.decode(proteo.system.readFile(BASEDIR.."script/"..param["script"]..".json"))
		
		local script={}
		script["type"]="SCRIPT"
		script["result"]="OK"
		script["libs"]={}
		
		if script_info["libs"]~=nil then
			for i=1,#script_info["libs"] do
				local lib_name=script_info["libs"][i]
				script["libs"][lib_name]=proteo.system.readFile(BASEDIR..libs[lib_name])
			end
		end
		
		--TODO: in alcuni casi (vedi parentme) lo script paziente/terapista potrebbe essere diverso
		-- nel caso lo script da caricare dipende dai permessi o si passa un parametro allo script?

		local data=proteo.system.readFile(BASEDIR.."script/"..param["script"]..".lua")
		--local st, ast = ParseLua(data)
		--print()
		--if st then
		--	script["script"]=Format_Mini(ast)
		--else
		--	script["script"]=data
		--end

		script["script"]=data

		return json.encode(script)
	end
)

proteo.route.get("proteo/script/:script",

	function(username,permission,data,param) 

		--print("GET SCRIPT PERMISSION: "..permission[1])
		if username=="demo" then --TODO può diventare l'opzione di default cercare lo script ANCHE nella cartella del proprio utente
								 --potrebbe essere un modo per testare gli script prima di rilasciarli nella cartella comune
			script={}
			script["type"]="SCRIPT"
			script["result"]="OK"
			data=proteo.system.readFile(BASEDIR.."script/demo/"..param["script"]..".lua")
			script["script"]=data

			return json.encode(script)
		end

		local permitted=ispermitted(permission,param["script"])
		--false
		--for i=1,#permission do
		--	print("PERMISSION: "..permission[i])
		--	if permission[i]==param["script"] then permitted=true end
		--end

 		if permitted==false then
 			local fail={}
			fail["type"]="SCRIPT"
			fail["result"]="FAIL"
			fail["script"]=proteo.system.readFile(BASEDIR.."login.lua")
			
			return json.encode(fail)
 		end

		local script={}
		script["type"]="SCRIPT"
		script["result"]="OK"

		--TODO: in alcuni casi (vedi parentme) lo script paziente/terapista potrebbe essere diverso
		-- nel caso lo script da caricare dipende dai permessi

		local data=proteo.system.readFile(BASEDIR.."script/"..param["script"]..".lua")
		--local st, ast = ParseLua(data)
		--print()
		--if st then
		--	script["script"]=Format_Mini(ast)
		--else
		--	script["script"]=data
		--end

		script["script"]=data

		return json.encode(script)
	end
)

--copia uno script in locale, può essere usato dal master per aggiungere script ai server
proteo.route.put("proteo/script/:script",

	function(username,permission,data,param) 

	end
)

proteo.route.post("proteo/console",

	function(username,permission,data,param) 

		local tmp=json.decode(data)

		print(username.."["..tmp["timestamp"].."]: "..tmp["console"])
		return "{}"
	end
)

proteo.route.get("proteo/lib/:lib",

	function(username,permission,data,param) 

		local lib={}
		lib["type"]="LIB"
		lib["result"]="OK"
		lib["name"]=param["lib"]
		lib["lib"]=proteo.system.readFile(BASEDIR..libs[param["lib"]])
		
		local ret=json.encode(lib)
		
		return ret
	end
)
