
local json=require "json"

-- INIT ===============================
--if proteo.system.fileExist("deepindigo.db")==false then
--	local id=proteo.ejdb.put("deepindigo.db","bigbrother",'{"name":"Tizio1", "surname":"Caio1","birth_date":"19770424","sex":"M", "pid":"PID001"}')--new doc
--	id=proteo.ejdb.put("deepindigo.db","bigbrother",'{"name":"Tizio2", "surname":"Caio2","birth_date":"19770424","sex":"M", "pid":"PID002"}')--new doc
--	id=proteo.ejdb.put("deepindigo.db","bigbrother",'{"name":"Tizio3", "surname":"Caio3","birth_date":"19770424","sex":"M", "pid":"PID003"}')--new doc
--	id=proteo.ejdb.put("deepindigo.db","bigbrother",'{"name":"Tizio4", "surname":"Caio4","birth_date":"19770424","sex":"M", "pid":"PID004"}')--new doc
--end
--=====================================

proteo.route.get("deepindigo/info",

	function(username,permission,data,param) 
		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("deepindigo/permissions",

	function(username,permission,data,param) 
		local permissions={}
		permissions[1]="deepindigo:deepindigo"

		return json.encode(permissions)
	end
)

proteo.route.get("deepindigo/data/:collection",

	function(username,permission,data,param) 

		data=json.decode(data)

        local info={}
        info["message"]="OK"
        info["result"]=proteo.ejdb.exe("deepindigo.db",param["collection"],data["query"])

		return json.encode(info)
	end
)

proteo.route.post("deepindigo/data/:collection",

	function(username,permission,data,param) 
   
		data=json.decode(data)

		if data["method"]=="put" then
    		id=proteo.ejdb.put("deepindigo.db",param["collection"],json.encode(data["data"]))
    	elseif data["method"]=="test" then
    		print("Deepindigo test: "..json.encode(data["data"]))
    		id=0
		end

   		local info={}
        info["message"]="OK"
        info["id"]=id
        
        return json.encode(info)
	end
)
