
local admin={}

admin.getUserid=function(username)
	local user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT id_user FROM users WHERE username='"..username.."'")

	return user[1]["id_user"]
end

admin.getUserByName=function(username)
	local user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT * FROM users WHERE username='"..username.."'")
	local permissions=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT app,id_permission,permission FROM permissions WHERE permissions.id_user = "..user[1]["id_user"])
		
	local ret={}
	ret.id_user=user[1]["id_user"]
	ret.username=user[1]["username"]
	ret.cancelled=user[1]["cancelled"]
	ret.permissions={}
	if permissions~=nil then
		for i=1,#permissions do
			table.insert(ret.permissions,{app=permissions[i].app,permission=permissions[i].permission,id_permission=permissions[i].id_permission})
		end
	end

	return ret
end

admin.getUserById=function(userid)
	--local doc=proteo.ejdb.get("bigbrother.db","bigbrother",tonumber(id))
	--return json.decode(doc)
	local user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT * FROM users WHERE id_user="..userid)
	local permissions=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT app,id_permission,permission FROM permissions WHERE permissions.id_user = "..userid)
		
	local ret={}
	ret.id_user=user[1]["id_user"]
	ret.username=user[1]["username"]
	ret.cancelled=user[1]["cancelled"]
	ret.permissions={}
	if permissions~=nil then
		for i=1,#permissions do
			table.insert(ret.permissions,{app=permissions[i].app,permission=permissions[i].permission,id_permission=permissions[i].id_permission})
		end
	end

	return ret
end

admin.getUserByIdForApp=function(userid,app)
	--local doc=proteo.ejdb.get("bigbrother.db","bigbrother",tonumber(id))
	--return json.decode(doc)
	local user=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT * FROM users WHERE id_user="..userid)
	local permissions=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT app,id_permission,permission FROM permissions WHERE permissions.id_user = "..userid.." AND permissions.app='"..app.."'")
		
	local ret={}
	ret.id_user=user[1]["id_user"]
	ret.username=user[1]["username"]
	ret.cancelled=user[1]["cancelled"]
	ret.permissions={}
	if permissions~=nil then
		for i=1,#permissions do
			table.insert(ret.permissions,{app=permissions[i].app,permission=permissions[i].permission,id_permission=permissions[i].id_permission})
		end
	end

	return ret
end

admin.addUser=function(user,currentid)
	local cancelled = user.cancelled and 1 or 0
	local sql="INSERT INTO `users` ('username','password','created_by','updated_by','cancelled') VALUES ('"..user.username.."','"..user.password.."',"..currentid..","..currentid..","..cancelled..")"
 	res,last_id=proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)

 	for i=1,#user.permissions do
 		sql = "INSERT INTO `permissions` ('id_user','app','permission','updated_by','date_updated') VALUES ("..last_id..",'"..user.permissions[i].app.."','"..user.permissions[i].permission.."',"..currentid..",CURRENT_TIMESTAMP);"
 		proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)
 	end
end

admin.updatePermissionForApp=function(id_user,new_permissions,app,currentid)

	local old_permissions=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT app,id_permission,permission FROM permissions WHERE permissions.id_user = "..id_user.." AND permissions.app='"..app.."'" )
	
	if old_permissions~=nil then
		for i=1,#old_permissions do
			local delete=true
			if new_permissions~=nil then
				for j=1,#new_permissions do
					if old_permissions[i].permission == new_permissions[j].permission then delete=false end
				end
			end
			
			if delete then 
				local sql="DELETE FROM `permissions` WHERE id_permission="..old_permissions[i].id_permission
				proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)
			end
		end
	end

	if new_permissions~=nil then
		for j=1,#new_permissions do
			local add=true
			if old_permissions~=nil then
				for i=1,#old_permissions do
					if old_permissions[i].permission == new_permissions[j].permission then add=false end
				end
			end

			if add then 
				sql = "INSERT INTO `permissions` ('id_user','app','permission','updated_by','date_updated') VALUES ("..id_user..",'"..app.."','"..new_permissions[j].permission.."',"..currentid..",CURRENT_TIMESTAMP);"
				proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)
			end
		end
	end
	--if permissions~=nil then
	--	for i=1,#permissions do
	--		table.insert(ret.permissions,{app=permissions[i].app,permission=permissions[i].permission,id_permission=permissions[i].id_permission})
	--	end
	--end
end

admin.updateUser=function(user,currentid)
	local update_password=""
	if user.password~=nil and user.password~="" then
		update_password="', 'password' = '"..user.password
	end
	local cancelled = user.cancelled and 1 or 0
	local sql="UPDATE 'users' SET 'username' = '"..user.username..update_password.."', 'updated_by' = "..currentid..",'date_updated' = CURRENT_TIMESTAMP, 'cancelled'="..cancelled.." WHERE id_user="..user.id_user

	--print(sql)

	proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)

	sql="DELETE FROM `permissions` WHERE id_user="..user.id_user
	proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)

	for i=1,#user.permissions do
 		sql = "INSERT INTO `permissions` ('id_user','app','permission','updated_by','date_updated') VALUES ("..user.id_user..",'"..user.permissions[i].app.."','"..user.permissions[i].permission.."',"..currentid..",CURRENT_TIMESTAMP);"
 		proteo.sqlite.exe(BASEDIR.."auth_user.db",sql)
 	end
end

admin.searchUser=function(search)

	local users=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT id_user,username FROM users ") --WHERE username LIKE %

	local ret={}
	ret.users=users
	
	return ret
end

admin.searchUserForApp=function(search,app)

	local users=proteo.sqlite.exe(BASEDIR.."auth_user.db","SELECT DISTINCT users.id_user,username FROM users, permissions WHERE permissions.id_user==users.id_user AND ( permissions.app='"..app.."' OR (permissions.app='proteo' AND permissions.permission='"..app.."'))")

	local ret={}
	ret.users=users
	
	return ret
end

return admin
