
local md5 = require 'md5'
local json = require "json"
local video_chat_game = require "video_chat"
local attenzione_visiva_game = require "attenzione_visiva"
local pose_analysis_game = require "pose_analysis"
local giochi_parole_game = require "giochi_parole"
local attenzione_uditiva_game = require "attenzione_uditiva"

--GAME
--  Nome
--  forms dove 1 è la form di partenza
-- init_game()

--enet_url="localhost:8888"

HOME=1
USERS=2
PEOPLE=3
FAMILY=4
GAMES=5
SETTING=6

local forms={}
local games={}
local left_buttons={}
local power=nil;

 enet_host=nil
 enet_server=nil
 event_timer=nil

local selected_user=nil;
local selected_game=0
local form_connect=nil
local form_wait=nil
local form_call=nil
local form_video=nil
local left_column=nil
local selected_group={}
local selected_user_group={}
local selected_people={}
local selected_people_group={}
local selected_user_people_group={}
local selected_family={}
local selected_connect={}

local current_permission={}

local request_connection_peer=nil
local request_connection_data=nil

local current_peer=nil
local current_role=nil
local connection_id=nil

------------------
--++++THEME+++++--
------------------

CNR={}
CNR["background"]="#002F5F"
CNR["title_back"]="Clear"
CNR["title_font"]="#B8DBD9"
CNR["button_font"]="#FBFBFF"
CNR["button_back"]="#20BF55"
CNR["alternate_row_back"]="#586F7C"
CNR["font_color"]="#FBFBFF"
CNR["check1_color"]="#208abf"
CNR["check2_color"]="#55bf20"
CNR["button_border"]="#586e75"

Umarell={}
Umarell["logo"]="crimson"
Umarell["light"]="gainsboro"
Umarell["medium"]="darkgrey"
Umarell["dark"]="dimgray"
Umarell["font"]="black"

Wonder={}
Wonder["logo"]="#BF2F1E"
Wonder["light"]="#F1E9E6"
Wonder["medium"]="#C5A78E"
Wonder["dark"]="#955432"
Wonder["font"]="#6E1A13"

current_theme=Umarell

------------------
--+++++INIT+++++--
------------------

function init()

	proteo.graphics.setBackground(current_theme["medium"])

	logo=proteo.graphics.newRect("logo",current_theme["logo"],current_theme["logo"],0,0,150,100)
	proteo.graphics.setLayer(logo,110)
	parentme=proteo.gui.newLabel('parentme','PARENTME','Helvetica',25,current_theme["light"],"clear",proteo.gui.LabelAlignment.Center,0,25,150,50)
	proteo.graphics.setLayer(parentme,120)

	--proteo.network.proteo_get("/parentme/mypermissions","permissions_callback")

	form_connect=create_connect()
	form_wait=create_waitconnection()
	form_call=create_call()
	form_video=create_video()
	proteo.graphics.setLayer(form_video.form,TOP)
	--la lista dei giochi va dentro permissions_callback


	create_header()
	--left_column=create_leftcolumn() --la creazione dei bottoni va dentro permissions_callback
	forms[HOME]=create_home()
	forms[USERS]=create_users()
	forms[PEOPLE]=create_people()
	forms[FAMILY]=create_family()
	--forms[GAMES]=create_games()
	forms[SETTING]=create_setting()
	
	

	proteo.gui.setHidden(form_connect.form,true)
	proteo.gui.setHidden(form_wait.form,true)
	proteo.gui.setHidden(form_call.form,true)
	proteo.gui.setHidden(form_video.form,true)

	proteo.network.proteo_post("/parentme/start","{}","start_callback") --TODO da testare
	proteo.network.proteo_get("/parentme/groups","get_groups_callback")
	proteo.network.proteo_get("/parentme/family","get_family_callback")

	open(HOME)
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

function touch(x,y)
	if proteo.graphics.isInside(power,x,y) then
		proteo.system.login("","","login")
	end

	if selected_game>0 then
		games[selected_game].touch_game(x,y)
	end
end

function release(x,y)

end

-------------------
--++++CREATE+++++--
-------------------

function create_header()
	header=proteo.gui.newForm("header","HEADER",'Helvetica',20,current_theme["logo"],current_theme["dark"],current_theme["dark"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,0,MAX_X,50,"")
	power=proteo.graphics.newIcon("power","power",current_theme["logo"],MAX_X-50,10,30,30)

	--proteo.graphics.setLayer(power,110)
	proteo.gui.addItem(header,power)

   return header
end

function create_leftcolumn(permissions)
	left=proteo.gui.newForm("left","LEFT",'Helvetica',20,current_theme["logo"],current_theme["light"],current_theme["light"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,100,MAX_Y-50,"")
  	
  	left_buttons[HOME]=proteo.gui.newButton('home_button','Home','Helvetica',20,current_theme["dark"],current_theme["medium"],1,current_theme["logo"],false,MIN_X,150,100,50,open_home)
  	proteo.gui.addItem(left,left_buttons[HOME]) 

  	local start_button_y=200
	for i=1,#permissions do
		if permissions[i]=="admin" then
			left_buttons[USERS]=proteo.gui.newButton('users_button','Users','Helvetica',20,current_theme["dark"],current_theme["medium"],1,current_theme["logo"],false,MIN_X,start_button_y,100,50,open_users)
			proteo.gui.addItem(left,left_buttons[USERS]) 
			start_button_y = start_button_y + 50
			break
		end
	end
	for i=1,#permissions do
		if permissions[i]=="researcher" then
			left_buttons[PEOPLE]=proteo.gui.newButton('people_button','People','Helvetica',20,current_theme["dark"],current_theme["medium"],1,current_theme["logo"],false,MIN_X,start_button_y,100,50,open_people)
  			proteo.gui.addItem(left,left_buttons[PEOPLE]) 
  			start_button_y = start_button_y + 50
  			break
		end
	end
	for i=1,#permissions do
		if permissions[i]=="user" then
			left_buttons[FAMILY]=proteo.gui.newButton('family_button','Family','Helvetica',20,current_theme["dark"],current_theme["medium"],1,current_theme["logo"],false,MIN_X,start_button_y,100,50,open_family)
  			proteo.gui.addItem(left,left_buttons[FAMILY]) 
  			start_button_y = start_button_y + 50
  			break
		end
	end
	--start_button_y = start_button_y + 50

  	left_buttons[GAMES]=proteo.gui.newButton('games_button','Games','Helvetica',20,current_theme["dark"],current_theme["medium"],1,current_theme["logo"],false,MIN_X,start_button_y,100,50,open_games)
  	proteo.gui.addItem(left,left_buttons[GAMES]) 
  	start_button_y = start_button_y + 50
  	left_buttons[SETTING]=proteo.gui.newButton('setting_button','Setting','Helvetica',20,current_theme["dark"],current_theme["medium"],1,current_theme["logo"],false,MIN_X,start_button_y,100,50,open_setting)
  	proteo.gui.addItem(left,left_buttons[SETTING]) 

  	version=proteo.gui.newLabel('version','v0.1','Helvetica',15,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,0,MAX_Y-50,100,50)

	proteo.graphics.setLayer(left,10);



  	--for i=1,#left_buttons do
  	--	if left_buttons[i] ~= nil then proteo.gui.addItem(left,left_buttons[i]) end
  	--end
  	--proteo.gui.addItem(left,left_buttons[USERS])
  	--proteo.gui.addItem(left,left_buttons[GAMES])
	--proteo.gui.addItem(left,left_buttons[SETTING])

   return left
end

function create_home()
	local form={}
	form.form=proteo.gui.newForm("welcome","WELCOME",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",100,50,MAX_X-100,MAX_Y-50,"")
  	
  	form.title_welcome=proteo.gui.newLabel('title_welcome','Benvenuti','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-200,50,300,30)

  	proteo.gui.addItem(form.form,form.title_welcome)

  	return form
end

function create_users()
	local form={}

	form.form=proteo.gui.newForm("users","USERS",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",100,50,MAX_X-100,MAX_Y-50,"")
  	--proteo.graphics.setLayer(users,10);

  	form.title_users=proteo.gui.newLabel('title_users','Users','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-200,50,300,30)

  	form.search_user=proteo.gui.newTextField('search_user',"",'Helvetica',15,current_theme["font"],"white",50 ,100,200,25,"")
	form.search_button=proteo.gui.newButton('search_button',"Search",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,275 ,100,75,25,"search_users")
  	form.list_users=proteo.gui.newList('list_users','Users','Helvetica',30,current_theme["light"],current_theme["dark"],50,150,300,325,"select_user")
 	form.label_user=proteo.gui.newLabel('label_user','User:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,150,100,25)
 	form.label_username=proteo.gui.newLabel('label_username','','Helvetica',15,current_theme["font"],"white",proteo.gui.LabelAlignment.Left,425,150,400,25)

 	form.label_permission=proteo.gui.newLabel('label_permission','Permissions:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,900,150,200,25)
 	form.admin_check=proteo.gui.newCheckbox('admin_check',current_theme["logo"],current_theme["dark"],proteo.gui.CheckboxType.CrossSquare,900,175,20,20,'')
	form.admin_label=proteo.gui.newLabel('admin_label','Admin','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,925,175,100,25)
	form.researcher_check=proteo.gui.newCheckbox('researcher_check',current_theme["logo"],current_theme["dark"],proteo.gui.CheckboxType.CrossSquare,900,205,20,20,'')
	form.researcher_label=proteo.gui.newLabel('researcher_label','Researcher','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,925,205,100,25)
	form.therapist_check=proteo.gui.newCheckbox('therapist_check',current_theme["logo"],current_theme["dark"],proteo.gui.CheckboxType.CrossSquare,900,235,20,20,'')
	form.therapist_label=proteo.gui.newLabel('therapist_label','Therapist','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,925,235,100,25)
	form.user_check=proteo.gui.newCheckbox('user_check',current_theme["logo"],current_theme["dark"],proteo.gui.CheckboxType.CrossSquare,900,265,20,20,'')
	form.user_label=proteo.gui.newLabel('user_label','User','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,925,265,100,25)

 	form.label_groups=proteo.gui.newLabel('label_groups','Groups:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,200,100,25)
    form.list_user_groups=proteo.gui.newList('list_user_groups','Groups','Helvetica',30,"blanchedalmond",current_theme["dark"],375,225,175,175,"select_group")
    form.user_groups=proteo.gui.newList('user_groups','User groups','Helvetica',30,"blanchedalmond",current_theme["dark"],700,225,175,175,"select_user_group")
    form.addgroup_button=proteo.gui.newButton('addgroup_button',"Add >>",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,575 ,275,100,25,"add_group")
    form.removegroup_button=proteo.gui.newButton('removegroup_button',"<< Remove",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,575 ,325,100,25,"remove_group")
    
    form.txt_user_group=proteo.gui.newTextField('txt_user_group',"",'Helvetica',15,current_theme["font"],"white",375 ,400,100,25,"")
	form.add_user_group=proteo.gui.newButton('add_user_group',"+",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,490 ,400,25,25,"add_user_group_call")
    form.remove_user_group=proteo.gui.newButton('remove_user_group',"-",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,525 ,400,25,25,"remove_user_group_call")

    form.update_user=proteo.gui.newButton('update_user',"Update",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"update_user_call")

 	proteo.gui.addItem(form.form,form.list_users)
 	proteo.gui.addItem(form.form,form.search_user)
 	proteo.gui.addItem(form.form,form.search_button)

 	proteo.gui.addItem(form.form,form.label_user)
 	proteo.gui.addItem(form.form,form.label_username)

 	proteo.gui.addItem(form.form,form.label_permission)
 	proteo.gui.addItem(form.form,form.admin_check)
 	proteo.gui.addItem(form.form,form.admin_label)
	proteo.gui.addItem(form.form,form.researcher_check)
 	proteo.gui.addItem(form.form,form.researcher_label)
	proteo.gui.addItem(form.form,form.therapist_check)
 	proteo.gui.addItem(form.form,form.therapist_label)
	proteo.gui.addItem(form.form,form.user_check)
 	proteo.gui.addItem(form.form,form.user_label)

 	proteo.gui.addItem(form.form,form.label_groups)
 	proteo.gui.addItem(form.form,form.list_user_groups)
 	proteo.gui.addItem(form.form,form.user_groups)
 	proteo.gui.addItem(form.form,form.addgroup_button)
 	proteo.gui.addItem(form.form,form.removegroup_button)

 	proteo.gui.addItem(form.form,form.update_user)

	proteo.gui.addItem(form.form,form.txt_user_group)
	proteo.gui.addItem(form.form,form.add_user_group)
	proteo.gui.addItem(form.form,form.remove_user_group)

  	proteo.gui.addItem(form.form,form.title_users)

  	return form
end

function create_people()
	local form={}
	form.form=proteo.gui.newForm("people","PEOPLE",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",100,50,MAX_X-100,MAX_Y-50,"")
  	
  	form.title_people=proteo.gui.newLabel('title_people','People','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-200,50,300,30)

	form.search_people=proteo.gui.newTextField('search_people',"",'Helvetica',15,current_theme["font"],"white",50 ,100,200,25,"")
	form.search_people_button=proteo.gui.newButton('search_people_button',"Search",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,275 ,100,75,25,"search_people")
  	form.list_people=proteo.gui.newList('list_people','Users','Helvetica',30,current_theme["light"],current_theme["dark"],50,150,300,325,"select_people")
 	form.label_user=proteo.gui.newLabel('label_user','Name:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,150,100,25)
 	form.label_people_username=proteo.gui.newLabel('label_users_username','','Helvetica',15,current_theme["dark"],"white",proteo.gui.LabelAlignment.Left,425,150,400,25)

 	form.label_people_groups=proteo.gui.newLabel('label_people_groups','Groups:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,200,100,25)
    form.list_people_groups=proteo.gui.newList('list_people_groups','Groups','Helvetica',30,"blanchedalmond",current_theme["dark"],375,225,175,175,"select_people_group")
    form.user_people_groups=proteo.gui.newList('user_people_groups','User groups','Helvetica',30,"blanchedalmond",current_theme["dark"],700,225,175,175,"select_user_people_group")
    form.addgroup_people_button=proteo.gui.newButton('addgroup_people_button',"Add >>",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,575 ,275,100,25,"add_group_people")
    form.removegroup_people_button=proteo.gui.newButton('removegroup_people_button',"<< Remove",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,575 ,325,100,25,"remove_group_people")
    
    form.update_people=proteo.gui.newButton('update_people',"Update",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,775 ,450,100,25,"update_people_call")

 	proteo.gui.addItem(form.form,form.list_people)
 	proteo.gui.addItem(form.form,form.search_people)
 	proteo.gui.addItem(form.form,form.search_people_button)
	proteo.gui.addItem(form.form,form.label_user)
	proteo.gui.addItem(form.form,form.label_people_username)

 	proteo.gui.addItem(form.form,form.label_people_groups)
 	proteo.gui.addItem(form.form,form.list_people_groups)
 	proteo.gui.addItem(form.form,form.user_people_groups)
 	proteo.gui.addItem(form.form,form.addgroup_people_button)
 	proteo.gui.addItem(form.form,form.removegroup_people_button)

	proteo.gui.addItem(form.form,form.update_people)

  	proteo.gui.addItem(form.form,form.title_people)

  	return form
end

function create_family()
	local form={}
	form.form=proteo.gui.newForm("family","FAMILY",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",100,50,MAX_X-100,MAX_Y-50,"")
  	
  	form.title_family=proteo.gui.newLabel('title_family','Family','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-200,50,300,30)

	form.list_family=proteo.gui.newList('list_family','Users','Helvetica',30,current_theme["light"],current_theme["dark"],50,150,300,525,"select_family")
 	form.label_name=proteo.gui.newLabel('label_name','Nome:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,150,100,25)
 	form.txt_name=proteo.gui.newTextField('txt_name',"",'Helvetica',15,current_theme["font"],"white",550 ,150,400,25,"")
	
	form.label_surname=proteo.gui.newLabel('label_surname','Cognome:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,180,100,25)
 	form.txt_surname=proteo.gui.newTextField('txt_surname',"",'Helvetica',15,current_theme["font"],"white",550 ,180,400,25,"")
	
	form.label_birthdate=proteo.gui.newLabel('label_birthdate','Data di nascita:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,210,100,25)
 	form.txt_birthdate=proteo.gui.newTextField('txt_birthdate',"__/__/___",'Helvetica',15,current_theme["font"],"white",550 ,210,100,25,"")
	
	form.label_F=proteo.gui.newLabel('label_F','F:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,665,210,25,25)
	form.F_check=proteo.gui.newCheckbox('F_check',current_theme["logo"],current_theme["dark"],proteo.gui.CheckboxType.SimpleSquare,685,212,20,20,'')
	form.label_M=proteo.gui.newLabel('label_M','M:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,715,210,25,25)
	form.M_check=proteo.gui.newCheckbox('M_check',current_theme["logo"],current_theme["dark"],proteo.gui.CheckboxType.SimpleSquare,735,212,20,20,'')
	proteo.gui.linkCheckbox(form.F_check,form.M_check)

 	form.label_phone=proteo.gui.newLabel('label_phone','Telefono:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,775,210,100,25)
 	form.txt_phone=proteo.gui.newTextField('txt_phone',"",'Helvetica',15,current_theme["font"],"white",850 ,210,100,25,"")

 	form.label_role=proteo.gui.newLabel('label_role','Membro della famiglia:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,240,100,25)
 	form.drop_role=proteo.gui.newDropDown('drop_role',"Seleziona...",'Helvetica',15,current_theme["font"],"","",proteo.gui.DropDownType.Normal,"white",550 ,240,200,25,"")
	
	drop_role_item1=proteo.gui.newDropDownItem('role1',"Madre",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_role_item2=proteo.gui.newDropDownItem('role2',"Padre",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_role_item3=proteo.gui.newDropDownItem('role3',"Nonno/a",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_role_item4=proteo.gui.newDropDownItem('role4',"Zio/a",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_role_item5=proteo.gui.newDropDownItem('role5',"Fratello/Sorella",'Helvetica',15,current_theme["font"],"white","","",200,30)
    proteo.gui.addItem(form.drop_role,drop_role_item1)
    proteo.gui.addItem(form.drop_role,drop_role_item2)
    proteo.gui.addItem(form.drop_role,drop_role_item3)
    proteo.gui.addItem(form.drop_role,drop_role_item4)
    proteo.gui.addItem(form.drop_role,drop_role_item5)

    form.label_education=proteo.gui.newLabel('label_education','Livello di istruzione:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,270,100,25)
 	form.drop_education=proteo.gui.newDropDown('drop_education',"Seleziona...",'Helvetica',15,current_theme["font"],"","",proteo.gui.DropDownType.Normal,"white",550 ,270,200,25,"")
	proteo.graphics.setLayer(form.drop_education,50)

	drop_education_item1=proteo.gui.newDropDownItem('edu1',"Scuola primaria",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_education_item2=proteo.gui.newDropDownItem('edu2',"Scuola secondaria inferiore",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_education_item3=proteo.gui.newDropDownItem('edu3',"Scuola secondaria superiore",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_education_item4=proteo.gui.newDropDownItem('edu4',"Laurea triennale",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_education_item5=proteo.gui.newDropDownItem('edu5',"Laurea magistrale",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_education_item6=proteo.gui.newDropDownItem('edu6',"Master post laurea",'Helvetica',15,current_theme["font"],"white","","",200,30)
    drop_education_item7=proteo.gui.newDropDownItem('edu7',"Dottorato",'Helvetica',15,current_theme["font"],"white","","",200,30)
    proteo.gui.addItem(form.drop_education,drop_education_item1)
    proteo.gui.addItem(form.drop_education,drop_education_item2)
    proteo.gui.addItem(form.drop_education,drop_education_item3) 
    proteo.gui.addItem(form.drop_education,drop_education_item4) 
    proteo.gui.addItem(form.drop_education,drop_education_item5) 
    proteo.gui.addItem(form.drop_education,drop_education_item6)
    proteo.gui.addItem(form.drop_education,drop_education_item7)  

    form.label_work=proteo.gui.newLabel('label_work','Occupazione:','Helvetica',15,current_theme["font"],"clear",proteo.gui.LabelAlignment.Left,375,300,100,25)
 	form.drop_work=proteo.gui.newDropDown('drop_work',"Seleziona...",'Helvetica',15,current_theme["font"],"","",proteo.gui.DropDownType.Normal,"white",550 ,300,200,25,"")
	proteo.graphics.setLayer(form.drop_work,25)  

	drop_work_item1=proteo.gui.newDropDownItem('drop_work_item1',"Disoccupato/casalinga",'Helvetica',15,current_theme["font"],"white","","",200,30)
	proteo.gui.addItem(form.drop_work,drop_work_item1)
    
    form.save_button=proteo.gui.newButton('save_button',"Save",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"save_family")
 	form.new_button=proteo.gui.newButton('new_button',"New",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,250 ,700,100,25,"new_family")

  	proteo.gui.addItem(form.form,form.title_family)
  	proteo.gui.addItem(form.form,form.list_family)
  	proteo.gui.addItem(form.form,form.label_name)
  	proteo.gui.addItem(form.form,form.txt_name)
	proteo.gui.addItem(form.form,form.label_surname)
  	proteo.gui.addItem(form.form,form.txt_surname)
	proteo.gui.addItem(form.form,form.label_birthdate)
  	proteo.gui.addItem(form.form,form.txt_birthdate)
	proteo.gui.addItem(form.form,form.label_phone)
  	proteo.gui.addItem(form.form,form.txt_phone)
	proteo.gui.addItem(form.form,form.label_role)
	proteo.gui.addItem(form.form,form.drop_role)
	proteo.gui.addItem(form.form,form.label_education)
	proteo.gui.addItem(form.form,form.drop_education)
	proteo.gui.addItem(form.form,form.label_work)
	proteo.gui.addItem(form.form,form.drop_work)
	proteo.gui.addItem(form.form,form.save_button)
	proteo.gui.addItem(form.form,form.new_button)

	proteo.gui.addItem(form.form,form.label_F)
	proteo.gui.addItem(form.form,form.F_check)
	proteo.gui.addItem(form.form,form.label_M)
	proteo.gui.addItem(form.form,form.M_check)

  	return form
end

function create_games()
	local form={}
	form.form=proteo.gui.newForm("games","GAMES",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",100,50,MAX_X-100,MAX_Y-50,"")
  	
  	form.title_games=proteo.gui.newLabel('title_games','Games','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-200,50,300,30)

  	form.game_buttons={}
  	for i=1,#games do
  		--print(games[i].name)
  		form.game_buttons[i]=proteo.gui.newButton('game_button'..i,games[i].name,'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,100+(i-1)*200 ,150,150,150,game_start)
  		proteo.gui.addItem(form.form,form.game_buttons[i])
  	end

  	--form.game_button1=proteo.gui.newButton('game_button1',"Video Chat",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,100 ,150,150,150,"game_start")
 	--form.game_button2=proteo.gui.newButton('game_button2',"Attenzione Visiva",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,300 ,150,150,150,"game_start")
 	
  	proteo.gui.addItem(form.form,form.title_games)
	--proteo.gui.addItem(form.form,form.game_button1)
	--proteo.gui.addItem(form.form,form.game_button2)

  	return form
end

function create_connect()
	local form={}
	form.form=proteo.gui.newForm("connect","CONNECT",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")
  	
  	form.title_connect=proteo.gui.newLabel('title_connect','Connect with:','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)

	--form.search_people=proteo.gui.newTextField('search_people',"",'Helvetica',15,current_theme["font"],"white",50 ,100,200,25,"")
	form.update_button=proteo.gui.newButton('update_button',"Update",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,275 ,100,75,25,"update_connected")
  	form.list_connected=proteo.gui.newList('list_connected','Users','Helvetica',30,current_theme["light"],current_theme["dark"],50,150,300,325,"select_connect")
	
  	form.connect=proteo.gui.newButton('connect',"Connect",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,750 ,450,100,25,"connect_connect")
	form.exit=proteo.gui.newButton('exit',"Exit",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"exit_connect")

  	proteo.gui.addItem(form.form,form.title_connect)
	--proteo.gui.addItem(form.form,form.search_people)
	proteo.gui.addItem(form.form,form.update_button)
	proteo.gui.addItem(form.form,form.list_connected)

	proteo.gui.addItem(form.form,form.connect)
	proteo.gui.addItem(form.form,form.exit)

  	return form
end

function create_setting()
	local form={}
	form.form=proteo.gui.newForm("setting","SETTING",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",100,50,MAX_X-100,MAX_Y-50,"")
  	
  	form.title_setting=proteo.gui.newLabel('title_setting','Setting','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-200,50,300,30)
	
  	proteo.gui.addItem(form.form,form.title_setting)

  	return form
end

function create_waitconnection()
	local form={}
	form.form=proteo.gui.newForm("waitconnection","WAIT...",'Helvetica',20,current_theme["light"],current_theme["logo"],current_theme["dark"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideClose+proteo.gui.FormType.HideResize,"","",MID_X-100,MID_Y-100,200,200,"")
  	
  	form.title_wait=proteo.gui.newLabel('title_wait','Wait for connection','Helvetica',20,current_theme["light"],"clear",proteo.gui.LabelAlignment.Center,50,50,100,30)

  	proteo.gui.addItem(form.form,form.title_wait)

  	return form
end

function create_call()
	local form={}
	form.form=proteo.gui.newForm("waitconnection","CALL...",'Helvetica',20,current_theme["light"],current_theme["logo"],current_theme["dark"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideClose+proteo.gui.FormType.HideResize,"","",MID_X-100,MID_Y-100,200,200,"")
  	
  	form.title_call=proteo.gui.newLabel('title_call','Call for game','Helvetica',20,current_theme["light"],"clear",proteo.gui.LabelAlignment.Center,50,50,100,30)
	form.ok_button=proteo.gui.newButton('ok_button',"Ok",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,125 ,150,50,25,"ok_connected")
  	form.cancel_button=proteo.gui.newButton('cancel_button',"Cancel",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,25 ,150,50,25,"cancel_connected")
  	
  	proteo.gui.addItem(form.form,form.title_call)
	proteo.gui.addItem(form.form,form.ok_button)
	proteo.gui.addItem(form.form,form.cancel_button)

  	return form
end

function create_video()
	local form={}
	form.form=proteo.gui.newForm("video","CHAT",'Helvetica',20,current_theme["light"],current_theme["logo"],current_theme["dark"],30,proteo.gui.FormType.HideClose+proteo.gui.FormType.HideResize,"","",MIN_X+100,MAX_Y-400,200,400,"")
  	
  	form.opencv_frame=proteo.opencv.img()
  	form.enet_frame=proteo.opencv.img()
  	form.opencv_img=proteo.graphics.newImage("webcam","@webcam",150,175,50,50)
  	form.enet_img=proteo.graphics.newImage("enetcam","@enetcam",100,125,150,150)
  	proteo.graphics.setLayer(form.opencv_img,TOP)
    proteo.opencv.setImg(form.enet_frame,150,150,"#000000")

    form.opencv_cap=proteo.opencv.videocapture(0)
    proteo.opencv.setFrameSize(form.opencv_cap,640,480)
    proteo.opencv.setBufferSize(form.opencv_cap,3)
    --proteo.opencv.frame(form.opencv_cap,form.opencv_frame)
    
    form.timer=proteo.system.createTimer(300,opencv_event)
    --proteo.system.startTimer(form.timer)
    
	proteo.gui.addItem(form.form,form.opencv_img)
	proteo.gui.addItem(form.form,form.enet_img)

  	return form
end
-------------------
--+++++OPEN++++++--
-------------------

function open(id)
	for i=1,#forms do
		proteo.gui.setHidden(forms[i].form,true)
		if left_buttons[i]~= nil then proteo.gui.setColor(left_buttons[i],current_theme["light"]) end
	end

	proteo.gui.setHidden(forms[id].form,false)
	proteo.gui.setColor(left_buttons[id],current_theme["medium"])
end

function open_home(sender)
	open(HOME)
end

function open_users(sender)
	open(USERS)

	proteo.gui.setEnabled(forms[USERS].update_user,false)
	proteo.gui.setEnabled(forms[USERS].addgroup_button,false)
	proteo.gui.setEnabled(forms[USERS].removegroup_button,false)
end

function open_people(sender)
	open(PEOPLE)
end

function open_family(sender)
	open(FAMILY)

	selected_family={}
	selected_family.id_apollo=nil
	selected_family.id_people=nil
	selected_family.name=""
	selected_family.surname=""

	selected_family.groups={}

	show_family(selected_family)

	proteo.gui.setEnabled(forms[FAMILY].txt_name,false)
	proteo.gui.setEnabled(forms[FAMILY].txt_surname,false)
	proteo.gui.setEnabled(forms[FAMILY].save_button,false)
end

function open_games(sender)
	open(GAMES)
end

function open_setting(sender)
	open(SETTING)
end

-------------------
--+++CALLBACK++++--
-------------------

function opencv_event(dt)
    proteo.opencv.frame(form_video.opencv_cap,form_video.opencv_frame)
    proteo.graphics.changeImage(form_video.opencv_img,form_video.opencv_frame)

    local tmp=proteo.opencv.imencode(form_video.opencv_frame)
            
    local data={}
    data["type"]="VIDEO"
	data["data"]=tmp
	data["role"]=current_role
	data["connection_id"]=connection_id
	local js=json.encode(data)
			
    current_peer:send(js)
    enet_host:flush()

end

function update_event(dt)
    local event = enet_host:service(0)
    --print(event)
    if event then
        if event.type == "connect" then
            print("Connected to", event.peer)

            data={}
            data["type"]="REPORT"
			data["username"]=USERNAME
			if ispermitted(current_permission,"therapist") then
				data["role"]="therapist"
			else
				data["role"]="user"
			end
			js=json.encode(data)
			
            event.peer:send(js)

        elseif event.type =="disconnect" then
        	print("Disconnected from", event.peer)
        elseif event.type == "receive" then
        	--print("Got message: "..event.data)
        	data=json.decode(event.data)
        	if data["type"]=="CONNECT" then
				print("CONNECT: "..data["from"].." to: "..data["to"])
        		--USER receive CALL from data["from"]=username
        		--CONNECTION SEQ--
				-- 3A
				--proteo.gui.setHidden(forms[HOME].form,true)
				proteo.gui.setHidden(form_call.form,false)
				proteo.graphics.setLayer(form_call.form,TOP)
				request_connection_peer=event.peer
				request_connection_data=data
			elseif data["type"]=="CONNECTED" then
			print("CONNECTED: "..data["connection_id"].." to: "..data["to"])
        		
				--CONNECTION SEQ--
				-- 6
				
				for i=1,#games do
					if games[i].name==data["game"] then
						selected_game=i
					end
				end

				proteo.gui.setHidden(form_call.form,true)
				proteo.gui.setHidden(form_wait.form,true)
				proteo.gui.setHidden(form_video.form,false)
				proteo.system.startTimer(form_video.timer)
				hideForms()

				connection_id=data["connection_id"]
				
				if ispermitted(current_permission,"therapist") then
					role="therapist"
				else
					role="user"
				end
				current_peer=event.peer
				current_role=role

				games[selected_game].start_game(data["to"],event.peer,role,connection_id)
				
			elseif data["type"]=="GAME" then
				games[selected_game].command(data)
			elseif data["type"]=="VIDEO" then
				proteo.opencv.imdecode(data["data"],form_video.enet_frame)
				proteo.graphics.changeImage(form_video.enet_img,form_video.enet_frame)
			end
        end
    end
end 

function connect_callback(res,data)
		print("CONNECT URL:"..data["url"])
		enet_host = proteo.enet.host_create()
		enet_server = enet_host:connect(data["url"])
		event_timer=proteo.system.createTimer(10,update_event)
		proteo.system.startTimer(event_timer)
end

function start_callback(res,data)
	proteo.network.proteo_get("/parentme/mypermissions","permissions_callback")
end

function permissions_callback(res,data)

	current_permission=data.permission
	--TODO se sono user o therapist avviso il server che sono connesso
	-- e mi metto in attesa per vedere se mi arrivano richieste di connessione

	if ispermitted(current_permission,"therapist") or ispermitted(current_permission,"user") then
		proteo.network.proteo_get("/parentme/connect","connect_callback")
	end

	--ADMIN show USERS
	--RESEARCHER show PEOPLE
	--THERAPIST show GAMES
	--USER show FAMILY & GAMES

	video_chat_game.init_game()
	games[1]=video_chat_game

	attenzione_visiva_game.init_game()
	games[2]=attenzione_visiva_game

	attenzione_uditiva_game.init_game()
	games[3]=attenzione_uditiva_game

	pose_analysis_game.init_game()
	games[4]=pose_analysis_game

	giochi_parole_game.init_game()
	games[5]=giochi_parole_game

	left_column=create_leftcolumn(data.permission) 

forms[GAMES]=create_games()
end

function get_groups_callback(res,data)
	proteo.gui.emptyList(forms[USERS].list_user_groups)
	proteo.gui.emptyList(forms[PEOPLE].list_people_groups)
	proteo.gui.setText(forms[USERS].txt_user_group,"")

	for i=1,#data do
        list_item_user_group=proteo.gui.newListItem(data[i]["id_group"],data[i]["name"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
        proteo.gui.addItem(forms[USERS].list_user_groups,list_item_user_group)

        list_item_people_group=proteo.gui.newListItem(data[i]["id_group"],data[i]["name"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
        proteo.gui.addItem(forms[PEOPLE].list_people_groups,list_item_people_group)
    end
end

function get_family_callback(res,data)
	proteo.gui.emptyList(forms[FAMILY].list_family)
	for i=1,#data do
		if data[i]~=nil then
        	list_item_user_family=proteo.gui.newListItem(data[i]["id_people"],data[i]["name"].." "..data[i]["surname"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",300,30)
        	proteo.gui.addItem(forms[FAMILY].list_family,list_item_user_family)
        end
    end
end

function get_people_callback(res,data)
	proteo.gui.emptyList(forms[PEOPLE].list_people)
	for i=1,#data do
        list_item_people=proteo.gui.newListItem(data[i]["id_people"],data[i]["name"].." "..data[i]["surname"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",300,30)
        proteo.gui.addItem(forms[PEOPLE].list_people,list_item_people)
    end
end

function get_online_callback(res,data)
	proteo.gui.emptyList(form_connect.list_connected)
	for i=1,#data do
        list_item_conn=proteo.gui.newListItem(data[i]["id_people"],data[i]["name"].." "..data[i]["surname"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",300,30)
        proteo.gui.addItem(form_connect.list_connected,list_item_conn)
    end
end
-------------------
--+++++USERS+++++--
-------------------

function search_users(sender)
	local data={}
	data["search"]="*"
	proteo.network.proteo_post("/parentme/search/users",json.encode(data),"search_callback")
end

function search_callback(res,data)
	proteo.gui.emptyList(forms[USERS].list_users)
 	for i=1,#data.users do
        list_item_gui=proteo.gui.newListItem(data.users[i]["id_user"],data.users[i]["username"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",300,30)
        proteo.gui.addItem(forms[USERS].list_users,list_item_gui)
    end
end

function select_user(selected)
	local username=proteo.gui.getText(selected)
    local userid=proteo.system.getName(selected)

    proteo.network.proteo_get("/parentme/user/"..userid,"get_user")
end

function get_user(res,data)

	selected_user=data
	proteo.gui.setEnabled(forms[USERS].update_user,true)
	proteo.gui.setEnabled(forms[USERS].addgroup_button,true)
	proteo.gui.setEnabled(forms[USERS].removegroup_button,true)
	show_user(selected_user)
end

function show_user(user)
	proteo.gui.setText(forms[USERS].label_username,user["username"])
	proteo.gui.setState(forms[USERS].admin_check,proteo.gui.CheckboxState.Unchecked)
	proteo.gui.setState(forms[USERS].researcher_check,proteo.gui.CheckboxState.Unchecked)
	proteo.gui.setState(forms[USERS].therapist_check,proteo.gui.CheckboxState.Unchecked)
	proteo.gui.setState(forms[USERS].user_check,proteo.gui.CheckboxState.Unchecked)
	for i=1,#user.permissions do
		if user.permissions[i].app == "parentme" then
			if user.permissions[i].permission=="admin" then
				proteo.gui.setState(forms[USERS].admin_check,proteo.gui.CheckboxState.Checked)
			elseif user.permissions[i].permission=="researcher" then
				proteo.gui.setState(forms[USERS].researcher_check,proteo.gui.CheckboxState.Checked)
			elseif user.permissions[i].permission=="therapist" then
				proteo.gui.setState(forms[USERS].therapist_check,proteo.gui.CheckboxState.Checked)
			elseif user.permissions[i].permission=="user" then
				proteo.gui.setState(forms[USERS].user_check,proteo.gui.CheckboxState.Checked)
			end
		end
	end

	proteo.gui.emptyList(forms[USERS].user_groups)
	if user.groups ~= nil then
		for i=1,#user.groups do
			list_item_user_groups=proteo.gui.newListItem(user.groups[i]["id_group"],user.groups[i]["name"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
        	proteo.gui.addItem(forms[USERS].user_groups,list_item_user_groups)
		end
	end
end

function update_user_call(sender)
	update_user={}

	update_user.permissions={}

	if proteo.gui.checkState(forms[USERS].admin_check) == proteo.gui.CheckboxState.Checked then
		table.insert(update_user.permissions,{app="parentme",permission="admin"})
	end

	if proteo.gui.checkState(forms[USERS].researcher_check) == proteo.gui.CheckboxState.Checked then
		table.insert(update_user.permissions,{app="parentme",permission="researcher"})
	end

	if proteo.gui.checkState(forms[USERS].therapist_check) == proteo.gui.CheckboxState.Checked then
		table.insert(update_user.permissions,{app="parentme",permission="therapist"})
	end

	if proteo.gui.checkState(forms[USERS].user_check) == proteo.gui.CheckboxState.Checked then
		table.insert(update_user.permissions,{app="parentme",permission="user"})
	end

	--for i=1,#selected_user.permissions do
	--	table.insert(save_user.permissions,{app=selected_user.permissions[i].app,permission=selected_user.permissions[i].permission})
	--end

	update_user.groups=selected_user.groups
	update_user.username=selected_user.username
	update_user.id_user=selected_user.id_user
	proteo.network.proteo_post("/parentme/update/user",json.encode(update_user),"update_user_callback")

	--print(json.encode(update_user))

end

function update_user_callback(res,data)

end

function select_group(selected)
	selected_group.name=proteo.gui.getText(selected)
    selected_group.id=proteo.system.getName(selected)
end

function select_user_group(selected)
	selected_user_group.name=proteo.gui.getText(selected)
    selected_user_group.id=proteo.system.getName(selected)
end

function add_group(sender)
	if selected_user~=nil and selected_group.id~=nil then
		list_item_user_groups=proteo.gui.newListItem(selected_group.id,selected_group.name,'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
    	proteo.gui.addItem(forms[USERS].user_groups,list_item_user_groups)

    	if selected_user.groups==nil then selected_user.groups={} end
    	table.insert(selected_user.groups,{id_group=selected_group.id,name=selected_group.name})
    end
end

function remove_group(sender)
	if selected_user~=nil and selected_user_group.id~=nil then
		for i=1,#selected_user.groups do
			if selected_user.groups[i].id_group==tonumber(selected_user_group.id) then 
				table.remove(selected_user.groups,i)
				--proteo.gui.removeItem(forms[USERS].user_groups,selected_user_group.id) --TODO
				proteo.gui.emptyList(forms[USERS].user_groups)

				for i=1,#selected_user.groups do
					list_item_user_groups=proteo.gui.newListItem(selected_user.groups[i]["id_group"],selected_user.groups[i]["name"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
        			proteo.gui.addItem(forms[USERS].user_groups,list_item_user_groups)
				end

				return 
			end
		end
	end
end

function add_user_group_call(sender)

	local new_group=proteo.gui.getText(forms[USERS].txt_user_group)

	local data={}
	data["name"]=new_group
	proteo.network.proteo_post("/parentme/add/group",json.encode(data),"get_groups_callback")
end

function remove_user_group_call(sender)

    local data={}
	data["id"]=selected_group.id
	proteo.network.proteo_post("/parentme/delete/group",json.encode(data),"get_groups_callback")
end

-------------------
--++++PEOPLE+++++--
-------------------
function search_people(sender)
	local data={}
	data["search"]="*"
	proteo.network.proteo_post("/parentme/search/people",json.encode(data),"get_people_callback")
end

function select_people(selected)
    local people_id=proteo.system.getName(selected)
    proteo.network.proteo_get("/parentme/people/"..people_id,"get_people")
end

function get_people(res,data)
	selected_people=data
	show_people(selected_people)
end

function show_people(people)
	proteo.gui.setText(forms[PEOPLE].label_people_username,people["name"].." "..people["surname"])

	proteo.gui.emptyList(forms[PEOPLE].user_people_groups)
	if people.groups ~= nil then
		for i=1,#people.groups do
			list_item_people_groups=proteo.gui.newListItem(people.groups[i]["id_group"],people.groups[i]["name"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
        	proteo.gui.addItem(forms[PEOPLE].user_people_groups,list_item_people_groups)
		end
	end
end

function select_people_group(selected)
	selected_people_group.name=proteo.gui.getText(selected)
    selected_people_group.id=proteo.system.getName(selected)
end

function select_user_people_group(selected)
	selected_user_people_group.name=proteo.gui.getText(selected)
	selected_user_people_group.id=proteo.system.getName(selected)
end

function add_group_people(sender)
	if selected_people~=nil and selected_people_group.id~=nil then
		list_item_people_groups=proteo.gui.newListItem(selected_people_group.id,selected_people_group.name,'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
    	proteo.gui.addItem(forms[PEOPLE].user_people_groups,list_item_people_groups)

    	if selected_people.groups==nil then selected_people.groups={} end
    	table.insert(selected_people.groups,{id_group=selected_people_group.id,name=selected_people_group.name})
    end
end

function remove_group_people(sender)
	if selected_people~=nil and selected_user_people_group.id~=nil then
		for i=1,#selected_people.groups do
			if selected_people.groups[i].id_group==tonumber(selected_user_people_group.id) then 
				table.remove(selected_people.groups,i)

				proteo.gui.emptyList(forms[PEOPLE].user_people_groups)

				for i=1,#selected_people.groups do
					list_item_people_groups=proteo.gui.newListItem(selected_people.groups[i]["id_group"],selected_people.groups[i]["name"],'Helvetica',15,current_theme["font"],current_theme["light"],"","",175,30)
        			proteo.gui.addItem(forms[PEOPLE].user_people_groups,list_item_people_groups)
				end

				return 
			end
		end
	end
end

function update_people_call(sender)
	update_people={}

	update_people.groups=selected_people.groups
	update_people.name=selected_people.name
	update_people.surname=selected_people.surname
	update_people.id_people=selected_people.id_people
	proteo.network.proteo_post("/parentme/update/people",json.encode(update_people),"update_people_callback")
end

function update_people_callback(res,data)

end
-------------------
--++++FAMILY+++++--
-------------------

function select_family(selected)
	local peopleid=proteo.system.getName(selected)
	proteo.gui.setText(forms[FAMILY].save_button,"Update")
	proteo.network.proteo_get("/parentme/people/"..peopleid,"get_family")
end

function get_family(res,data)

	selected_family=data
	selected_family.action="update"
	
	proteo.gui.setEnabled(forms[FAMILY].txt_name,true)
	proteo.gui.setEnabled(forms[FAMILY].txt_surname,true)
	proteo.gui.setEnabled(forms[FAMILY].save_button,true)

	show_family(selected_family)
end

function show_family(people)
	proteo.gui.setText(forms[FAMILY].txt_name,people["name"])
	proteo.gui.setText(forms[FAMILY].txt_surname,people["surname"])
end

function new_family(sender)
	proteo.gui.setText(forms[FAMILY].save_button,"Add")
	
	proteo.gui.setEnabled(forms[FAMILY].txt_name,true)
	proteo.gui.setEnabled(forms[FAMILY].txt_surname,true)
	proteo.gui.setEnabled(forms[FAMILY].save_button,true)

	selected_family.action="add"
	selected_family.id_apollo=nil
	selected_family.id_people=nil
	selected_family.name=""
	selected_family.surname=""

	selected_family.groups={}

	show_family(selected_family)
end

function save_family(sender)
	
	local save_data_family={}

	save_data_family.name=proteo.gui.getText(forms[FAMILY].txt_name)
	save_data_family.surname=proteo.gui.getText(forms[FAMILY].txt_surname)

	save_data_family.groups=selected_family.groups

	if selected_family.action=="update" then
		save_data_family.id_people=selected_family.id_people
		save_data_family.id_apollo=selected_family.id_apollo
		proteo.network.proteo_post("/parentme/update/people",json.encode(save_data_family),"save_people_callback")
	elseif selected_family.action=="add" then

		proteo.network.proteo_post("/parentme/add/people",json.encode(save_data_family),"save_people_callback")
	end
end

function save_people_callback(res,data)
	proteo.network.proteo_get("/parentme/family","get_family_callback")

	selected_family={}
	selected_family.id_apollo=nil
	selected_family.id_people=nil
	selected_family.name=""
	selected_family.surname=""

	selected_family.groups={}

	show_family(selected_family)
end

-------------------
--+++CONNECTION+++--
-------------------

function update_connected(sender)
	proteo.network.proteo_get("/parentme/online","get_online_callback")
end

function select_connect(selected)
	selected_connect=proteo.system.getName(selected)
end

function exit_connect(sender)
	proteo.gui.setHidden(left_column,false)
	proteo.gui.setHidden(forms[GAMES].form,false)
	proteo.gui.setHidden(form_connect.form,true)
end

function connect_connect(sender)
	--CONNECTION SEQ--
	-- 1
	data={}
	data["game"]=games[selected_game].name
	data["to"]=selected_connect--.id_people
	--data["user"]=selected_connect.id_user
	proteo.network.proteo_post("/parentme/connect",json.encode(data),"connected_callback")
end

function connected_callback(res,data)

	--CONNECTION SEQ--
	-- 3B

	--open message form "wait for connection"
	proteo.gui.setHidden(form_connect.form,true)
	proteo.gui.setHidden(form_wait.form,false)

	--TODO bisogna passare il people_id e l' user_id
	--games[selected_game].start_game()
end

function ok_connected(sender)

	--l'utente ha accettatol la connessione 

	--CONNECTION SEQ--
	-- 4A

	js=json.encode(request_connection_data)
			
    request_connection_peer:send(js)
	--mando l'ok al server
end

function cancel_connected(sender)
	--l'utente ha rifiutato la connessione

	--si chiude la finestra e si manda il messaggio al server
end
-------------------
--+++++GAME++++++--
-------------------
function game_start(sender)

	--se therapist apriamo connect, altrimenti solo start_game

	for i=1,#games do
  		if forms[GAMES].game_buttons[i]==sender then
  			selected_game=i
  		end
  	end

   if ispermitted(current_permission,"therapist") then
		proteo.gui.setHidden(left_column,true)
		proteo.gui.setHidden(forms[GAMES].form,true)
		proteo.gui.setHidden(form_connect.form,false)
   elseif ispermitted(current_permission,"user") then
   		proteo.gui.setHidden(form_connect.form,true)
		--TODO bisogna passare il people_id e l' user_id
		games[selected_game].start_game()
	end

end

function exit_game(sender)
	proteo.gui.setHidden(left_column,false)
	proteo.gui.setHidden(forms[GAMES].form,false)
	proteo.gui.setHidden(form_connect.form,true)
	games[selected_game].exit_game()
	selected_game=0
end

function hideForms()
	proteo.gui.setHidden(left_column,true)
	for i=1,#forms do
		proteo.gui.setHidden(forms[i].form,true)
	end
end

-------------------
--++++UTILITY++++--
-------------------

function ispermitted(permissions,perm)

	for i=1,#permissions do
		--print("PERMISSION: "..permission[i])
		if permissions[i]==perm then return true end
	end

	return false
end

-------------------
--+++++++++++++++--
-------------------
