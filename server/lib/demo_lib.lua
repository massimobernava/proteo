local inspect = require("inspect")
local json=require "json"

--DEBUG
require "tfl_utility"

local demo={}

math.randomseed( os.time() )
positions={}
positions[1]={x=650,y=530}
positions[2]={x=750,y=530}
positions[3]={x=850,y=530}
positions[4]={x=950,y=530}
positions[5]={x=650,y=630}
positions[6]={x=750,y=630}
positions[7]={x=850,y=630}
positions[8]={x=950,y=630}

demo.current_page=nil
demo.pages={}

demo.avatar={}
demo.avatar.default_animation={}
demo.avatar.alone_animation={}
demo.avatar.speaking_animation={}
demo.avatar.wow_animation={}

demo.playing_sound=false

demo.audio_out = proteo.audio.openDevice(nil,false,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
proteo.audio.pauseDevice(demo.audio_out ,false)

demo.save_callback=function()

end


demo.save_all=function()

	data={}
	data["method"]="put"
	data["data"]={}
	for i=1,#demo.pages do
		data["data"][i]=demo.pages[i].record
	end
	proteo.network.proteo_post("/deepindigo/data/demo",json.encode(data),demo.save_callback)

end

demo.create_field=function(field_array,form,row)
	if field_array~=nil then
 		if field_array.field_type=="txt" then
 			label = proteo.gui.newLabel("label"..row,field_array.title,"Helvetica",20,"Black","Clear",proteo.gui.LabelAlignment.Left,600,50+row*50,100,30)
 			textfield = proteo.gui.newTextField("textfield"..row,"","Helvetica",25,"#000000","#aaaaaa",750,50+row*50,400,30,"")

 			proteo.gui.addItem(form,label)
 			proteo.gui.addItem(form,textfield)

 			return {type="txt",title=field_array.title,textfield=textfield}
 		end
 	end

 	return nil
end

demo.button_click = function (sender)

	for i=1,10 do
		demo.current_page.record[i]={}
		if demo.current_page.fields[i]~=nil and demo.current_page.fields[i].type=="txt" then
			demo.current_page.record[i].type="txt"
			demo.current_page.record[i].title=demo.current_page.fields[i].title
			demo.current_page.record[i].value=proteo.gui.getText(demo.current_page.fields[i].textfield)
		end
	end
	demo.current_page.function_next()
end

demo.create_info_page=function(option)
	
	--print(inspect(option))

	form=proteo.gui.newForm("form_sequence","sequence form",'Helvetica',20,"black","black",option.color,30,proteo.gui.FormType.HideBar+proteo.gui.FormType.Fixed,"","",MIN_X+20,MIN_Y+20,MAX_X-40,MAX_Y-40,"")
 
 	page={}
    page.type="info"
    page.form=form
    page.record={}
    page.fields={}
    page.function_next=option.function_next

 	page.fields[1]=demo.create_field(option.field1_array,form,1)
 	page.fields[2]=demo.create_field(option.field2_array,form,2)
 	page.fields[3]=demo.create_field(option.field3_array,form,3)
 	page.fields[4]=demo.create_field(option.field4_array,form,4)
 	page.fields[5]=demo.create_field(option.field5_array,form,5)
 	page.fields[6]=demo.create_field(option.field6_array,form,6)
 	page.fields[7]=demo.create_field(option.field7_array,form,7)
 	page.fields[8]=demo.create_field(option.field8_array,form,8)
 	page.fields[9]=demo.create_field(option.field9_array,form,9)
 	page.fields[10]=demo.create_field(option.field10_array,form,10)

 	button = proteo.gui.newButton("button",option.button_txt,"Helvetica",30,"#000000","#aaaaaa",1,"#bbbbbb",true,950,600,150,50,demo.button_click)
 	proteo.gui.addItem(form,button)

 	proteo.gui.setHidden(form,true)

    

	demo.pages[#demo.pages+1]=page

	return page
 end

demo.create_sequence_page=function(option)
	--local form=proteo.gui.newForm()
	form=proteo.gui.newForm("form_sequence","sequence form",'Helvetica',20,"black","black",option.color,30,proteo.gui.FormType.HideBar+proteo.gui.FormType.Fixed,"","",MIN_X+20,MIN_Y+20,MAX_X-40,MAX_Y-40,"")
    main_img=proteo.graphics.newImage("main_img",proteo.system.document()..option.main_image,800,250,375,403)

    options={}
    options[1]={file=option.option1_image,correct=option.option1_correct}
    options[2]={file=option.option2_image,correct=option.option2_correct}
    options[3]={file=option.option3_image,correct=option.option3_correct}
    options[4]={file=option.option4_image,correct=option.option4_correct}
    options[5]={file=option.option5_image,correct=option.option5_correct}
    options[6]={file=option.option6_image,correct=option.option6_correct}
    options[7]={file=option.option7_image,correct=option.option7_correct}
    options[8]={file=option.option8_image,correct=option.option8_correct}

    if option.shuffle then
    	shuffle(options)
    end

    proteo.gui.addItem(form,main_img)
    for i=1,8 do
    	options[i].img=proteo.graphics.newImage("option"..i.."_img",proteo.system.document()..options[i].file,positions[i].x,positions[i].y,73,73)
    	proteo.gui.addItem(form,options[i].img)
    end

    proteo.gui.setHidden(form,true)

    page={}
    page.type="seq"
    page.form=form
    page.options=options
    page.correct=option.function_correct
    page.wrong=option.function_wrong
    page.record={}

	demo.pages[#demo.pages+1]=page

	return page
end

demo.show=function(page)
	if demo.current_page~=nil then
		proteo.gui.setHidden(demo.current_page.form,true)
	end
	proteo.gui.setHidden(page.form,false)
	demo.current_page=page
end

demo.touch=function(x,y)
	--print("Touch x:"..x.." y: "..y)
	if demo.current_page.type=="seq" then
		for i=1,8 do
    		if proteo.graphics.isInside(demo.current_page.options[i].img,x,y) then
    		
    			demo.current_page.record[#page.record+1]={}
    			demo.current_page.record[#page.record].type="selection"
				demo.current_page.record[#page.record].time=os.clock()
    			demo.current_page.record[#page.record].option_id=i
    			demo.current_page.record[#page.record].option_file=demo.current_page.options[i].file

    			if demo.current_page.options[i].correct then
    				demo.current_page.correct()
    				demo.current_page.record[#page.record].correct=true
    			else
    				demo.current_page.wrong()
    				demo.current_page.record[#page.record].correct=false
    			end
    		end
    	end
    end
end

demo.avatar_update = function(dt)

	demo.avatar.current_frame=(demo.avatar.current_frame % #demo.avatar.current_animation)+1
	--print("frame: "..demo.avatar.current_frame.." of: "..#demo.avatar.current_animation)
	proteo.graphics.changeImage(avatar_img,demo.avatar.current_animation[demo.avatar.current_frame])

end

demo.create_avatar = function(default_animation,alone_animation,speaking_animation,wow_animation)

	avatar_img=proteo.graphics.newImage("avatar_img","@avatar",300,523,512,512)

	def_anim=proteo.opencv.videocapture(proteo.system.document()..default_animation)
	alone_anim=proteo.opencv.videocapture(proteo.system.document()..alone_animation)
	speak_anim=proteo.opencv.videocapture(proteo.system.document()..speaking_animation)
	wow_anim=proteo.opencv.videocapture(proteo.system.document()..wow_animation)
	avatar_frame=proteo.opencv.img()

	i=1 
    while proteo.opencv.frame(def_anim,avatar_frame)==1 do
    	demo.avatar.default_animation[i]=avatar_frame
    	avatar_frame=proteo.opencv.img()
    	i=i+1
    end

    i=1 
    while proteo.opencv.frame(alone_anim,avatar_frame)==1 do
    	demo.avatar.alone_animation[i]=avatar_frame
    	avatar_frame=proteo.opencv.img()
    	i=i+1
    end

    i=1 
    while proteo.opencv.frame(speak_anim,avatar_frame)==1 do
    	demo.avatar.speaking_animation[i]=avatar_frame
    	avatar_frame=proteo.opencv.img()
    	i=i+1
    end

    i=1 
    while proteo.opencv.frame(wow_anim,avatar_frame)==1 do
    	demo.avatar.wow_animation[i]=avatar_frame
    	avatar_frame=proteo.opencv.img()
    	i=i+1
    end

	demo.avatar.current_frame=1
	demo.avatar.current_animation=demo.avatar.default_animation

	proteo.graphics.changeImage(avatar_img,demo.avatar.current_animation[demo.avatar.current_frame])

	proteo.graphics.setLayer(avatar_img,TOP)

	avatar_timer=proteo.system.createTimer(50,demo.avatar_update)
    proteo.system.startTimer(avatar_timer)

end

demo.load_sound = function(file)

	wav_audio = proteo.audio.loadWav(proteo.system.document()..file)
 	proteo.audio.convertAudio(wav_audio,48000,proteo.audio.Format.AUDIO_F32SYS,2)
 	--proteo.audio.playAudio(demo.audio_out,wav_audio,100,nil)
 	return wav_audio
end

demo.end_sound = function(index)
	demo.avatar.current_frame=1
 	demo.avatar.current_animation=demo.avatar.default_animation
 	demo.playing_sound=false
end

demo.play_sound = function(sound,animation)

	if demo.playing_sound then 
		return
	end
	demo.playing_sound=true
 	proteo.audio.playAudio(demo.audio_out,sound,100,demo.end_sound )

 	demo.avatar.current_frame=1
 	if animation=="default" then
 		demo.avatar.current_animation=demo.avatar.default_animation
 	elseif animation=="alone" then
 		demo.avatar.current_animation=demo.avatar.alone_animation
 	elseif animation=="speaking" then
 		demo.avatar.current_animation=demo.avatar.speaking_animation
 	elseif animation=="wow" then
 		demo.avatar.current_animation=demo.avatar.wow_animation
 	end

end

demo.pipe = function (json_data)
  data = json.decode(json_data)
  proteo.opencv.frame(demo.cap,demo.frame)
  tmp = proteo.opencv.imencode(demo.frame)
  
  for _, b in ipairs(data['data']) do
    landmarks = get_landmark(b,demo.frame,b['landmarks'])
    --show_pose(landmarks,left_frame)
  end

  	if #landmarks > 250 then
        a={}
        a.x=landmarks[8].x-landmarks[9].x
        a.y=landmarks[8].y-landmarks[9].y
        a.z=landmarks[8].z-landmarks[9].z
        b={}
        b.x=landmarks[250].x-landmarks[9].x
        b.y=landmarks[250].y-landmarks[9].y
        b.z=landmarks[250].z-landmarks[9].z 

        N={}
        N.x = a.y * b.z  - a.z * b.y
        N.y = a.z * b.x - a.x * b.z
        N.z = a.x * b.y - a.y * b.x
    
        if N.x<-300 or N.x>300 then
        	demo.distraction_level=demo.distraction_level+1
        	print("+ ("..N.x..")")
        else
        	demo.distraction_level=0
        	print("0 ("..N.x..")")
        end
    else
    	demo.distraction_level=demo.distraction_level+1
    end

    if demo.distraction_level > 10 then
    	print("ATTENZIONE")
    	demo.function_distracted()
    	demo.distraction_level=-100
    	demo.current_page.record[#demo.current_page.record+1]={}
    	demo.current_page.record[#demo.current_page.record].type="distracted"
    	demo.current_page.record[#demo.current_page.record].time=os.clock()
    end

  --DEBUG
  proteo.opencv.flip(demo.frame,demo.frame,1)
  proteo.graphics.changeImage(debug_img,demo.frame)
  --=====

  data = {}
  data['type']='FRAME'
  data['encoding']='JPEG'
  data['request']='TFLFACE'
  data['frame']=tmp
  data['session']=demo.current_session

  return json.encode(data)
end

demo.face_update = function(dt)

	if demo.zmq_socket ~= nil then
    	buffer = proteo.zmq.recv(demo.zmq_socket,proteo.zmq.flag.ZMQ_DONTWAIT)
    	if buffer ~= nil then
      		proteo.zmq.send (demo.zmq_socket,demo.pipe(buffer),proteo.zmq.flag.ZMQ_DONTWAIT)
    	end
  	end
end

demo.start_callback = function(res,data)

	demo.current_session = data['session']

  	demo.zmq_context = proteo.zmq.ctx_new()
  	demo.zmq_socket = proteo.zmq.socket_new(demo.zmq_context,proteo.zmq.sockType.ZMQ_REQ)
  
  	proteo.zmq.connect(demo.zmq_socket,'tcp://poseidone.irib.cloud:5555')
  
  	demo.frame = proteo.opencv.img()
  	demo.cap = proteo.opencv.videocapture(0)
  	proteo.opencv.setFrameSize(demo.cap,640,480)
  	proteo.opencv.setBufferSize(demo.cap,3)

  	proteo.opencv.frame(demo.cap,demo.frame)
  	proteo.opencv.flip(demo.frame,demo.frame,1)
  
  	--DEBUG
	debug_img=proteo.graphics.newImage("debug_img","@webcam",300,200,160,120)
  	proteo.graphics.setLayer(debug_img,TOP)
	proteo.graphics.changeImage(debug_img,demo.frame)
	--=====

	demo.distraction_level=0

  	tmp = proteo.opencv.imencode(demo.frame)

  	data = {}
  	data['type']='FRAME'
  	data['frame']=tmp
  	data['session']=demo.current_session
  	data['encoding']='JPEG'
  	data['request']='TFLFACE'
  	proteo.zmq.send (demo.zmq_socket,json.encode(data),proteo.zmq.flag.ZMQ_DONTWAIT)

	face_timer = proteo.system.createTimer(50,demo.face_update)
	proteo.system.startTimer(face_timer)

 end

demo.face_detect = function(option)
	proteo.network.proteo_post("/deepcrimson/start",'{}',demo.start_callback)
	demo.function_attentive=option.function_attentive
	demo.function_distracted=option.function_distracted
end


function shuffle(tbl)
  size = #tbl
  for i = size, 1, -1 do
    local rand = math.random(size)
    tbl[i], tbl[rand] = tbl[rand], tbl[i]
  end
  return tbl
end

return demo
