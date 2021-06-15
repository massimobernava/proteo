
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
    print("Selected video: "..selected_video)
    --video_cap=proteo.opencv.videocapture("http://localhost:8888/sample_video.avi")

end

function frame_update(dt)

  --analyzeframe(current_frame)
  --frame_wl=proteo.opencv.img()
  --proteo.opencv.copy(current_video[current_frame].frame,frame_wl)

  --proteo.graphics.changeImage(video_img,frame_wl)

  --current_frame=current_frame+1

  analyzeframe(current_frame)

  current_frame=current_frame+1

  if current_frame>#current_video then
    proteo.system.stopTimer(frame_timer)
  end  
end

frame_timer=proteo.system.createTimer(50,frame_update)

function landmarks_callback(res,data)
	frame_wl=proteo.opencv.img()
	--proteo.opencv.setSize(current_video[current_frame].frame_wl,video_size.x,video_size.y)
  	proteo.opencv.copy(current_video[current_frame].frame,frame_wl)

    current_video[current_frame].request=data['request']
  	current_video[current_frame].data=data['data']
    current_video[current_frame].landmarks={}

  	for i, b in ipairs(data['data']) do
      --print("Center X:"..landmarks[1].x.." Y:"..landmarks[1].y)
      landmarks = get_landmark(b,frame_wl,b['landmarks'])
      current_video[current_frame].landmarks[i]=landmarks

      if data['request']=="TFLPOSE" or data['request']=="TFLHOLI" then
        show_pose(landmarks,frame_wl)
      elseif data['request'] == "TFLFACE" then
        show_facemesh(landmarks,frame_wl)
      else
        show_hand(landmarks,frame_wl)
      end
  	end

  if current_video[current_frame].request=="TFLHOLI" then
    for _, b in ipairs(data['face']) do
      landmarks = show_landmark(b,frame_wl,b['landmarks'])
    end
  end
    

  	proteo.graphics.changeImage(video_img,frame_wl)

    
end

function analyzeframe(nframe)
  local tmp=proteo.opencv.imencode(current_video[nframe].frame)
            
    local data={}
    data['type']='FRAME'
    data["encoding"]="JPEG"
    data['session']=current_session
    if proteo.gui.getId(sel)=="drop_gui_face" then
        data["request"]="TFLFACE"
    elseif proteo.gui.getId(sel)=="drop_gui_pose" then
        data["request"]="TFLPOSE"
    elseif proteo.gui.getId(sel)=="drop_gui_holi" then
        data["request"]="TFLHOLI"
    elseif proteo.gui.getId(sel)=="drop_gui_hand" then
        data["request"]="TFLHAND"
    end

    data["frame"]=tmp

    local js=json.encode(data)

    proteo.network.proteo_post("/deepcrimson/landmarks",js,landmarks_callback)
end

function analyzecurrentframe(sender)

    sel=proteo.gui.getValue(drop_video)
    if sel==nil then 
        return
    end

  analyzeframe(current_frame)
    
end

function analyzevideo(sender)

    sel=proteo.gui.getValue(drop_video)
    if sel==nil then 
        return
    end

    current_frame=1
    proteo.system.startTimer(frame_timer)
    
end

function savedata(sender)

  for i=1,#current_video do
    if current_video[i].landmarks~=nil then
      for j=1,#current_video[i].landmarks do
        row="Frame"..i..",Box"..j
        for k=1,#current_video[i].landmarks[j] do
          row=row..","..current_video[i].landmarks[j][k].x
          row=row..","..current_video[i].landmarks[j][k].y
          row=row..","..current_video[i].landmarks[j][k].z
        end
        print(row)
      end
    end
  end
end

function showvideo(sender)
    if selected_video~=nil then
        current_video={}
        --data=proteo.system.readFile(selected_file,readfile)
        --selected_video="http://localhost:8888/sample_video.avi"

        --In modalità web la fase di videocapture richiede un'attesa prima di richiedere il frame, per adesso ho spostato in selectvideo
        video_cap=proteo.opencv.videocapture(selected_video)
        local i=1
        video_frame=proteo.opencv.img()

        while proteo.opencv.frame(video_cap,video_frame)==1 do
        --if proteo.opencv.frame(video_cap,video_frame)==1 then
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

      for i=1,#current_video[current_frame].landmarks do
        if current_video[current_frame].request == "TFLPOSE" or current_video[current_frame].requestv=="TFLHOLI" then
          show_pose(current_video[current_frame].landmarks[i],frame_wl)
        elseif current_video[current_frame].request == "TFLFACE" then
          show_facemesh(current_video[current_frame].landmarks[i],frame_wl)
        else
          show_hand(current_video[current_frame].landmarks[i],frame_wl)
        end
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

      for i=1,#current_video[current_frame].landmarks do
        if current_video[current_frame].request == "TFLPOSE" or current_video[current_frame].requestv=="TFLHOLI" then
          show_pose(current_video[current_frame].landmarks[i],frame_wl)
        elseif current_video[current_frame].request == "TFLFACE" then
          show_facemesh(current_video[current_frame].landmarks[i],frame_wl)
        else
          show_hand(current_video[current_frame].landmarks[i],frame_wl)
        end
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
	  next_video=proteo.gui.newButton('next_video',">>",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 500 ,MAX_Y - 120,100,50,nextvideo)
    prev_video=proteo.gui.newButton('prev_video',"<<",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 350 ,MAX_Y - 120,100,50,prevvideo)
    
    analyze_frame=proteo.gui.newButton('analyse_frame',"Analyze Frame",'Helvetica',20,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 650 ,MAX_Y - 120,150,50,analyzecurrentframe)
    analyze_video=proteo.gui.newButton('analyse_video',"Analyze All",'Helvetica',20,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 820 ,MAX_Y - 120,150,50,analyzevideo)
    save_data=proteo.gui.newButton('analyse_video',"Save",'Helvetica',25,colors.button_font,colors.button_back,1,colors.button_border,true,MIN_X + 990 ,MAX_Y - 120,150,50,savedata)
    
    drop_video=proteo.gui.newDropDown('drop_video',"Select Mode",'Helvetica',20,colors.button_font,colors.button_back,"",proteo.gui.DropDownType.Normal,colors.button_back,MIN_X+50 ,MAX_Y - 150,200,30,"")
    drop_gui_pose=proteo.gui.newDropDownItem('drop_gui_pose',"BlazePose",'Helvetica',20,colors.button_font,colors.button_back,"","",200,30)
    drop_gui_face=proteo.gui.newDropDownItem('drop_gui_face',"BlazeFace",'Helvetica',20,colors.button_font,colors.button_back,"","",200,30)
    drop_gui_hand=proteo.gui.newDropDownItem('drop_gui_hand',"Hand",'Helvetica',20,colors.button_font,colors.button_back,"","",200,30)
    drop_gui_holi=proteo.gui.newDropDownItem('drop_gui_holi',"Holistic",'Helvetica',20,colors.button_font,colors.button_back,"","",200,30)
    proteo.gui.addItem(drop_video,drop_gui_pose)
    proteo.gui.addItem(drop_video,drop_gui_face)
    proteo.gui.addItem(drop_video,drop_gui_hand)
    proteo.gui.addItem(drop_video,drop_gui_holi)

    proteo.gui.addItem(form,video_load)
    proteo.gui.addItem(form,video_img)
    proteo.gui.addItem(form,show_video)
    proteo.gui.addItem(form,next_video)
    proteo.gui.addItem(form,prev_video)
    proteo.gui.addItem(form,drop_video)
    proteo.gui.addItem(form,analyze_video)
    proteo.gui.addItem(form,analyze_frame)
    proteo.gui.addItem(form,save_data)

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
          print("Found:"..filename)

          --if EMSCRIPTEN==0 then
            videos[md5.sumhexa(filename)]=filename
            list_item_file=proteo.gui.newListItem(md5.sumhexa(filename),proteo.system.basename(filename),'Helvetica',15,colors.list_font,colors.list_back,"","",500,50)
          --else
          --  videos[filename]=filename
          --  list_item_file=proteo.gui.newListItem(filename,proteo.system.basename(filename),'Helvetica',15,colors.list_font,colors.list_back,"","",500,50)
          --end

          proteo.gui.addItem(video_list,list_item_file)
        end
        )
end

function start_callback(res,data)

	current_session = data['session']
end

function download_callback()

end

function init()

	home=create_page()
	load_list(home.video_load)
	
  --if EMSCRIPTEN==1 then
  --  proteo.network.download("sample_video.avi","/proteo/sample_video.avi","download_callback")
  --end

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