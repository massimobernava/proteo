

local json=require "json"
local admin=require "admin"
local md5 = require 'pure_md5'

-- INIT ===============================

local function ispermitted(permissions,perm)

	for i=1,#permissions do
		--print("PERMISSION: "..permission[i])
		if permissions[i]==perm then return true end
	end

	return false
end

local charset = {}  do -- [0-9a-zA-Z]
    for c = 48, 57  do table.insert(charset, string.char(c)) end
    for c = 65, 90  do table.insert(charset, string.char(c)) end
    for c = 97, 122 do table.insert(charset, string.char(c)) end
end

local function randomString(length)
    if not length or length <= 0 then return '' end
    math.randomseed(os.clock()^5)
    return randomString(length - 1) .. charset[math.random(1, #charset)]
end

--=====================================

proteo.route.get("admin/info",

	function(username,permission,data,param) 

		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("admin/permissions",

	function(username,permission,data,param) 
		local permissions={}

		permissions.permission={}
		permissions.permission[1]="admin:admin"


		return json.encode(permissions)
	end
)



proteo.route.get("admin/config",

	function(username,permission,data,param) 

		if username~="admin" and ispermitted(permission,"admin")==false then return "NULL" end
		
		--local conf=proteo.system.readFile(BASEDIR.."config.json") --TODO attenzione, se dentro il config "originale" il basedir punta altrove questo readfile non lo trova
		
		--TODO forse si popssono aggiungere anche i file nella cartella plugin in modo da poter vedere quali sono attivi e quali no
		local conf=proteo.system.readFile("./config.json")
		
		return conf
	end
)

proteo.route.put("admin/config",

	function(username,permission,data,param) 

	end
)

--TODO forse è meglio passare l'userid
proteo.route.get("admin/user/:username",

	function(username,permission,data,param) 

		if username~="admin" and ispermitted(permission,"admin")==false then return "{}" end

		local send=admin.getUserByName(param["username"])
		
		return json.encode(send)
	end
)

proteo.route.post("admin/add/user",

	function(username,permission,data,param) 

		if username~="admin" and ispermitted(permission,"admin")==false then return "{}" end

		local current_user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT id_user FROM users WHERE username='"..username.."'")

		local user=json.decode(data)

		admin.addUser(user,current_user[1].id_user)


 		local ret={}
 		ret["type"]="ADDUSER"
 		ret["result"]="OK"

 		return json.encode(ret)
	end
)

proteo.route.post("admin/update/user",

	function(username,permission,data,param) 

		if username~="admin" and ispermitted(permission,"admin")==false then return "{}" end

		local current_user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT id_user FROM users WHERE username='"..username.."'")

		local user=json.decode(data)

		admin.updateUser(user,current_user[1].id_user)

		local ret={}
 		ret["type"]="UPDATEUSER"
 		ret["result"]="OK"

 		return json.encode(ret)
	end
)

proteo.route.post("admin/search/users",

	function(username,permission,data,param) 

			if username~="admin"and ispermitted(permission,"admin")==false then return "{}" end

			local users=admin.searchUser(data)

			return json.encode(users)
	end
)


proteo.route.post("admin/register",

	function(username,permission,data,param) 

		local register=json.decode(data)
		local password=randomString(8)

		--local current_user_id=admin.getUserid(username)

		local user={}

		user.cancelled=0
		user.username=register.username
		user.password=md5.Calc(password)
		user.permissions={}
		user.permissions[1]={}
		user.permissions[1].app="proteo"
		user.permissions[1].permission=register.script


		admin.addUser(user,0)
		proteo.system.sendMail("","","","Registrazione","Utente: "..register.username.." Password: "..password)

		--local users=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT id_user,username FROM users ") --WHERE username LIKE %

		local ret={}
 		ret["type"]="REGISTER"
 		ret["username"]=register.username
 		ret["result"]="OK"

 		return json.encode(ret)
	end
)
