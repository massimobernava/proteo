
local attenzione_uditiva={}

attenzione_uditiva.name="Attenzione uditiva"
attenzione_uditiva.forms={}

attenzione_uditiva.touch_game= function(x,y)


end

attenzione_uditiva.init_game=function()

	attenzione_uditiva.forms[1]=proteo.gui.newForm("attenzione_uditiva","ATTENZIONE UDITIVA",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")
  	
  	local title_attenzione_uditiva=proteo.gui.newLabel('title_attenzione_uditiva','Attenzione uditiva','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)

	local exit=proteo.gui.newButton('exit',"Exit",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"exit_game")

  	proteo.gui.addItem(attenzione_uditiva.forms[1],title_attenzione_uditiva)

	proteo.gui.addItem(attenzione_uditiva.forms[1],exit)

	proteo.gui.setHidden(attenzione_uditiva.forms[1],true)
end

attenzione_uditiva.start_game=function(peopleid,peer,role,connection_id)
	proteo.gui.setHidden(attenzione_uditiva.forms[1],false)
end

attenzione_uditiva.exit_game=function()
	proteo.gui.setHidden(attenzione_uditiva.forms[1],true)
end

attenzione_uditiva.command=function(data)

end

return attenzione_uditiva
