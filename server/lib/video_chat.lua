
local video_chat={}

video_chat.name="Video Chat"
video_chat.forms={}

video_chat.touch_game= function(x,y)


end

video_chat.init_game=function()

	video_chat.forms[1]=proteo.gui.newForm("video_chat","VIDEOCHAT",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")
  	
  	local title_videochat=proteo.gui.newLabel('title_videochat','Video Chat','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)

	local exit=proteo.gui.newButton('exit',"Exit",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"exit_game")

  	proteo.gui.addItem(video_chat.forms[1],title_videochat)

	proteo.gui.addItem(video_chat.forms[1],exit)

	proteo.gui.setHidden(video_chat.forms[1],true)
end

video_chat.start_game=function(peopleid,peer,role,connection_id)
	proteo.gui.setHidden(video_chat.forms[1],false)
end

video_chat.exit_game=function()
	proteo.gui.setHidden(video_chat.forms[1],true)
end

video_chat.command=function(data)

end

return video_chat
