

	-- TODO "require" richiede il token che durante il login ancora non è valido
	-- local md5 = require 'md5'

	local label_username=nil 
	local text_username=nil
	local label_password=nil
	local text_password=nil
	local label_script=nil
	local text_script=nil
	local btn_login=nil
	local config=nil
	
	function init()

		--TODO
		collectgarbage('stop') --prevent GC

		proteo.graphics.setBackground("dimgray")
		config=proteo.system.loadConfig()

		form_login=proteo.gui.newForm("form_login","Login",'Colaborate',20,"black","crimson","blanchedalmond",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideResize+proteo.gui.FormType.HideClose,"","",MID_X-150,MID_Y-200,300,400,"")
    	register_login=proteo.gui.newForm("register_login","Register",'Colaborate',20,"black","crimson","blanchedalmond",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideResize+proteo.gui.FormType.HideClose,"","",MID_X-150,MID_Y-200,300,400,"")
    	home_login=proteo.gui.newForm("home_login","Proteo",'Colaborate',20,"black","crimson","blanchedalmond",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideResize+proteo.gui.FormType.HideClose,"","",MID_X-150,MID_Y-200,300,400,"")
    
    	--LOGIN
		label_username=proteo.gui.newLabel('label_username','Username:','Colaborate',15,"black",'clear',proteo.gui.LabelAlignment.Left,10,75,150,25)
		text_username=proteo.gui.newTextField('text_username',config.username,'Colaborate',15,'black','white',10,100,280,25,'')

		label_password=proteo.gui.newLabel('label_password','Password:','Colaborate',15,"black",'clear',proteo.gui.LabelAlignment.Left,10,150,150,25)
		text_password=proteo.gui.newTextField('text_password',config.password,'Colaborate',15,'black','white',10,175,150,25,'')

		label_script=proteo.gui.newLabel('label_script','Script:','Colaborate',15,"black",'clear',proteo.gui.LabelAlignment.Left,10,225,150,25)
		text_script=proteo.gui.newTextField('text_script',config.script,'Colaborate',15,'black','white',10,250,150,25,'')

		btn_login=proteo.gui.newButton('btn_login','Login','Colaborate',15,"black","cornsilk",1,"crimson",false,175,350,100,25,"login")
		btn_home=proteo.gui.newButton('btn_home','Back','Colaborate',15,"black","cornsilk",1,"crimson",false,25,350,100,25,"tohome")

		btn_getscript=proteo.gui.newButton('btn_getscript','Get','Colaborate',15,"black","cornsilk",1,"crimson",false,175,250,100,25,"getscripts")
		btn_forgotpassword=proteo.gui.newButton('btn_forgotpassword','Forgot','Colaborate',15,"black","cornsilk",1,"crimson",false,175,175,100,25,"forgotpassword")

		proteo.gui.setState(text_password,proteo.gui.TextFieldType.Password)

		proteo.gui.addItem(form_login,btn_login)
		proteo.gui.addItem(form_login,btn_home)
		proteo.gui.addItem(form_login,btn_getscript)
		proteo.gui.addItem(form_login,btn_forgotpassword)
		proteo.gui.addItem(form_login,label_username)
		proteo.gui.addItem(form_login,text_username)
		proteo.gui.addItem(form_login,label_password)
		proteo.gui.addItem(form_login,text_password)
		proteo.gui.addItem(form_login,label_script)
		proteo.gui.addItem(form_login,text_script)
		
		--REGISTER
		btn_register=proteo.gui.newButton('btn_login','Register','Colaborate',15,"black","cornsilk",1,"crimson",false,175,350,100,25,"register")
		btn_home2=proteo.gui.newButton('btn_home2','Back','Colaborate',15,"black","cornsilk",1,"crimson",false,25,350,100,25,"tohome")
		label_user_reg=proteo.gui.newLabel('label_user_reg','Username:','Colaborate',15,"black",'clear',proteo.gui.LabelAlignment.Left,10,75,150,25)
		text_user_reg=proteo.gui.newTextField('text_user_reg',"",'Colaborate',15,'black','white',10,100,280,25,'checkmail')

		label_accept=proteo.gui.newLabel('label_accept','Accept....','Colaborate',15,"black",'clear',proteo.gui.LabelAlignment.Left,10,200,150,25)
		check_accept=proteo.gui.newCheckbox('check_accept',"crimson","dimgray",proteo.gui.CheckboxType.CrossSquare,10,175,25,25,'')

		proteo.gui.addItem(register_login,btn_register)
		proteo.gui.addItem(register_login,label_user_reg)
		proteo.gui.addItem(register_login,text_user_reg)
		proteo.gui.addItem(register_login,label_accept)
		proteo.gui.addItem(register_login,check_accept)
		proteo.gui.addItem(register_login,btn_home2)

		--HOME
		label_welcome=proteo.gui.newLabel('label_welcome','Welcome!','Colaborate',30,"crimson",'clear',proteo.gui.LabelAlignment.Center,50,50,200,50)
		to_login=proteo.gui.newButton('to_login','Login','Colaborate',15,"black","cornsilk",1,"crimson",false,100,275,100,25,"tologin")
		to_register=proteo.gui.newButton('to_register','Register','Colaborate',15,"black","cornsilk",1,"crimson",false,100,350,100,25,"toregister")

		proteo.gui.addItem(home_login,label_welcome)
		proteo.gui.addItem(home_login,to_login)
		proteo.gui.addItem(home_login,to_register)
		--TODO saveconfig checkbox
    
    	tohome(btn_home)
    end
    
    function update(dt)
    
    end
    
    function tologin(sender)
    	proteo.gui.setHidden(form_login,false)
		proteo.gui.setHidden(home_login,true)
		proteo.gui.setHidden(register_login,true)
    end

    function toregister(sender)
    	proteo.gui.setHidden(form_login,true)
		proteo.gui.setHidden(home_login,true)
		proteo.gui.setHidden(register_login,false)
	end

	function tohome(sender)
    	proteo.gui.setHidden(form_login,true)
		proteo.gui.setHidden(home_login,false)
		proteo.gui.setHidden(register_login,true)
	end

    function login(sender)
    	config["username"]=proteo.gui.getText(text_username)
    	config["password"]=proteo.gui.getText(text_password)
    	config["script"]=proteo.gui.getText(text_script)
    	proteo.system.saveConfig(config)
    	
    	proteo.gui.setHidden(label_username,true)
    	proteo.gui.setHidden(text_username,true)
    	proteo.gui.setHidden(label_password,true)
    	proteo.gui.setHidden(text_password,true)
    	proteo.gui.setHidden(label_script,true)
    	proteo.gui.setHidden(text_script,true)
    	--proteo.gui.setHidden(btn_login,true)
    	proteo.gui.setHidden(form_login,true)

    	collectgarbage('restart')
    	collectgarbage('collect')

    	--print(md5.Calc(config["password"]))
    	proteo.system.login(config["username"],config["password"],config["script"])
	end

	function register(sender)
		local username=proteo.gui.getText(text_user_reg)
		data='{"username":"'..username..'","script":"parentme"}'
		proteo.network.proteo_post("/register",data,"register_callback")
	end

	function register_callback(res,data)
		proteo.gui.setText(text_username,data.username)
		proteo.gui.setHidden(form_login,false)
		proteo.gui.setHidden(home_login,true)
		proteo.gui.setHidden(register_login,true)
	end

	function checkmail(sender)

	end
