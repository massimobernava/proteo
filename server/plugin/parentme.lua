
local json=require "json"
local admin=require "admin"
local apollo=require "apollo"
--local attenzione_visiva_game = require "attenzione_visiva_game"

-- INIT ===============================

local parentme_peers={}
local connections={}

local function ispermitted(permissions,perm)

	for i=1,#permissions do
		--print("PERMISSION: "..permission[i])
		if permissions[i]==perm then return true end
	end

	return false
end

function tprint (tbl, indent)
  if not indent then indent = 0 end
  for k, v in pairs(tbl) do
    formatting = string.rep("  ", indent) .. k .. ": "
    if type(v) == "table" then
      print(formatting)
      tprint(v, indent+1)
    elseif type(v) == 'boolean' then
      print(formatting .. tostring(v))		
    else
      print(formatting .. v)
    end
  end
end

if proteo.system.fileExist(BASEDIR.."parentme.db")==false then
	proteo.sqlite.exe(BASEDIR.."parentme.db",
		[[
		CREATE TABLE 'groups' (
		'id_group' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'name' TEXT NOT NULL UNIQUE
		);]])

	--proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO groups (name) VALUES('Gruppo1')")
	--proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO groups (name) VALUES('Gruppo2')")
	--proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO groups (name) VALUES('Gruppo3')")

	proteo.sqlite.exe(BASEDIR.."parentme.db",
		[[
		CREATE TABLE 'usergroups' (
		'id_usergroups' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'id_group' INTEGER NOT NULL,
		'id_user' INTEGER NOT NULL
		);]])

	--proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO usergroups (id_group,id_user) VALUES(1,1)")
	--proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO usergroups (id_group,id_user) VALUES(3,1)")

	proteo.sqlite.exe(BASEDIR.."parentme.db",
		[[
		CREATE TABLE 'people' (
		'id_people' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'id_user' INTEGER NOT NULL,
		'id_apollo' TEXT NOT NULL UNIQUE
		);]])

--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO people (id_user,id_apollo) VALUES(1,'1')")
--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO people (id_user,id_apollo) VALUES(1,'2')")
--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO people (id_user,id_apollo) VALUES(1,'3')")
--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO people (id_user,id_apollo) VALUES(1,'4')")

	proteo.sqlite.exe(BASEDIR.."parentme.db",
		[[
		CREATE TABLE 'peoplegroups' (
		'id_peoplegroups' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'id_group' INTEGER NOT NULL,
		'id_people' INTEGER NOT NULL
		);
		]]
		)

--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(1,1)")
--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(3,2)")
--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(1,3)")
--	proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(3,4)")
--	
end

--if proteo.system.fileExist(BASEDIR.."apollo.db")==false then
--	local id=proteo.ejdb.put(BASEDIR.."apollo.db","apollo",'{"name":"Tizio1", "surname":"Caio1","birth_date":"19770424","sex":"M", "pid":"PID001"}')--new doc
--	id=proteo.ejdb.put(BASEDIR.."apollo.db","apollo",'{"name":"Tizio2", "surname":"Caio2","birth_date":"19770424","sex":"M", "pid":"PID002"}')--new doc
--	id=proteo.ejdb.put(BASEDIR.."apollo.db","apollo",'{"name":"Tizio3", "surname":"Caio3","birth_date":"19770424","sex":"M", "pid":"PID003"}')--new doc
--	id=proteo.ejdb.put(BASEDIR.."apollo.db","apollo",'{"name":"Tizio4", "surname":"Caio4","birth_date":"19770424","sex":"M", "pid":"PID004"}')--new doc
--end
--=====================================



proteo.route.get("parentme/info",

	function(username,permission,data,param) 
		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("parentme/permissions",

	function(username,permission,data,param) 
		local permissions={}
		permissions.permission={}

		permissions.permission[1]="parentme:admin"
		permissions.permission[2]="parentme:researcher"
		permissions.permission[3]="parentme:therapist"
		permissions.permission[4]="parentme:user"
		permissions.permission[5]="parentme:null"

		-- ADMIN gestisce i gruppi, vede tutti gli utenti che hanno un permesso "parentme" 
		-- RESEARCHER aggiunge gli utenti e i pazienti ai gruppi
		-- THERAPIST vede solo i pazienti che appartengono ad un gruppo in cui è presente
		-- USER vede i people che hanno lo stesso id_user possono aggiungere nuovi pazienti
		-- NULL permette il login ma vede solo una pagina in cui c'è scritto che deve essere abilitato

		return json.encode(permissions)
	end
)

proteo.route.get("parentme/mypermissions",

	function(username,permission,data,param) 

		local permissions={}
		permissions.permission=permission

		return json.encode(permissions)
	end
)

-- USERS 

proteo.route.get("parentme/user/:userid",

	function(username,permission,data,param) 

		if ispermitted(permission,"admin")==false then return "{}" end

		local send=admin.getUserByIdForApp(param["userid"],"parentme")

		send.groups=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT groups.id_group,name FROM usergroups,groups WHERE usergroups.id_group=groups.id_group AND id_user="..param["userid"])

		return json.encode(send)

	end
)

proteo.route.post("parentme/search/users",

	function(username,permission,data,param) 

			if ispermitted(permission,"admin")==false then return "{}" end

			local users=admin.searchUserForApp("*",'parentme')
			--proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT DISTINCT users.id_user,username FROM users, permissions WHERE permissions.id_user==users.id_user AND permissions.app='parentme'")

			return json.encode(users)
	end
)


proteo.route.post("parentme/update/user",

	function(username,permission,data,param) 

		if ispermitted(permission,"admin")==false then return "{}" end

		--local current_user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT id_user FROM users WHERE username='"..username.."'")
		local current_user_id=admin.getUserid(username)

		local user=json.decode(data)

		admin.updatePermissionForApp(user.id_user,user.permissions,"parentme",current_user_id)

		local sql="DELETE FROM usergroups WHERE id_user="..user.id_user
		proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

		if user.groups~=nil then
			for i=1,#user.groups do
 				sql = "INSERT INTO usergroups ('id_user','id_group' ) VALUES ("..user.id_user..","..user.groups[i].id_group..")"
 				proteo.sqlite.exe(BASEDIR.."parentme.db",sql)
 			end
 		end

		local ret={}
 		ret["type"]="UPDATEUSER"
 		ret["result"]="OK"

 		return json.encode(ret)

	end
)

-- EVENT


host_enet=nil
local fast_event_timer=nil
local slow_event_timer=nil

--TODO da testare
proteo.route.post("parentme/start",

	function(username,permission,data,param) 

		local ret={}		
 		ret["type"]="HOST ALREADY STARTED"

		if host_enet==nil then
			ret["type"]="HOST STARTED"
			host_enet=proteo.enet.host_create("*:"..PORT,64,3)
			fast_event_timer=proteo.system.createTimer(10,"update_event")
			slow_event_timer=proteo.system.createTimer(100,"update_event")
			proteo.system.startTimer(slow_event_timer) --da testare questa differenzazione dei timer
		end

 		ret["result"]="OK"

 		return json.encode(ret)
	end
)

function update_event(dt)

	if host_enet==nil then return end
	
	local event = host_enet:service(0)

	if event then
		if event.type == "connect" then
            print("Connected to", event.peer)

        elseif event.type =="disconnect" then
        	
        elseif event.type == "receive" then
			--print("Got message: "..event.data.." From: "..event.peer)
			data=json.decode(event.data)
            if data["type"]=="REPORT" then
            	print("REPORT: "..data["username"].." role: "..data["role"])
            	local id=admin.getUserid(data["username"])
            	parentme_peers[id]={}
            	parentme_peers[id].role=data["role"]
            	parentme_peers[id].name=data["username"]
            	parentme_peers[id].peer=event.peer
   
            elseif data["type"]=="CONNECT" then
            	--Receive "CONNECT" from USER, now all is done
        		print("CONNECT: "..data["from"].." to: "..data["to"])
        		
        		--CONNECTION SEQ--
				-- 5A
				connection_id=#connections+1
            	data["type"]="CONNECTED"
            	data["connection_id"]=connection_id

            	local id=admin.getUserid(data["from"])

				js=json.encode(data)
			
				parentme_peers[id].peer:send(js) --send CONNECTED to THERAPIST
            	event.peer:send(js) --send CONNECTED to USER

				connections[connection_id]={}
				connections[connection_id].user=event.peer
				connections[connection_id].therapist=parentme_peers[id].peer

				proteo.system.stopTimer(slow_event_timer)
				proteo.system.startTimer(fast_event_timer)
				
			elseif data["type"]=="GAME" then
				
				js=json.encode(data)
			
				if data["role"]=="user" then
					connections[data["connection_id"]].therapist:send(js)
				else
					connections[data["connection_id"]].user:send(js)
				end
			elseif data["type"]=="VIDEO" then
				
				js=json.encode(data)
			
				if data["role"]=="user" then
					connections[data["connection_id"]].therapist:send(js)
				else
					connections[data["connection_id"]].user:send(js)
				end
			end
        end
	end
end

proteo.route.post("parentme/connect",

	function(username,permission,data,param) 

		--CONNECTION SEQ--
		-- 2
		
		local connection=json.decode(data)

		data={}
        data["type"]="CONNECT"
		data["from"]=username
		data["to"]=connection.to
		data["game"]=connection.game

		js=json.encode(data)
		local people_conn=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT * FROM people WHERE id_people="..connection.to)

		user_id=tonumber(people_conn[1].id_user)

		--for key,value in pairs(parentme_peers) do print("PEERS: ",key,value.name) end
		--Send CONNECT to USER
		parentme_peers[user_id].peer:send(js)

		return json.encode(js)
	end
)

proteo.route.get("parentme/connect",

	function(username,permission,data,param) 

		--interfaces=proteo.network.getInterfaces()

		--for k, v in pairs(interfaces) do
		--	print(k..":"..v)
		--end
		--local url=interfaces.en0
		--if url==nil then url=interfaces.wlp2s0 end

		return '{"url":"'..BASEURL..':'..PORT..'"}'
	end
)

proteo.route.get("parentme/online",

	function(username,permission,data,param) 

		local userid=admin.getUserid(username)

		local sql="SELECT DISTINCT people.id_people, id_apollo, usergroups.id_user FROM people,usergroups,peoplegroups WHERE "..
		"(people.id_people=peoplegroups.id_people AND peoplegroups.id_group=(SELECT usergroups.id_group FROM usergroups WHERE usergroups.id_user="..userid.."))"..
		" OR (people.id_user=usergroups.id_user AND usergroups.id_group=(SELECT usergroups.id_group FROM usergroups WHERE usergroups.id_user="..userid.."))"
		.."GROUP BY people.id_people"

		local people=proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

		local ret={}
		for i=1,#people do

			if parentme_peers[people[i].id_user]~=nil then
				apollo_data=apollo.getPeople(people[i]["id_apollo"])

				--tprint(apollo_data)
				if apollo_data~=nil then
					people[i].name=apollo_data.name
					people[i].surname=apollo_data.surname

					ret[#ret+1]=people[i]
				end
			end
		end

		return json.encode(ret)
	end
)

-- PEOPLE

proteo.route.post("parentme/search/people",

	function(username,permission,data,param) 

	if ispermitted(permission,"researcher")==false then return "{}" end

		local userid=admin.getUserid(username)

		--select: people che hanno lo stesso gruppo di userid
		--        oppure people il cui user id di appartenenza fa parte di un gruppo di cui fa parte anche userid
		local sql="SELECT DISTINCT people.id_people, id_apollo, usergroups.id_user FROM people,usergroups,peoplegroups WHERE "..
		"(people.id_people=peoplegroups.id_people AND peoplegroups.id_group=(SELECT usergroups.id_group FROM usergroups WHERE usergroups.id_user="..userid.."))".. 
		"OR (people.id_user=usergroups.id_user AND usergroups.id_group=(SELECT usergroups.id_group FROM usergroups WHERE usergroups.id_user="..userid.."))"
		.."GROUP BY people.id_people"

		local people=proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

		if people~=nil then
			for i=1,#people do
				apollo_data=apollo.getPeople(people[i]["id_apollo"])

				--tprint(apollo_data)
				if apollo_data~=nil then
					people[i].name=apollo_data.name
					people[i].surname=apollo_data.surname
				end
			end
		end
		return json.encode(people)
	end
)

proteo.route.get("parentme/family",

	function(username,permission,data,param) 


		local userid=admin.getUserid(username)

		--seletc: people che hanno lo stesso gruppo di userid
		--        oppure people il cui user id di appartenenza fa parte di un gruppo di cui fa parte acneh userid
		local people=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT * FROM people WHERE people.id_user="..userid)

		if people~=nil then
			for i=1,#people do
				apollo_data=apollo.getPeople(people[i]["id_apollo"])

				if apollo_data~=nil then
					people[i].name=apollo_data.name
					people[i].surname=apollo_data.surname
				else
					people[i]=nil
				end
			end
		end

		return json.encode(people)
	end
)

proteo.route.get("parentme/people/:peopleid",

	function(username,permission,data,param)
		local people=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT * FROM people WHERE id_people="..param["peopleid"])

		apollo_data=apollo.getPeople(people[1]["id_apollo"])
		people[1].name=apollo_data.name
		people[1].surname=apollo_data.surname
		people[1].birth_date=apollo_data.birth_date
		people[1].sex=apollo_data.sex
		people[1].groups=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT groups.id_group,name FROM peoplegroups,groups WHERE peoplegroups.id_group=groups.id_group AND id_people="..people[1]["id_people"])

		return json.encode(people[1])
	end
)

proteo.route.post("parentme/add/people",

	function(username,permission,data,param) 
		--id=proteo.ejdb.put(BASEDIR.."apollo.db","apollo",'{"name":"Tizio2", "surname":"Caio2","birth_date":"19770424","sex":"M", "pid":"PID002"}')--new doc
		--proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO people (id_user,id_apollo) VALUES(1,'4')")
		local current_user_id=admin.getUserid(username)
		local people=json.decode(data)

		--TODO
		people.birth_date="19770424"
		people.sex="M"

		local id_apollo=apollo.newPeople(people)
		res,id_people=proteo.sqlite.exe(BASEDIR.."parentme.db","INSERT INTO people (id_user,id_apollo) VALUES("..current_user_id..",'"..id_apollo.."')")

		if people.groups~=nil then
			for i=1,#people.groups do
 				sql = "INSERT INTO peoplegroups ('id_people','id_group' ) VALUES ("..id_people..","..people.groups[i].id_group..")"
 				proteo.sqlite.exe(BASEDIR.."parentme.db",sql)
 			end
 		end
	end
)

proteo.route.post("parentme/update/people",

	function(username,permission,data,param) 

		local current_user_id=admin.getUserid(username)

		print("UPDATE PEOPLE: "..data)

		local people=json.decode(data)

		local sql="DELETE FROM peoplegroups WHERE id_people="..people.id_people
		proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

		for i=1,#people.groups do
 			sql = "INSERT INTO peoplegroups ('id_people','id_group' ) VALUES ("..people.id_people..","..people.groups[i].id_group..")"
 			proteo.sqlite.exe(BASEDIR.."parentme.db",sql)
 		end

 		apollo.updatePeople(people)

		local ret={}
 		ret["type"]="UPDATEPEOPLE"
 		ret["result"]="OK"

 		return json.encode(ret)
	end
)

--GROUPS

proteo.route.get("parentme/groups",

	function(username,permission,data,param) 

		local groups={}
		if ispermitted(permission,"admin") then 

			groups=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT id_group,name FROM groups")
		end

		return json.encode(groups)
	end
)

proteo.route.post("parentme/add/group",

	function(username,permission,data,param) 

		if ispermitted(permission,"admin") then
			local group=json.decode(data)
			local sql="INSERT INTO `groups` ('name') VALUES ('"..group.name.."')"
 			res,last_id=proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

 			groups=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT id_group,name FROM groups")
 			return json.encode(groups)
 		end

		local ret={}
 		ret["type"]="ADDGROUP"
 		ret["result"]="FAIL"

 		return json.encode(ret)
	end
)

proteo.route.post("parentme/delete/group",

	function(username,permission,data,param) 

		if ispermitted(permission,"admin") then
			local group=json.decode(data)

			local sql="DELETE FROM `peoplegroups` WHERE id_group="..group.id
			 res,last_id=proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

			local sql="DELETE FROM `usergroups` WHERE id_group="..group.id
			 res,last_id=proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

			local sql="DELETE FROM `groups` WHERE id_group="..group.id
			 res,last_id=proteo.sqlite.exe(BASEDIR.."parentme.db",sql)

 			groups=proteo.sqlite.exe(BASEDIR.."parentme.db","SELECT id_group,name FROM groups")
 			return json.encode(groups)
 		end

		local ret={}
 		ret["type"]="DELETEGROUP"
 		ret["result"]="FAIL"

 		return json.encode(ret)
	end
)

--TEST 

proteo.route.get("parentme/test/:testname/:testversion/id/:id",

	function(username,permission,data,param) 

		data={}
        data["message"]="0"
        data["tests"]={}
        data["tests"][1]={}
        data["tests"][1]["name"]="attenzione_visiva"
        data["tests"][1]["data"]=[[EN;20/01/2020-11:55:11;
ST;1579517711.2721;1/3;Animali;0;animali/leone.jpg;animali/cavallo.gif;animali/uccello.jpg;2;attenzione_visiva.data;
ES;1579517712.4296;3;YES;
ST;1579517712.4297;1/3;Animali;1;animali/leone.jpg;animali/cavallo.gif;animali/uccello.jpg;2;attenzione_visiva.data;
ES;1579517713.2313;3;YES;
ST;1579517713.2319;1/3;Animali;2;animali/uccello.jpg;animali/leone.jpg;animali/cavallo.gif;3;attenzione_visiva.data;
ES;1579517714.0422;4;YES;
END;YES]]

	return json.encode(data)

	end
)

proteo.route.post("parentme/add/test",

	function(username,permission,data,param) 


	end
)

proteo.route.post("parentme/update/test",

	function(username,permission,data,param) 


	end
)


