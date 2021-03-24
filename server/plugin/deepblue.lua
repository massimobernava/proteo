
--if deepblue_once==true then return end
--deepblue_once=true

local json=require "json"
local BB=require "bigbrother"

-- INIT ===============================

if proteo.system.fileExist("deepblue.db")==false then
	proteo.sqlite.exe("deepblue.db",
		[[
		CREATE TABLE 'groups' (
		'id_group' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'name' TEXT NOT NULL UNIQUE
		);]])

	proteo.sqlite.exe("deepblue.db","INSERT INTO groups (name) VALUES('Gruppo1')")
	proteo.sqlite.exe("deepblue.db","INSERT INTO groups (name) VALUES('Gruppo2')")
	proteo.sqlite.exe("deepblue.db","INSERT INTO groups (name) VALUES('Gruppo3')")

	proteo.sqlite.exe("deepblue.db",
		[[
		CREATE TABLE 'usergroups' (
		'id_usergroups' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'id_group' INTEGER NOT NULL,
		'username' TEXT NOT NULL
		);]])

	proteo.sqlite.exe("deepblue.db","INSERT INTO usergroups (id_group,username) VALUES(1,'massimo.bernava@gmail.com')")
	proteo.sqlite.exe("deepblue.db","INSERT INTO usergroups (id_group,username) VALUES(3,'massimo.bernava@gmail.com')")

	proteo.sqlite.exe("deepblue.db",
		[[
		CREATE TABLE 'people' (
		'id_people' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'id_bigbrother' TEXT NOT NULL UNIQUE
		);]])

	proteo.sqlite.exe("deepblue.db","INSERT INTO people (id_bigbrother) VALUES('1')")
	proteo.sqlite.exe("deepblue.db","INSERT INTO people (id_bigbrother) VALUES('2')")
	proteo.sqlite.exe("deepblue.db","INSERT INTO people (id_bigbrother) VALUES('3')")
	proteo.sqlite.exe("deepblue.db","INSERT INTO people (id_bigbrother) VALUES('4')")

	proteo.sqlite.exe("deepblue.db",
		[[
		CREATE TABLE 'peoplegroups' (
		'id_peoplegroups' INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
		'id_group' INTEGER NOT NULL,
		'id_people' INTEGER NOT NULL
		);
		]]
		)

	proteo.sqlite.exe("deepblue.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(1,1)")
	proteo.sqlite.exe("deepblue.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(3,2)")
	proteo.sqlite.exe("deepblue.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(1,3)")
	proteo.sqlite.exe("deepblue.db","INSERT INTO peoplegroups (id_group,id_people) VALUES(3,4)")
	
end

if proteo.system.fileExist("bigbrother.db")==false then
	local id=proteo.ejdb.put("bigbrother.db","bigbrother",'{"name":"Tizio1", "surname":"Caio1","birth_date":"19770424","sex":"M", "pid":"PID001"}')--new doc
	id=proteo.ejdb.put("bigbrother.db","bigbrother",'{"name":"Tizio2", "surname":"Caio2","birth_date":"19770424","sex":"M", "pid":"PID002"}')--new doc
	id=proteo.ejdb.put("bigbrother.db","bigbrother",'{"name":"Tizio3", "surname":"Caio3","birth_date":"19770424","sex":"M", "pid":"PID003"}')--new doc
	id=proteo.ejdb.put("bigbrother.db","bigbrother",'{"name":"Tizio4", "surname":"Caio4","birth_date":"19770424","sex":"M", "pid":"PID004"}')--new doc
end
--=====================================



proteo.route.get("deepblue/info",

	function(username,permission,data,param) 
		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("deepblue/permissions",

	function(username,permission,data,param) 
		local permissions={}
		permissions[1]="deepblue:adduser"

		return json.encode(permissions)
	end
)

--GET "/deepblue/people/search/"..search.."/group/"..group.."/type/"..type
--GET "/deepblue/test/attenzione_visiva/2.0/bbid/"..currentBBID

proteo.route.get("deepblue/test/:testname/:testversion/bbid/:id",

	function(username,permission,data,param) 

        local info={}
        info["message"]="0"
        info["tests"]={}
        info["tests"][1]={}
        info["tests"][1]["name"]="attenzione_visiva"
        info["tests"][1]["data"]=[[EN;20/01/2020-11:55:11;
ST;1579517711.2721;1/3;Animali;0;animali/leone.jpg;animali/cavallo.gif;animali/uccello.jpg;2;attenzione_visiva.data;
ES;1579517712.4296;3;YES;
ST;1579517712.4297;1/3;Animali;1;animali/leone.jpg;animali/cavallo.gif;animali/uccello.jpg;2;attenzione_visiva.data;
ES;1579517713.2313;3;YES;
ST;1579517713.2319;1/3;Animali;2;animali/uccello.jpg;animali/leone.jpg;animali/cavallo.gif;3;attenzione_visiva.data;
ES;1579517714.0422;4;YES;
END;YES]]

		return json.encode(info)
	end
)

proteo.route.put("deepblue/test",

	function(username,permission,data,param) 
    --send_data["data"]
    --send_data["name"]
    --send_data["version"]
    --send_data["bigbrotherID"]
    --send_data["test_effectued_from_user"]
		local info={}
        info["message"]="0"
        
        return json.encode(info)
	end
)

proteo.route.post("deepblue/test",

	function(username,permission,data,param) 
    --send_data["data"]
    --send_data["name"]
    --send_data["version"]
    --send_data["bigbrotherID"]
    --send_data["test_effectued_from_user"]
   		local info={}
        info["message"]="0"
        
        return json.encode(info)
	end
)

proteo.route.post("deepblue/add/people",

	function(username,permission,data,param) 

		local json_data=json.decode(data)
		local group=json_data["group"]
		json_data["group"]=nil
		
		local id=BB.newPeople(json_data)
		
		if id~=-1 then
			proteo.sqlite.exe("deepblue.db","INSERT INTO people (id_bigbrother) VALUES('"..id.."')")
			proteo.sqlite.exe("deepblue.db","INSERT INTO peoplegroups (id_group,id_people) SELECT "..group..",id_people FROM people WHERE id_bigbrother='"..id.."'")
		end
		
		--backup
		--proteo.netword.post("http://server.backup/deepblue/add/people",data,token,callback)
	end
)

proteo.route.get("deepblue/people/group/:group/type/:type",

	function(username,permission,data,param) 

		local people=proteo.sqlite.exe("deepblue.db","SELECT id_bigbrother FROM people,peoplegroups WHERE people.id_people=peoplegroups.id_people AND id_group='"..param["group"].."';")

		local test={}
		if people~=nil then

			test["message"]="0"
			test["people"]={}

			for i=1,#people do
				local data=BB.getPeople(people[i]["id_bigbrother"])
				test["people"][tostring(people[i]["id_bigbrother"])]=data.pid--people[i]["id_bigbrother"]
			end
		else
			test["message"]="1"
		end

		return json.encode(test)
	end
)

proteo.route.get("deepblue/groups",

	function(username,permission,data,param) 

		local groups=proteo.sqlite.exe("deepblue.db","SELECT groups.id_group,name FROM groups,usergroups WHERE groups.id_group=usergroups.id_group AND username='"..username.."';")

		local test={}
		if groups~=nil then
			test["message"]="0"
			test["groups"]={}
			for i=1,#groups do
				--print("Group: "..groups[i]["id_group"] .." "..groups[i]["name"])
				test["groups"][tostring(groups[i]["id_group"])]=groups[i]["name"]
			end
		else
			test["message"]="1"
		end
		--print("GROUP")
		return json.encode(test)
	end
)
