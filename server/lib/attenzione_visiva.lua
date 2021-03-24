
local json=require "json"

local attenzione_visiva={}

local catTitle={"Animali","Figure","Frutta","Giocattoli","Indumenti","Veicoli","Volti","Miste"}
local stimTitle={"1/3","1/5","1/7","2/5","2/7","2/9","3/7","3/9","3/11","4/9","4/11","4/13"}
local stimNTarget={1,1,1,2,2,2,3,3,3,4,4,4}

local img_es={} --ES FORM
local n_es=nil --ES FORM

local selected_cat=1
local selected_stim=1

local data_log=""
local data_cat={}

--------------------
--++++CURRENT+++++--
--------------------
currentBBID=nil
currentState={} --indica quante ripetizioni di un esercizio ci sono state currentState[categoria][esercizio]=ripetizioni

rip_es=0 --numero di ripetizioni dello stesso esercizio
rip_target=1 --numero di target da trovare dentro l'esercizio
corr_es={} --elenco delle risposte corrette
currentForm=0 -- 1= main, 2 = es,3 = wait

current_peopleid=0
current_peer=nil
current_role=nil
current_connection_id=0

--------------------
--+++++IMAGES+++++--
--------------------

local MX={0,-200,200,-400, 400}
local MY={-100,100,300}

local img_es_pos={}
for j=0,2 do
    for i=1,5 do
        img_es_pos[1+(5*j)+i] = {}
        img_es_pos[1+(5*j)+i].X = MID_X + MX[i]
        img_es_pos[1+(5*j)+i].Y = MID_Y + MY[1+j]+50
    end
end

attenzione_visiva.name="Attenzione Visiva"
attenzione_visiva.forms={}

local function create_es()
	local form={}

	form.form=proteo.gui.newForm("stimolo","STIMOLO",'Helvetica',20,current_theme["logo"],current_theme["medium"],"white",30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")
  	
  	form.n_es=proteo.gui.newLabel('n_es','1','Helvetica',15,'DarkGrey','Clear',proteo.gui.LabelAlignment.Center,MIN_X+10,MIN_Y+10,20,20)

  	form.img_es={}
  	form.img_es[1]=proteo.graphics.newImage("target_es",proteo.system.document()..'red_cross.png',MID_X,100+50,150,150)
  	
  	proteo.gui.addItem(form.form,form.img_es[1])
	proteo.gui.addItem(form.form,form.n_es)
  	
  	for i=2,16 do
        form.img_es[i]=proteo.graphics.newImage("img"..i.."_es",proteo.system.document()..'red_cross.png',img_es_pos[i].X,img_es_pos[i].Y,150,150)
        proteo.gui.addItem(form.form,form.img_es[i])
    end

    form.blue_rest_es={}
    form.blue_cross_es={}
    for i=1,4 do
     	--gli aiuti blu
    	form.blue_rest_es[i]=proteo.graphics.newRect("blue_rest"..i.."_es","#0000FF","#0000FF",MID_X,MID_Y,180,180)
     	proteo.gui.addItem(form.form,form.blue_rest_es[i])
    	--i segni blu di quando si indovina 
    	form.blue_cross_es[i]=proteo.graphics.newImage("blue_cross"..i.."_es",proteo.system.document()..'blue_cross.png',MID_X,MID_Y,190,190)
    	proteo.gui.addItem(form.form,form.blue_cross_es[i])
    end



  	return form
end

local function create_main()

	local form={}

	form.form=proteo.gui.newForm("attenzione_visiva","ATTENZIONEVISIVA",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")	
  	local title_attenzione_visiva=proteo.gui.newLabel('title_attenzione_visiva','NOME COGNOME','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)
	local exit=proteo.gui.newButton('exit',"Exit",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,650,100,25,"exit_game")

	form.ell_cat_attivis={}
	form.lbl_cat_attivis={}
	form.btn_st_attvis={}

	for j=1,8 do
        form.ell_cat_attivis[j]=proteo.graphics.newEllipse('ell_cat'..j..'_attivis',current_theme["logo"],"Clear",MIN_X +250,50 + 80+(j*60),25,25)
        form.lbl_cat_attivis[j]=proteo.gui.newLabel('lbl_cat'..j..'_attivis',catTitle[j],'Helvetica',25,current_theme["font"],"Clear",proteo.gui.LabelAlignment.Left,MIN_X + 280,50+60+(j*60),200,25)

        proteo.gui.addItem(form.form,form.ell_cat_attivis[j])
        proteo.gui.addItem(form.form,form.lbl_cat_attivis[j])

        form.btn_st_attvis[j]={}
        for i=1,12 do
            form.btn_st_attvis[j][i]=proteo.gui.newButton('btn_cat'..j..'_st'..i..'_attvis',stimTitle[i],'Helvetica',9,current_theme["font"],current_theme["light"],1,current_theme["light"],false,MIN_X + 240 +(i*55),50 + 90+(j*60),50,10,"")
        	proteo.gui.addItem(form.form,form.btn_st_attvis[j][i])
        end
    end

  	proteo.gui.addItem(form.form,title_attenzione_visiva)
	proteo.gui.addItem(form.form,exit)

    return form

end

local function create_wait()

	local form={}

	form.form=proteo.gui.newForm("wait","WAIT",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")	
  	local title_wait=proteo.gui.newLabel('title_wait','WAIT..','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)
	

  	proteo.gui.addItem(form.form,title_wait)

    return form

end

--=============================
--GET

function get_av_data(res,data)
	--print("Data: "..data)
	update_data_cat(selected_cat,data)

    selected_stim=get_selected_stim(selected_cat)

    if selected_stim==0 then 
        --Prova a freddo ?
        print("Prova a freddo")
        selected_stim=1
    end

    
	--local log_data="EN;" --Avvio dell'esercizio con data e ora
    --log_data=log_data..os.date("%d/%m/%Y-%I:%M:%S")..";"
    --print(log_data)
    --log(log_data)

	--rip_es=currentState[selected_cat][selected_stim]
    showEs(selected_cat,selected_stim)

    currentForm=2
	proteo.gui.setHidden(attenzione_visiva.forms[1].form,true)
	proteo.gui.setHidden(attenzione_visiva.forms[2].form,false)
end

function get_av_tests(res,data)
	--print("Data: "..data)

    local tests=data.tests
        for i=1,#tests do
            --print("B")
            for key,value in pairs(tests[i]) 
            do
                if key=="name" then print("TEST:"..value) 
                elseif key=="data" then
                	data_log=value
                    log_to_state(value)
                --elseif key=="_id" then
                --    currentTest=value
                --    print("CurrentTest: "..currentTest)
                --else
                --    print(key.."->"..value)
                end
            end
        end
        --finito di caricare il test aggiorni la pagina e la mostri
        setButtonColor()
    

	
	if current_role=="user" then
		proteo.gui.setHidden(attenzione_visiva.forms[3].form,false)
        currentForm=3
	else
		proteo.gui.setHidden(attenzione_visiva.forms[1].form,false)
        currentForm=1
	end
	
	proteo.gui.setHidden(attenzione_visiva.forms[2].form,true)
end


--=============================

function log_to_state(data_log)
    local data_lines=split(data_log,"\r\n")
    print("log_to_state:"..data_log)

    for j=1,#data_lines do
        local sline=split(data_lines[j],";")
        if sline[1]=="ST" then
            print("   Stimolo ->:"..sline[3])
            print("   Categoria ->:"..sline[4])
            print("   Ripetizioni ->:"..sline[5])
            for i=1,#catTitle do
                if catTitle[i]==sline[4] then
                    for j=1,#stimTitle do
                        if stimTitle[j]==sline[3] then
                            if currentState[i][j]==nil or tonumber(currentState[i][j])<(tonumber(sline[5])+1) then currentState[i][j]=(tonumber(sline[5])+1) end
                        end
                    end
                end
            end       
         end
    end
end


function setButtonColor()
    --print("setButtonColor")
    for i=1,#catTitle do
        --print(catTitle[i]..":")
        for j=1,#stimTitle do
            --print("-"..stimTitle[j]..":"..currentState[i][j])
            if currentState[i][j] == 0 then
                proteo.gui.setColor(attenzione_visiva.forms[1].btn_st_attvis[i][j],"#00FF00")
            elseif currentState[i][j] < 3 then
                proteo.gui.setColor(attenzione_visiva.forms[1].btn_st_attvis[i][j],"#FF0000")
            elseif currentState[i][j] >= 3 then
                proteo.gui.setColor(attenzione_visiva.forms[1].btn_st_attvis[i][j],"#0000FF")
            end
        end
    end
end

function empty_state()
    currentState={}
    for i=1,#catTitle do
        --proteo.gui.setText(state_load[i]," ")
        currentState[i]={}
        for j=1,#stimTitle do
            currentState[i][j]=0
        end
    end
end

--questa funzione si dovrebbe aprire anche lato paziente su comando remoto del terapista
function showEs(cat,stim)
  
  	--print("showEs: "..cat .." "..stim)
  	st_es=data_cat[cat][stim]
  	rip_es=currentState[cat][stim]

    local log_data="ST;" --Presentazione dello stimolo, con il tempo e tutta la descrizione dello stimolo
    log_data=log_data..proteo.system.clock()..";"
    log_data=log_data..stimTitle[stim]..";"
    log_data=log_data..catTitle[cat]..";"
    log_data=log_data..rip_es..";"

    --print("save start clock:"..proteo.system.clock())
    --print("save start stim:"..stimTitle[selected_stim])
    --print("save start rip:"..rip_es)

    proteo.graphics.changeImage(attenzione_visiva.forms[2].img_es[1],st_es[1])

    for i=1,#attenzione_visiva.forms[2].blue_rest_es do
        proteo.graphics.setHidden(attenzione_visiva.forms[2].blue_rest_es[i],true)
        --e i segni blu
        proteo.graphics.setHidden(attenzione_visiva.forms[2].blue_cross_es[i],true)
    end

    local tmp={}

    j=stimNTarget[stim] --numero di copie di target
    for i=1,j do
        tmp[i]=st_es[1]
    end
    j=j+1
    for i=2,#stimTitle do
        if st_es[i]~=nil then tmp[j]=st_es[i] j=j+1 end
    end

    tmp=shuffle(tmp)

	-- imposta le immagini corrette
    corr_es={}
    j=1
    for i=1,#tmp do
        log_data=log_data..tmp[i]..";"
        if tmp[i]==st_es[1] then 
            --pos=proteo.graphics.getPosition(img_es[i+1])
            proteo.graphics.setPosition(attenzione_visiva.forms[2].blue_rest_es[j],img_es_pos[i+1].X-90,img_es_pos[i+1].Y-90)
            --e i segni blu
            proteo.graphics.setPosition(attenzione_visiva.forms[2].blue_cross_es[j],img_es_pos[i+1].X,img_es_pos[i+1].Y)
            corr_es[j]=i 
            j=j+1 
        end --print("Corr: "..i) -- corr_es contiene tutte le risposte corrette
    end
    
    -- salva le informazioni nel log
    for i=1,#corr_es do
        log_data=log_data..corr_es[i]..";"
    end

	-- imposta le immagini (e nasconde quelle inutili)
    for i=1,15 do
    	--if tmp[i]~=nil then print("image: "..tmp[i]) end
        proteo.graphics.setHidden(attenzione_visiva.forms[2].img_es[i+1],tmp[i]==nil) 
        if tmp[i]~=nil then proteo.graphics.changeImage(attenzione_visiva.forms[2].img_es[i+1],tmp[i]) end
    end
    --log_data=log_data..currentDataFile..";"
    --print(log_data)
    log(log_data)

    --TODO send tmp & corr_es
end

--=============================

--------------------
--+++++UTILITY++++--
--------------------

function log(line)
    print(line)
    data_log=data_log..line.."\r\n"
end

function sleep(s)
  local ntime = os.clock() + s
  repeat until os.clock() > ntime
end

function shuffle(tbl)
  size = #tbl
  for i = size, 1, -1 do
    local rand = math.random(size)
    tbl[i], tbl[rand] = tbl[rand], tbl[i]
  end
  return tbl
end

function split(s, sep)
    local fields = {}

    local sep = sep or " "
    local pattern = string.format("([^%s]+)", sep)
    string.gsub(s, pattern, function(c) fields[#fields + 1] = c end)

    return fields
end

function update_data_cat(cat,data)
	local listaStim=split(data,"\r\n")
    
    data_cat[cat]={}
    for i=1,#stimTitle do
    	data_cat[cat][i]={}
    	local tmp=split(listaStim[i],";")
    	for j=1,10 do
        	if tmp[j]~="nil" then data_cat[cat][i][j]=tmp[j] else data_cat[cat][i][j]=nil end
    	end
    end
end

function get_selected_stim(cat)
    for i=1,#stimTitle do
        if currentState[cat][i]<3 then
            return i
        end
    end
    return 0
end

function select_cat(cat)
	selected_cat=cat

	if data_cat[selected_cat]==nil then
        print(catTitle[selected_cat])
        proteo.network.getData(SERVER.."/attenzionevisiva/attenzione_visiva"..tostring(cat-1)..".data","get_av_data")
    else
    	selected_stim=get_selected_stim(selected_cat)
    	showEs(selected_cat,selected_stim)

    	currentForm=2
		proteo.gui.setHidden(attenzione_visiva.forms[1].form,true)
		proteo.gui.setHidden(attenzione_visiva.forms[2].form,false)
    end
end


--==================================================== EXTERNAL
attenzione_visiva.touch_game= function(x,y)

	if currentForm==1 then
		for j=1,8 do
        	if proteo.graphics.isInside(attenzione_visiva.forms[1].ell_cat_attivis[j],x,y) then
				
				data={}
				data["type"]="GAME"
				data["game"]=attenzione_visiva.name
				data["connection_id"]=current_connection_id
				data["role"]=current_role
				data["command"]="select_cat"
				data["cat"]=j
				data["seed"]=4--TODO da generare casualmente
               
                math.randomseed(data["seed"])

				js=json.encode(data)
				current_peer:send(js)
        		select_cat(j)
            	return
        	end
   		end

   	else
		for i=1,16 do   
            if proteo.graphics.isInside(attenzione_visiva.forms[2].img_es[i],x,y)==true then

                local corr=i-1

                --print("save touch clock:"..proteo.system.clock())
                log_data="ES;" --Selezione dello stimolo con il tempo e l'eventuale correttezza
                log_data=log_data..proteo.system.clock()..";"

                is_corr=false
                n_corr=1
                for j=1,#corr_es do
                    if corr==corr_es[j] then
                        is_corr=true
                        n_corr=j
                    end
                end

                if is_corr then --se l'immagine cliccata è corretta
                    --print("save correct: yes")
                    log_data=log_data..i..";YES;"

                    --A] ogni esperimento (ex. 1/3) va ripetuto su "stimNTarget[selected_stim]" immagini
                    --questa informazioni è mantenuta su rip_target

                    if rip_target==stimNTarget[selected_stim] then
                        --C] altrimenti se ho raggiunto il numero totale di immagini corrette presenti
                        -- nell'esperimento valuto l'esperimento eseguito (rip_es++)

                        rip_target=1
                        
                        --print(log_data)
                        rip_es=rip_es+1
                        log(log_data)    
                        
                        currentState[selected_cat][selected_stim]=rip_es
                        if rip_es>=3 then
                            --proteo.gui.setText(n_es,"1")
                            setButtonColor()
                            log("END;YES") --fine dello stimolo (con esito positivo)
                            --send_log()
    						currentForm=1
							proteo.gui.setHidden(attenzione_visiva.forms[1].form,false)
							proteo.gui.setHidden(attenzione_visiva.forms[2].form,true)
                        else
                            --rip_es=rip_es+1
                            proteo.gui.setText(attenzione_visiva.forms[2].n_es,tostring(rip_es))
                            showEs(selected_cat,selected_stim)
                            return
                        end
                    else
                        --B] se il numero di immagini (e quindi di croci blu) è inferiore al target 
                        --procedi aumentando rip_target e mostrando la corretta crocetta blu

                        rip_target=rip_target+1
                        --proteo.gui.setText(n_es,tostring(rip_es))
                        --showEs(st[selected_stim],stimNTarget[selected_stim])
                        proteo.graphics.setHidden(attenzione_visiva.forms[2].blue_cross_es[n_corr],false)
                        corr_es[n_corr]=-1 --elimino la risposta data
                    end
                else
                	--proteo.graphics.changeImage(attenzione_visiva.forms[2].img_es[i],'animali/leone.jpg')
                    --print("save correct: no")
                    log_data=log_data..i..";NO;"
                    if helpA==true then
                        --print("save HelpA remove:"..i)
                        log_data=log_data.."HA;" --Selezione dell'immagine da rimuovere
                        proteo.gui.setHidden(img_es[i],true)
                        helpA=false
                    end
                    --print(log_data)
                    log(log_data)
                end
       
                return
            end --if
        end --for
   	end
end

attenzione_visiva.init_game=function()

	attenzione_visiva.forms[1]=create_main()
	attenzione_visiva.forms[2]=create_es()
	attenzione_visiva.forms[3]=create_wait()

	proteo.gui.setHidden(attenzione_visiva.forms[1].form,true)
	proteo.gui.setHidden(attenzione_visiva.forms[2].form,true)
	proteo.gui.setHidden(attenzione_visiva.forms[3].form,true)
end

attenzione_visiva.start_game=function(peopleid,peer,role,connection_id)

	empty_state()

	current_peopleid=0 --peopleid
	current_peer=peer
	current_role=role
	current_connection_id=connection_id

    --TODO
    --se role=="user" visualizzo "wait"
    --se role=="therapist" faccio il get del test per il peopleid
	proteo.network.proteo_get("/parentme/test/attenzione_visiva/3.0/id/"..current_peopleid,"get_av_tests")

end

attenzione_visiva.exit_game=function()
	proteo.gui.setHidden(attenzione_visiva.forms[1].form,true)
	proteo.gui.setHidden(attenzione_visiva.forms[2].form,true)
	proteo.gui.setHidden(attenzione_visiva.forms[3].form,true)
end

attenzione_visiva.command=function(data)

	print("New command:"..data["command"])
	if data["command"] == "select_cat" then
		j=data["cat"]
        math.randomseed(data["seed"])
        select_cat(j)
        
        proteo.gui.setHidden(attenzione_visiva.forms[3].form,true)
		proteo.gui.setHidden(attenzione_visiva.forms[2].form,false)
    end
    

	
end

return attenzione_visiva
