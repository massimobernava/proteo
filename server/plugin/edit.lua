

local json=require "json"
local md5 = require 'pure_md5'

-- INIT ===============================

local function ispermitted(permissions,perm)

	for i=1,#permissions do
		--print("PERMISSION: "..permission[i])
		if permissions[i]==perm then return true end
	end

	return false
end

local function starts_with(str, start)
   return str:sub(1, #start) == start
end

local function ends_with(str, ending)
   return ending == "" or str:sub(-#ending) == ending
end

--=====================================

proteo.route.get("edit/info",

	function(username,permission,data,param) 

		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("edit/permissions",

	function(username,permission,data,param) 
		local permissions={}

		permissions.permission={}
		permissions.permission[1]="edit:edit"


		return json.encode(permissions)
	end
)


proteo.route.get("edit/plugins",

	function(username,permission,data,param) 

		local files=proteo.system.dir(BASEDIR.."plugin/")

		local plugins={}
		plugins.list={}

		if username~="demo" then
			for i=1,#files do
				if ends_with(files[i],".xml") then
					plugins.list[#plugins.list+1]=files[i]:gsub("%.xml", "")
				end
			end
		end

		return json.encode(plugins)

	end
)

proteo.route.get("edit/scripts",

	function(username,permission,data,param) 

		local files=proteo.system.dir(BASEDIR.."script/")
		
		if username=="demo" then
			files=proteo.system.dir(BASEDIR.."script/demo/")
		end

		local scripts={}
		scripts.list={}
		for i=1,#files do
			if ends_with(files[i],".xml") then
				scripts.list[#scripts.list+1]=files[i]:gsub("%.xml", "")
			end
		end

		return json.encode(scripts)

	end
)

proteo.route.get("edit/items",

	function(username,permission,data,param) 

		local items={}
		items.items={}

		if username=="demo" then
			items.items[1]={id="proteo_lib",hide=true}
			items.items[2]={id="proteo_route",hide=true}
			items.items[3]={id="proteo_gui",hide=true}
			items.items[4]={id="proteo_opencv",hide=true}
			items.items[5]={id="proteo_system",hide=true}
			items.items[6]={id="proteo_graphics",hide=true}
			items.items[7]={id="proteo_enet",hide=true}
			items.items[8]={id="proteo_zmq",hide=true}
			items.items[9]={id="proteo_network",hide=true}
			items.items[10]={id="proteo_ffmpeg",hide=true}
			items.items[11]={id="proteo_audio",hide=true}
			items.items[12]={id="proteo_array",hide=true}
		end

		return json.encode(items)

	end
)

proteo.route.get("edit/plugin/:plugin",

	function(username,permission,data,param) 

		--if username~="admin" and ispermitted(permission,"edit")==false then return "NULL" end

		local script={}
		script["type"]="PLUGIN"
		script["result"]="OK"

		if username=="demo" then
			script["result"]="PERMISSION DENIED"
			return json.encode(script)
		end

		local data=proteo.system.readFile(BASEDIR.."plugin/"..param["plugin"]..".xml")

		script["xml"]=data
		
		return json.encode(script)
	end
)

proteo.route.get("edit/script/:script",

	function(username,permission,data,param) 

		--if username~="admin" and ispermitted(permission,"edit")==false then return "NULL" end
		
		local script={}
		script["type"]="SCRIPT"
		script["result"]="OK"

		if username=="demo" then
			data=proteo.system.readFile(BASEDIR.."script/demo/"..param["script"]..".xml")
		else
			data=proteo.system.readFile(BASEDIR.."script/"..param["script"]..".xml")
		end

		script["xml"]=data
		
		return json.encode(script)
	end
)
proteo.route.put("edit/plugin/:plugin",

	function(username,permission,data,param) 

		local script={}
		script["type"]="PLUGIN"
		script["result"]="OK"

		if username=="demo" then
			script["result"]="PERMISSION DENIED"
			return json.encode(script)
		end

		--local data=proteo.system.readFile(BASEDIR.."plugin/"..param["plugin"]..".xml")

		--script["script"]=data
		local tmp=json.decode(data)
		
		--print("LUA: "..tmp["lua"])

		local res_xml=proteo.system.writeFile(BASEDIR.."plugin/"..param["plugin"]..".xml",tmp["xml"])
		local res_lua=proteo.system.writeFile(BASEDIR.."plugin/"..param["plugin"]..".lua",tmp["lua"])

		if res_xml~=0 or res_lua~=0 then
			script["result"]="FAIL"
		end

		proteo.system.reboot()

		return json.encode(script)
	end
)

proteo.route.put("edit/script/:script",

	function(username,permission,data,param) 

		local script={}
		script["type"]="SCRIPT"
		script["result"]="OK"

		--local data=proteo.system.readFile(BASEDIR.."plugin/"..param["plugin"]..".xml")

		--script["script"]=data
		local tmp=json.decode(data)
		
		--print("LUA: "..tmp["lua"])

		if username=="demo" then
			res_xml=proteo.system.writeFile(BASEDIR.."script/demo/"..param["script"]..".xml",tmp["xml"])
			res_lua=proteo.system.writeFile(BASEDIR.."script/demo/"..param["script"]..".lua",tmp["lua"])
		else
			res_xml=proteo.system.writeFile(BASEDIR.."script/"..param["script"]..".xml",tmp["xml"])
			res_lua=proteo.system.writeFile(BASEDIR.."script/"..param["script"]..".lua",tmp["lua"])
		end

		if res_xml~=0 or res_lua~=0 then
			script["result"]="FAIL"
		end

		return json.encode(script)
	end
)
