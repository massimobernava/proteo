
local pose_analysis={}

pose_analysis.name="Pose analysis"
pose_analysis.forms={}

pose_analysis.touch_game= function(x,y)


end

pose_analysis.init_game=function()

	pose_analysis.forms[1]=proteo.gui.newForm("pose_analysis","POSE ANALYSIS",'Helvetica',20,current_theme["logo"],current_theme["medium"],current_theme["medium"],30,proteo.gui.FormType.Fixed+proteo.gui.FormType.HideBar,"","",0,50,MAX_X,MAX_Y-50,"")
  	
  	local title_pose=proteo.gui.newLabel('title_pose','Pose Analysis','Helvetica',30,current_theme["logo"],"clear",proteo.gui.LabelAlignment.Center,MAX_X/2-150,50,300,30)

	local exit=proteo.gui.newButton('exit',"Exit",'Helvetica',15,current_theme["font"],current_theme["light"],1,current_theme["logo"],false,900 ,450,100,25,"exit_game")

  	proteo.gui.addItem(pose_analysis.forms[1],title_pose)

	proteo.gui.addItem(pose_analysis.forms[1],exit)

	proteo.gui.setHidden(pose_analysis.forms[1],true)
end

pose_analysis.start_game=function(peopleid,peer,role,connection_id)
	proteo.gui.setHidden(pose_analysis.forms[1],false)
end

pose_analysis.exit_game=function()
	proteo.gui.setHidden(pose_analysis.forms[1],true)
end

pose_analysis.command=function(data)

end

return pose_analysis
