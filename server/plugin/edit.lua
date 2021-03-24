

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
		for i=1,#files do
			if ends_with(files[i],".xml") then
				plugins.list[#plugins.list+1]=files[i]:gsub("%.xml", "")
			end
		end

		return json.encode(plugins)

	end
)

proteo.route.get("edit/scripts",

	function(username,permission,data,param) 

		local files=proteo.system.dir(BASEDIR.."script/")
		
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

proteo.route.get("edit/plugin/:plugin",

	function(username,permission,data,param) 

		--if username~="admin" and ispermitted(permission,"edit")==false then return "NULL" end
		
		local script={}
		script["type"]="PLUGIN"
		script["result"]="OK"

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

		local data=proteo.system.readFile(BASEDIR.."script/"..param["script"]..".xml")

		script["xml"]=data
		
		return json.encode(script)
	end
)
proteo.route.put("edit/plugin/:plugin",

	function(username,permission,data,param) 

		local script={}
		script["type"]="PLUGIN"
		script["result"]="OK"

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

		local res_xml=proteo.system.writeFile(BASEDIR.."script/"..param["script"]..".xml",tmp["xml"])
		local res_lua=proteo.system.writeFile(BASEDIR.."script/"..param["script"]..".lua",tmp["lua"])

		if res_xml~=0 or res_lua~=0 then
			script["result"]="FAIL"
		end

		return json.encode(script)
	end
)
