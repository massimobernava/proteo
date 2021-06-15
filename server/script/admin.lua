local md5 = require 'md5'
local json=require "json"

--TODO
-- delete checkbox
-- password mode per i textbox
-- avvisi per passord assende, sbagliata o user che non è una mail
-- search textbox
-- server REBOOT

-- REPOSITORY
-- A che serve PERMISSIONS?


admin={} --TODO una sola variabile globale

form_users=nil
--form_config=nil
form_permissions=nil

--option_button=nil
users_button=nil
permissions_button=nil

list_users=nil
list_plugins=nil
list_permissions=nil

config=nil

text_username=nil
text_password=nil
text_confirm=nil
check_delete=nil

selected_user=nil
selected_permission=nil
selected_user_permission=nil
save_button2=nil
new_button=nil
addpermission_button=nil
removepermission_button=nil

------------------
--+++++INIT+++++--
------------------

function init()

	proteo.graphics.setBackground("dimgray")
	
	title_admin=proteo.gui.newLabel('title_admin','Proteo admin','Helvetica',40,"blanchedalmond","clear",proteo.gui.LabelAlignment.Left,MIN_X+50,MIN_Y+50,300,40)

	username_admin=proteo.gui.newForm("username_admin",USERNAME,'Helvetica',20,"crimson","cadetgrey","cadetgrey",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideClose,"","",MAX_X-450,MIN_Y+50,400,60,"")
    logout_button=proteo.gui.newButton('logout_button','Logout...','Helvetica',10,'blanchedalmond','dimgray',1,"crimson",false,290,35,100,20,logout)
 	proteo.gui.addItem(username_admin,logout_button)
    proteo.gui.setState(username_admin,proteo.gui.FormState.WindowBar)

	form_users=proteo.gui.newForm("form_users","Users",'Helvetica',20,"jet","crimson","blanchedalmond",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideResize+proteo.gui.FormType.HideClose,"","",150,150,MAX_X-300,MAX_Y-300,"")
    admin.form_config=proteo.gui.newForm("form_config","Config",'Helvetica',20,"jet","crimson","blanchedalmond",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideResize+proteo.gui.FormType.HideClose,"","",150,150,MAX_X-300,MAX_Y-300,"")
    form_permissions=proteo.gui.newForm("form_permissions","Permissions",'Helvetica',20,"jet","crimson","blanchedalmond",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideResize+proteo.gui.FormType.HideClose,"","",150,150,MAX_X-300,MAX_Y-300,"")
    
    admin.config_button=proteo.gui.newButton('config_button','Config','Helvetica',20,'jet','blanchedalmond',1,"crimson",false,MIN_X,180,150,50,open_options)
 	users_button=proteo.gui.newButton('users_button','Users','Helvetica',20,'jet','blanchedalmond',1,"crimson",false,MIN_X,230,150,50,open_users)
 	permissions_button=proteo.gui.newButton('permissions_button','Permissions','Helvetica',20,'jet','blanchedalmond',1,"crimson",false,MIN_X,280,150,50,open_permissions)
 
 	search_user=proteo.gui.newTextField('search_user',"",'Helvetica',15,"black","white",50 ,50,200,25,"")
	search_button=proteo.gui.newButton('search_button',"Search",'Helvetica',15,"jet","blanchedalmond",1,"crimson",false,275 ,50,75,25,"search_users")
    
    label_username=proteo.gui.newLabel('label_username','Username:','Helvetica',15,"jet","clear",proteo.gui.LabelAlignment.Left,375,75,100,20)
    text_username=proteo.gui.newTextField('text_username',"",'Helvetica',15,"black","white",500 ,75,300,20,"")
    label_password=proteo.gui.newLabel('label_password','Password:','Helvetica',15,"jet","clear",proteo.gui.LabelAlignment.Left,375,100,100,20)
    text_password=proteo.gui.newTextField('text_password',"",'Helvetica',15,"black","white",500 ,100,300,20,"")
    label_confirm=proteo.gui.newLabel('label_confirm','Confirm:','Helvetica',15,"jet","clear",proteo.gui.LabelAlignment.Left,375,125,100,20)
    text_confirm=proteo.gui.newTextField('text_confirm',"",'Helvetica',15,"black","white",500 ,125,300,20,"")
    label_delete=proteo.gui.newLabel('label_delete','Delete:','Helvetica',15,"jet","clear",proteo.gui.LabelAlignment.Left,375,150,100,20)
    check_delete=proteo.gui.newCheckbox('check_delete',"crimson","dimgray",proteo.gui.CheckboxType.CrossSquare,500,150,20,20,'')
	proteo.gui.addItem(form_users,label_username)
 	proteo.gui.addItem(form_users,text_username)
	proteo.gui.addItem(form_users,label_password)
 	proteo.gui.addItem(form_users,text_password)
	proteo.gui.addItem(form_users,label_confirm)
 	proteo.gui.addItem(form_users,text_confirm)
	proteo.gui.addItem(form_users,label_delete)
	proteo.gui.addItem(form_users,check_delete)

    label_permissions=proteo.gui.newLabel('label_permissions','Permissions:','Helvetica',15,"jet","clear",proteo.gui.LabelAlignment.Left,375,200,100,25)
    list_permissions=proteo.gui.newList('list_permissions','Permissions','Helvetica',30,"blanchedalmond","dimgray",375,225,175,175,"select_permission")
    user_permissions=proteo.gui.newList('user_permissions','User permissions','Helvetica',30,"blanchedalmond","dimgray",700,225,175,175,"select_user_permission")
    addpermission_button=proteo.gui.newButton('addpermission_button',"Add >>",'Helvetica',15,"jet","blanchedalmond",1,"crimson",false,575 ,275,100,25,"add_permission")
    removepermission_button=proteo.gui.newButton('removepermission_button',"<< Remove",'Helvetica',15,"jet","blanchedalmond",1,"crimson",false,575 ,325,100,25,"remove_permission")
    save_button2=proteo.gui.newButton('save_button2',"Save",'Helvetica',20,"jet","blanchedalmond",1,"crimson",false,MAX_X-425 ,MAX_Y-350,100,25,"save_user_call")
 	new_button=proteo.gui.newButton('new_button',"New",'Helvetica',20,"jet","blanchedalmond",1,"crimson",false,MAX_X-950 ,MAX_Y-350,100,25,"new_user")

	proteo.gui.addItem(form_users,label_permissions)
	proteo.gui.addItem(form_users,list_permissions)
	proteo.gui.addItem(form_users,user_permissions)
	proteo.gui.addItem(form_users,addpermission_button)
	proteo.gui.addItem(form_users,removepermission_button)
	proteo.gui.addItem(form_users,save_button2)
	proteo.gui.addItem(form_users,new_button)

 	list_users=proteo.gui.newList('list_users','Users','Helvetica',30,"blanchedalmond","dimgray",50,100,300,325,"select_user")
 	proteo.gui.addItem(form_users,list_users)
 	proteo.gui.addItem(form_users,search_user)
 	proteo.gui.addItem(form_users,search_button)

 	label_plugins=proteo.gui.newLabel('label_plugins','Plugins:','Helvetica',20,"jet","clear",proteo.gui.LabelAlignment.Left,50,50,200,25)
 	list_plugins=proteo.gui.newList('list_plugins','Plugins','Helvetica',30,"blanchedalmond","dimgray",50,100,225,300,"")
 	addplugin_button=proteo.gui.newButton('addplugin_button',"Add",'Helvetica',20,"jet","blanchedalmond",1,"crimson",false,50 ,425,100,25,"add_plugin")
 	removeplugin_button=proteo.gui.newButton('removeplugin_button',"Remove",'Helvetica',20,"jet","blanchedalmond",1,"crimson",false,175 ,425,100,25,"remove_plugin")

 	save_button=proteo.gui.newButton('save_button',"Save",'Helvetica',20,"jet","blanchedalmond",1,"crimson",false,MAX_X-425 ,MAX_Y-350,100,25,"save_option")
 	proteo.gui.addItem(admin.form_config,label_plugins)
 	proteo.gui.addItem(admin.form_config,list_plugins)
	proteo.gui.addItem(admin.form_config,addplugin_button)
	proteo.gui.addItem(admin.form_config,removeplugin_button)
	proteo.gui.addItem(admin.form_config,save_button)

	proteo.network.proteo_get("/admin/config","config_callback")

 	open_options(admin.config_button)
end

--------------------
--+++++UPDATE+++++--
--------------------

function update(dt)
        
end

function keydown(k)

end

function keyup(k)

end

function release(x,y)

end

-------------------
--+++++LOCAL+++++--
-------------------
function logout(sender)
	proteo.system.login("","","login")
end

function open_options(sender)
	proteo.gui.setHidden(form_users,true)
	proteo.gui.setHidden(admin.form_config,false)
	proteo.gui.setHidden(form_permissions,true)

	proteo.gui.setColor(admin.config_button,'blanchedalmond')
	proteo.gui.setColor(users_button,'cadetgrey')
	proteo.gui.setColor(permissions_button,'cadetgrey')
end

function open_users(sender)
	proteo.gui.setHidden(form_users,false)
	proteo.gui.setHidden(admin.form_config,true)
	proteo.gui.setHidden(form_permissions,true)

	proteo.gui.setColor(admin.config_button,'cadetgrey')
	proteo.gui.setColor(users_button,'blanchedalmond')
	proteo.gui.setColor(permissions_button,'cadetgrey')

	proteo.gui.setText(save_button2,"Save")

	proteo.gui.setEnabled(save_button2,false)
	proteo.gui.setText(text_username,"")
	proteo.gui.setEnabled(text_username,false)
	proteo.gui.setEnabled(addpermission_button,false)
	proteo.gui.setEnabled(removepermission_button,false)
	proteo.gui.emptyList(user_permissions)

end

function open_permissions(sender)
	proteo.gui.setHidden(form_users,true)
	proteo.gui.setHidden(admin.form_config,true)
	proteo.gui.setHidden(form_permissions,false)

	proteo.gui.setColor(admin.config_button,'cadetgrey')
	proteo.gui.setColor(users_button,'cadetgrey')
	proteo.gui.setColor(permissions_button,'blanchedalmond')
end

function search_users(sender)
	local data={}
	data["search"]="*"
	proteo.network.proteo_post("/admin/search/users",json.encode(data),"search_callback")
end

function search_callback(res,data)
	proteo.gui.emptyList(list_users)
 	for i=1,#data.users do
        list_item_gui=proteo.gui.newListItem(data.users[i]["id_user"],data.users[i]["username"],'Helvetica',15,"jet","cadetgrey","","",300,30)
        proteo.gui.addItem(list_users,list_item_gui)
    end
end

function config_callback(res,data)
	config=data
	for i=1,#config.plugins do
		local list_item_plugin=proteo.gui.newListItem("list"..i.."_item_plugin",config.plugins[i],'Helvetica',20,"jet","cadetgrey","","",300,30)
    	proteo.gui.addItem(list_plugins,list_item_plugin)
    	proteo.network.proteo_get("/"..config.plugins[i].."/permissions","permissions_callback")
    end
end

function permissions_callback(res,data)
	for i=1,#data.permission do
		local list_item_permission=proteo.gui.newListItem("list"..i.."_item_permission",data.permission [i],'Helvetica',15,"jet","cadetgrey","","",175,20)
    	proteo.gui.addItem(list_permissions,list_item_permission)
    end
end

function select_user(selected)
	local username=proteo.gui.getText(selected)
    local userid=proteo.system.getName(selected)

    proteo.network.proteo_get("/admin/user/"..username,"get_user")
end

function select_permission(selected)
	selected_permission=proteo.gui.getText(selected)
end

function select_user_permission(selected)
	selected_user_permission=proteo.gui.getText(selected)
end

function show_user(data)
	proteo.gui.setText(text_username,data["username"])
	proteo.gui.setText(text_password,"")
	proteo.gui.setText(text_confirm,"")

	if data.cancelled==0 then proteo.gui.setState(check_delete,proteo.gui.CheckboxState.Unchecked)
	else proteo.gui.setState(check_delete,proteo.gui.CheckboxState.Checked) end

	proteo.gui.emptyList(user_permissions)
	for i=1,#data.permissions do
		local list_item_permission=proteo.gui.newListItem("list"..i.."_item_permission",data.permissions[i].app..":"..data.permissions[i].permission,'Helvetica',15,"jet","cadetgrey","","",175,20)
    	proteo.gui.addItem(user_permissions,list_item_permission)
    end
end

function get_user(res,data)

	--tprint(data)

	selected_user=data
	selected_user.action="update"
	proteo.gui.setText(save_button2,"Update")
	proteo.gui.setEnabled(save_button2,true)
	proteo.gui.setEnabled(text_username,true)
	proteo.gui.setEnabled(addpermission_button,true)
	proteo.gui.setEnabled(removepermission_button,true)
	show_user(selected_user)
end

function add_permission(sender)
	if selected_user~=nil and selected_permission~=nil then
		local p=split(selected_permission, ":")
		for i=1,#selected_user.permissions do
			if selected_user.permissions[i].app==p[1] and selected_user.permissions[i].permission==p[2] then 
				return
			end
		end

		table.insert(selected_user.permissions,{app=p[1],permission=p[2]})
		local list_item_permission=proteo.gui.newListItem("list"..#selected_user.permissions.."_item_permission",p[1]..":"..p[2],'Helvetica',15,"jet","cadetgrey","","",175,20)
    	proteo.gui.addItem(user_permissions,list_item_permission)
	end
end

function remove_permission(sender)
if selected_user~=nil and selected_user_permission~=nil then
		local p=split(selected_user_permission, ":")
		for i=1,#selected_user.permissions do
			if selected_user.permissions[i].app==p[1] and selected_user.permissions[i].permission==p[2] then 
				table.remove(selected_user.permissions,i)
				show_user(selected_user)
				return 
			end
		end
		
	end
end

function save_user_call(sender)

	print("Save User!")
	
	local password=proteo.gui.getText(text_password)
	local confirm=proteo.gui.getText(text_confirm)

	save_user={}
	--save_user.password=""
	if password~="" and password~=nil and password==confirm then
		save_user.password=md5.sumhexa(password)
	end
	save_user.username=proteo.gui.getText(text_username)
	save_user.cancelled=(proteo.gui.checkState(check_delete)==proteo.gui.CheckboxState.Checked)

	save_user.permissions={}

	for i=1,#selected_user.permissions do
		table.insert(save_user.permissions,{app=selected_user.permissions[i].app,permission=selected_user.permissions[i].permission})
	end

		
	if selected_user.action=="update" then
		save_user.id_user=selected_user.id_user
		proteo.network.proteo_post("/admin/update/user",json.encode(save_user),"save_user_callback")
	elseif selected_user.action=="add" then
		if save_user.password==nil then return end
		proteo.network.proteo_post("/admin/add/user",json.encode(save_user),"save_user_callback")
	end
end

function save_user_callback(sender)
 --IF OK disable all and delete all
end

function new_user(sender)

	selected_user={}
	selected_user.action="add"
	selected_user.username=""
	selected_user.permissions={}

	proteo.gui.setText(save_button2,"Add")

	proteo.gui.setEnabled(save_button2,true)
	proteo.gui.setEnabled(text_username,true)
	proteo.gui.setEnabled(addpermission_button,true)
	proteo.gui.setEnabled(removepermission_button,true)
	show_user(selected_user)
end

function save_option(sender)

end
-------------------
--+++++++++++++++--
-------------------

function split(s, sep)
    local fields = {}

    local sep = sep or " "
    local pattern = string.format("([^%s]+)", sep)
    string.gsub(s, pattern, function(c) fields[#fields + 1] = c end)

    return fields
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
