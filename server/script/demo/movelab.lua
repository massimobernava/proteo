
local json=require "json"
local md5 = require 'md5'
local inspect = require("inspect")

require "tfl_utility"

selected_video=nil
current_video={}
current_frame=1
current_session=nil

video_img=nil
drop_video=nil
video_size={}
video_size.x=480
video_size.y=640

videos={}
colors={}

colors.form_back="lightblue"
colors.list_back="aliceblue"
colors.list_font="black"

colors.button_font="black"
colors.button_back="aliceblue"
colors.button_border="black"

function selectvideo(sel)

    selected_video=videos[proteo.system.getName(sel)]

end

function landmarks_callback(res,data)
	frame_wl=proteo.opencv.img()
	--proteo.opencv.setSize(current_video[current_frame].frame_wl,video_size.x,video_size.y)
  	proteo.opencv.copy(current_video[current_frame].frame,frame_wl)

  	current_video[current_frame].data=data['data']

  	for _, b in ipairs(data['data']) do
    	landmarks = show_landmark(b,frame_wl,b['landmarks'])
  	end


  	proteo.graphics.changeImage(video_img,frame_wl)

end

function analysevideo(sender)

    sel=proteo.gui.getValue(drop_video)
    if sel==nil then 
        return
    end
    --proteo.opencv.resize(video_frame,video_resized)
    --proteo.opencv.cropImg(video_frame,video_resized,(videosize[2]-456)/2,(videosize[1]-256)/2,456,256)   

    --proteo.opencv.convert(current_video[videoframe].frame,current_video[videoframe].frame,proteo.opencv.matType.CV_32F)
    local tmp=proteo.opencv.imencode(current_video[current_frame].frame)
            
    local data={}
    data['type']='FRAME'
    data["encoding"]="JPEG"
    data['session']=current_session
    if proteo.gui.getId(sel)=="drop_gui_face" then
        data["request"]="TFLFACE"
    elseif proteo.gui.getId(sel)=="drop_gui_pose" then
        data["request"]="TFLPOSE"
    end

    data["frame"]=tmp

    local js=json.encode(data)

    proteo.network.proteo_post("/deepcrimson/landmarks",js,landmarks_callback)

end

function showvideo(sender)
    if selected_video~=nil then
        current_video={}
        --data=proteo.system.readFile(selected_file,readfile)
        video_cap=proteo.opencv.videocapture(selected_video)
        local i=1
        video_frame=proteo.opencv.img()

        while proteo.opencv.frame(video_cap,video_frame)==1 do
            current_video[i]={}

            current_video[i].frame=proteo.opencv.img()

            proteo.opencv.setImg(current_video[i].frame,video_size.x,video_size.y,"#000000") 
            proteo.opencv.resize(video_frame,current_video[i].frame)
            --proteo.opencv.infoImg(current_video[i].frame)
            i=i+1
        end
        
        current_frame=1
        
        proteo.graphics.changeImage(video_img,current_video[current_frame].frame)
        
    end
end

function nextvideo(sender)
    current_frame=current_frame+1
    --proteo.opencv.addWeighted(current_video[videoframe].frame,0.5,current_video[videoframe].pose,0.5,current_video[videoframe].result,0)
    if current_video[current_frame].data~=nil then
    	--proteo.graphics.changeImage(video_img,current_video[current_frame].frame_wl)

    	frame_wl=proteo.opencv.img()
		proteo.opencv.copy(current_video[current_frame].frame,frame_wl)

  		for _, b in ipairs(current_video[current_frame].data) do
    		landmarks = show_landmark(b,frame_wl,b['landmarks'])
  		end

  		proteo.graphics.changeImage(video_img,frame_wl)

    else
    	proteo.graphics.changeImage(video_img,current_video[current_frame].frame)
    end
    --proteo.gui.setText(frame_video,videoframe)
end

function prevvideo(sender)
    current_frame=current_frame-1

    --proteo.opencv.addWeighted(current_video[videoframe].frame,0.5,current_video[videoframe].pose,0.5,current_video[videoframe].result,0)
    if current_video[current_frame].data~=nil then
    	--proteo.graphics.changeImage(video_img,current_video[current_frame].frame_wl)

    	frame_wl=proteo.opencv.img()
		proteo.opencv.copy(current_video[current_frame].frame,frame_wl)

  		for _, b in ipairs(current_video[current_frame].data) do
    		landmarks = show_landmark(b,frame_wl,b['landmarks'])
  		end

  		proteo.graphics.changeImage(video_img,frame_wl)

    else
    	proteo.graphics.changeImage(video_img,current_video[current_frame].frame)
    end
    --proteo.gui.setText(frame_video,videoframe)
end

function create_page()

	form=proteo.gui.newForm("form_movelab","Movelab",'Helvetica',20,"black","black",colors.form_back,30,proteo.gui.FormType.HideBar+proteo.gui.FormType.Fixed,"","",MIN_X+20,MIN_Y+20,MAX_X-40,MAX_Y-40,"")
  	
  	video_load=proteo.gui.newList('video_load','','Helvetica',30,colors.list_font,colors.list_back,MIN_X+20,150,500,400,selectvideo)
    show_video=proteo.gui.newButton('show_video',"Show>>",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 420 ,MAX_Y - 220,100,50,showvideo)
    video_img=proteo.graphics.newImage("video","@video",800,350,video_size.x,video_size.y)
	next_video=proteo.gui.newButton('next_video',">>",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 650 ,MAX_Y - 120,100,50,nextvideo)
    prev_video=proteo.gui.newButton('prev_video',"<<",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 500 ,MAX_Y - 120,100,50,prevvideo)
    
    analyse_video=proteo.gui.newButton('analyse_video',"Analyse",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 800 ,MAX_Y - 120,300,50,analysevideo)
    
    drop_video=proteo.gui.newDropDown('drop_video',"Select Mode",'Helvetica',20,colors.button_font,colors.button_back,"",proteo.gui.DropDownType.Normal,colors.button_back,MIN_X+200 ,MAX_Y - 110,200,30,"")
    drop_gui_pose=proteo.gui.newDropDownItem('drop_gui_pose',"BlazePose",'Helvetica',20,colors.button_font,colors.button_back,"","",200,30)
    drop_gui_face=proteo.gui.newDropDownItem('drop_gui_face',"BlazeFace",'Helvetica',20,colors.button_font,colors.button_back,"","",200,30)
    proteo.gui.addItem(drop_video,drop_gui_pose)
    proteo.gui.addItem(drop_video,drop_gui_face)

    proteo.gui.addItem(form,video_load)
    proteo.gui.addItem(form,video_img)
    proteo.gui.addItem(form,show_video)
    proteo.gui.addItem(form,next_video)
    proteo.gui.addItem(form,prev_video)
    proteo.gui.addItem(form,drop_video)
    proteo.gui.addItem(form,analyse_video)

    page={}
    page.form=form
    page.video_load=video_load
    page.video_img=video_img

  return page
end

function load_list(video_list)

	proteo.gui.emptyList(video_list)
	fileapply(
        proteo.system.document(),
        "%a*avi$",
        function(filename)

            videos[md5.sumhexa(filename)]=filename

            list_item_file=proteo.gui.newListItem(md5.sumhexa(filename),proteo.system.basename(filename),'Helvetica',15,colors.list_font,colors.list_back,"","",500,50)
            proteo.gui.addItem(video_list,list_item_file)
        end
        )
end

function start_callback(res,data)

	current_session = data['session']
end


function init()

	home=create_page()
	load_list(home.video_load)
	
	proteo.network.proteo_post("/deepcrimson/start",'{}',start_callback)

	proteo.gui.setHidden(home.form,false)
end


function update(dt)


end

function fileapply(path, regex, closure)
   for i,filename in ipairs(proteo.system.dir(path)) do

      if filename ~= '.' and filename ~= '..' then
         filename = path ..  filename
         if proteo.system.isdir(filename) then
            fileapply(filename.."/", regex, closure)
         else
            if filename:match(regex) then
               closure(filename)
            end
         end
      end
   end
end