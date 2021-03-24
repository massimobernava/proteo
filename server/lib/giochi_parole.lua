
local giochi_parole={}

giochi_parole.name="Giochi con le parole"
giochi_parole.forms={}

giochi_parole.touch_game= function(x,y)


end

giochi_parole.init_game=function()

	giochi_parole.forms[1]=proteo.gui.newForm("giochi_parole","POSE ANALYSIS",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")
  	
  	local title_parole=proteo.gui.newLabel('title_parole','Giochi con le parole','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)

	local exit=proteo.gui.newButton('exit',"Exit",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"exit_game")

  	proteo.gui.addItem(giochi_parole.forms[1],title_parole)

	proteo.gui.addItem(giochi_parole.forms[1],exit)

	proteo.gui.setHidden(giochi_parole.forms[1],true)
end

giochi_parole.start_game=function(peopleid,peer,role,connection_id)
	proteo.gui.setHidden(giochi_parole.forms[1],false)
end

giochi_parole.exit_game=function()
	proteo.gui.setHidden(giochi_parole.forms[1],true)
end

giochi_parole.command=function(data)

end

return giochi_parole
