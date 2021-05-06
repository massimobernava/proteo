require 'Form'
local json=require "json"

forms={}

theme={}
theme["background"]="#002F5F"
theme["title_back"]="Clear"
theme["title_font"]="#B8DBD9"
theme["button_font"]="#FBFBFF"
theme["button_back"]="#20BF55"
theme["button_border"]="#586e75"
theme["alternate_row_back"]="#586F7C"
theme["font_color"]="#FBFBFF"
theme["check1_color"]="#208abf"
theme["check2_color"]="#55bf20"

update_function=nil

enet_url="192.168.0.116:9999"
------------------
--+++++ENET+++++--
------------------

local enet_host=nil
local enet_server=nil
enet_frame=nil
enet_resized=nil
enet_cap=nil
enet_img=nil
enet_audio_out=nil
enet_audio_in=nil
enet_rec_stream=nil
enet_play_stream=nil

function enet_init()
	enet_host = proteo.enet.host_create()
	enet_server = enet_host:connect(enet_url)

    enet_frame=proteo.opencv.img()
    enet_resized=proteo.opencv.img()
    proteo.opencv.setImg(enet_resized,100,100,"#000000")
    
    enet_cap=proteo.opencv.videocapture(0)
    proteo.opencv.setFrameSize(enet_cap,640,480)
    proteo.opencv.setBufferSize(enet_cap,3)
    proteo.opencv.frame(enet_cap,enet_frame)

    t2=proteo.system.createTimer(10,enet_event)
    proteo.system.startTimer(t2)
    
    proteo.audio.infoDrivers()
    print("Platform: "..proteo.system.getPlatform())
    if proteo.audio.init() then
        proteo.audio.infoDevices()
        --proteo.audio.quit()
    end

    enet_audio_out=proteo.audio.openDevice(nil,false,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
    enet_audio_in=proteo.audio.openDevice(nil,true,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
    
    proteo.audio.pauseDevice(enet_audio_out,false)
    proteo.audio.pauseDevice(enet_audio_in,false)

    enet_rec_stream=proteo.audio.newStream(48000,proteo.audio.Format.AUDIO_F32SYS,2,22050,proteo.audio.Format.AUDIO_S16SYS,1)
    enet_play_stream=proteo.audio.newStream(22050,proteo.audio.Format.AUDIO_S16SYS,1,48000,proteo.audio.Format.AUDIO_F32SYS,2)

    t_enet=proteo.gui.newLabel('t_enet','Enet Test','Helvetica',50,theme["title_font"],theme["title_back"],proteo.gui.LabelAlignment.Center,MID_X-150,MIN_Y+30,300,40)
    back_enet=proteo.gui.newButton('back_enet',"Back",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MAX_Y - 100,100,50,"back")

    enet_img=proteo.graphics.newImage("chat_webcam","",300,400,400,400)
    proteo.graphics.changeImage(enet_img,enet_frame)

    enetForm=Form('enetForm')
    enetForm.backgroundColor="#000000"
    enetForm:addControl(t_enet)
    enetForm:addControl(back_enet)
    enetForm:addControl(enet_img)

    forms["enetForm"]=enetForm
    forms["enetForm"]:show()

    update_function=enet_update
end

function enet_update(dt)
	
end

function tm()
    local event = enet_host:service(0)
    if event then
        if event.type == "connect" then
            print("Connected to", event.peer)
            event.peer:send("hello world")
        elseif event.type == "receive" then
            print("Got message: ", event.data, event.peer)
            --done = true
        end
    end
end


function enet_event(dt)
    local event = enet_host:service(0)
    if event then
        if event.type == "connect" then
            print("Connected to", event.peer)
            proteo.opencv.frame(enet_cap,enet_frame)
            proteo.opencv.resize(enet_frame,enet_resized)
            tmp=proteo.opencv.imencode(enet_resized)
            
            data={}
            data["type"]="VIDEO"
			data["data"]=tmp
			js=json.encode(data)
			
            event.peer:send(js)
            
            proteo.audio.recordStream(enet_audio_in,enet_rec_stream)
			proteo.audio.playStream(enet_audio_out,enet_play_stream)
			tmp=proteo.audio.getStream(enet_rec_stream,100)
			
			data["type"]="AUDIO"
			data["data"]=tmp

			js=json.encode(data)
			
			event.peer:send(js)

        elseif event.type == "receive" then
            --print("Got message: ", event.data, event.peer)
            --done = true
            data=json.decode(event.data)
            if data["type"]=="VIDEO" then
				proteo.opencv.imdecode(data["data"],enet_resized)
				proteo.graphics.changeImage(enet_img,enet_resized)
				
				proteo.opencv.frame(enet_cap,enet_frame)
				proteo.opencv.resize(enet_frame,enet_resized)
				tmp=proteo.opencv.imencode(enet_resized)
				data["data"]=tmp
				js=json.encode(data)
			
				event.peer:send(js)
			elseif data["type"]=="AUDIO" then
				if data["data"]~=nil then
					proteo.audio.putStream(enet_play_stream,data["data"])
				end
				
				tmp=proteo.audio.getStream(enet_rec_stream,100)
				data["data"]=tmp
				js=json.encode(data)
			
				event.peer:send(js)
			end
			      
        end
    end
end
------------------
--++++OPENCV++++--
------------------

opencv_img=nil  --Image Control
opencv_frame=nil --Frame
--opencv_resized=nil --Frame
opencv_cap=nil --VideoCapture
--opencv_host=nil
--opencv_server=nil

function opencv_init()

    --opencv_host = proteo.enet.host_create()
    --opencv_server = opencv_host:connect(enet_url)

    opencv_frame=proteo.opencv.img()
	opencv_resized=proteo.opencv.img()
	--proteo.opencv.setImg(opencv_resized,100,100,"#000000")
	
    opencv_cap=proteo.opencv.videocapture(0)
    proteo.opencv.setFrameSize(opencv_cap,640,480)
    proteo.opencv.setBufferSize(opencv_cap,3)
    proteo.opencv.frame(opencv_cap,opencv_frame)
    
    t3=proteo.system.createTimer(10,opencv_event)
    proteo.system.startTimer(t3)

    t_opencv=proteo.gui.newLabel('t_opencv','OpenCV Test','Helvetica',50,theme["title_font"],theme["title_back"],proteo.gui.LabelAlignment.Center,MID_X-150,MIN_Y+30,300,40)
    back_opencv=proteo.gui.newButton('back_opencv',"Back",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MAX_Y - 100,100,50,"back")
    
    opencv_img=proteo.graphics.newImage("webcam","",300,400,400,400)
    proteo.graphics.changeImage(opencv_img,opencv_frame)

    opencvForm=Form('opencvForm')
    opencvForm.backgroundColor="#000000"
    opencvForm:addControl(opencv_img)
    opencvForm:addControl(t_opencv)
    opencvForm:addControl(back_opencv)

    forms["opencvForm"]=opencvForm
    forms["opencvForm"]:show()

    update_function=opencv_update
end

function opencv_update(dt)
    --print("FPS: "..1000/dt)

end

function opencv_event(dt)
    proteo.opencv.frame(opencv_cap,opencv_frame)
    proteo.graphics.changeImage(opencv_img,opencv_frame)

end

------------------
--++++AUDIO+++++--
------------------

dev_audio_out=nil
wav_audio=nil
play_audio=nil
record_audio=nil
rec_audio=nil

function audio_init()

    t_audio=proteo.gui.newLabel('t_audio','Audio Test','Helvetica',50,theme["title_font"],theme["title_back"],proteo.gui.LabelAlignment.Center,MID_X-150,MIN_Y+30,300,40)
    back_audio=proteo.gui.newButton('back_audio',"Back",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MAX_Y - 100,100,50,"back")
    
    btn1_audio=proteo.gui.newButton('btn1_audio',"Play",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 200,100,50,"play")
    btn2_audio=proteo.gui.newButton('btn2_audio',"Record",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 300,100,50,"record")
    btn3_audio=proteo.gui.newButton('btn3_audio',"Stream",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 400,100,50,"stream")
   
    btn12_audio=proteo.gui.newButton('btn12_audio',"Stop",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 220 ,MIN_Y + 200,100,50,"stopplay")
    btn22_audio=proteo.gui.newButton('btn22_audio',"Play",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 220 ,MIN_Y + 300,100,50,"stoprecord")
    
    proteo.audio.infoDrivers()
    print("Platform: "..proteo.system.getPlatform())
    if proteo.audio.init() then
        proteo.audio.infoDevices()
        --proteo.audio.quit()
    end

    dev_audio_out=proteo.audio.openDevice(nil,false,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
    dev_audio_in=proteo.audio.openDevice(nil,true,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
    
    proteo.audio.pauseDevice(dev_audio_out,false)
    proteo.audio.pauseDevice(dev_audio_in,false)

    wav_audio=proteo.audio.loadWav(proteo.system.document().."wav_test.wav")
    proteo.audio.convertAudio(wav_audio,48000,proteo.audio.Format.AUDIO_F32SYS,2)
    rec_audio=proteo.audio.createBuffer("rec_audio",48000,proteo.audio.Format.AUDIO_F32SYS,2,5) --5 secondi di buffer
    
    rec_stream=proteo.audio.newStream(48000,proteo.audio.Format.AUDIO_F32SYS,2,22050,proteo.audio.Format.AUDIO_S16SYS,1)--48000,proteo.audio.Format.AUDIO_F32SYS,2)
    play_stream=proteo.audio.newStream(22050,proteo.audio.Format.AUDIO_S16SYS,1,48000,proteo.audio.Format.AUDIO_F32SYS,2)--(48000,proteo.audio.Format.AUDIO_F32SYS,2,48000,proteo.audio.Format.AUDIO_F32SYS,2)

    proteo.audio.infoAudio(wav_audio)

    audioForm=Form('audioForm')
    audioForm.backgroundColor="#000000"
    audioForm:addControl(t_audio)
    audioForm:addControl(back_audio)
    audioForm:addControl(btn1_audio)
    audioForm:addControl(btn2_audio)
    audioForm:addControl(btn3_audio)
    audioForm:addControl(btn12_audio)
    audioForm:addControl(btn22_audio)

    forms["audioForm"]=audioForm
    forms["audioForm"]:show()

    update_function=audio_update

end

function audio_update(dt)
    data=proteo.audio.getStream(rec_stream,100)
    if data~=nil then proteo.audio.putStream(play_stream,data) end
end

function play(sender)
    play_audio=proteo.audio.playAudio(dev_audio_out,wav_audio,100) --CALLBACK QUANDO E' FINITA?
end

function stopplay(sender)
    proteo.audio.stopAudio(play_audio)
end

function record(sender)

    record_audio=proteo.audio.recordAudio(dev_audio_in,rec_audio) --CALLBACK QUANDO E' FULL?
end

function stream(sender)
    proteo.audio.recordStream(dev_audio_in,rec_stream)
    proteo.audio.playStream(dev_audio_out,play_stream)
end

function stoprecord(sender)
    play_audio=proteo.audio.playAudio(dev_audio_out,rec_audio,100)
    --proteo.audio.stopAudio(record_audio)
    --proteo.audio.stopStream(rec_stream)
    --proteo.audio.stopStream(play_stream)
end

------------------
--++++++GUI+++++--
------------------
t_gui=nil
back_gui=nil
label1_gui=nil
label2_gui=nil
label3_gui=nil

function change_color_gui(sender)

end


function gui_init()
    t_gui=proteo.gui.newLabel('t_gui','Gui Test','Helvetica',50,theme["title_font"],theme["title_back"],proteo.gui.LabelAlignment.Center,MID_X-150,MIN_Y+30,300,40)
    back_gui=proteo.gui.newButton('back_gui',"Back",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MAX_Y - 100,100,50,"back")
    
    label1_gui=proteo.gui.newLabel('label1_gui','label left','Helvetica',20,"red","white",proteo.gui.LabelAlignment.Left,MIN_X+25,MIN_Y+150,200,30)
    label2_gui=proteo.gui.newLabel('label2_gui','label center','Helvetica',20,"black","white",proteo.gui.LabelAlignment.Center,MIN_X+25,MIN_Y+190,200,30)
    label3_gui=proteo.gui.newLabel('label3_gui','label right','Helvetica',20,"green","white",proteo.gui.LabelAlignment.Right,MIN_X+25,MIN_Y+230,200,30)

    drop_gui=proteo.gui.newDropDown('drop_gui',"DropDown",'Helvetica',20,"burlywood","","",proteo.gui.DropDownType.Normal,"cadetblue",MIN_X+325 ,MIN_Y+150,200,30,"")
    drop_gui_item1=proteo.gui.newDropDownItem('drop_gui_item1',"Prova1",'Helvetica',20,"burlywood","cadetblue","","",200,30)
    drop_gui_item2=proteo.gui.newDropDownItem('drop_gui_item2',"Prova2",'Helvetica',20,"burlywood","cadetblue","","",200,30)
    proteo.gui.addItem(drop_gui,drop_gui_item1)
    proteo.gui.addItem(drop_gui,drop_gui_item2)

    button1_gui=proteo.gui.newButton('button1_gui',"round button",'Helvetica',30,"black","aqua",5,"coral",true,MIN_X + 25 ,MIN_Y+300,200,50,"")
    button2_gui=proteo.gui.newButton('button2_gui',"square button",'Helvetica',30,"black","coral",3,"aqua",false,MIN_X + 325 ,MIN_Y+300,200,50,"")
    button3_gui=proteo.gui.newButton('button3_gui',"active button",'Helvetica',30,"black","white",1,"white",true,MIN_X + 625 ,MIN_Y+300,200,50,change_color_gui)
    
    textfield_gui=proteo.gui.newTextField('textfield_gui',"",'Helvetica',25,"black","white",MIN_X + 25 ,MIN_Y+400,500,30,"")

    check1_gui=proteo.gui.newCheckbox('check1_gui',"darkorange","blueviolet",proteo.gui.CheckboxType.SimpleSquare,MIN_X + 25,MIN_Y + 450,30,30,'')
    check2_gui=proteo.gui.newCheckbox('check2_gui',"darkorange","blueviolet",proteo.gui.CheckboxType.CrossSquare,MIN_X + 75,MIN_Y + 450,30,30,'')
    check3_gui=proteo.gui.newCheckbox('check3_gui',"darkorange","blueviolet",proteo.gui.CheckboxType.CheckSquare,MIN_X + 125,MIN_Y + 450,30,30,'')
    check4_gui=proteo.gui.newCheckbox('check4_gui',"darkorange","blueviolet",proteo.gui.CheckboxType.SimpleRound,MIN_X + 175,MIN_Y + 450,30,30,'')
    check5_gui=proteo.gui.newCheckbox('check5_gui',"darkorange","blueviolet",proteo.gui.CheckboxType.CrossRound,MIN_X + 225,MIN_Y + 450,30,30,'')
    check6_gui=proteo.gui.newCheckbox('check6_gui',"darkorange","blueviolet",proteo.gui.CheckboxType.CheckRound,MIN_X + 275,MIN_Y + 450,30,30,'')

    radio1_gui=proteo.gui.newCheckbox('radio1_gui',"darkmagenta","cornflowerblue",proteo.gui.CheckboxType.SimpleRound,MIN_X + 25,MIN_Y + 500,30,30,'')
    radio2_gui=proteo.gui.newCheckbox('radio2_gui',"darkmagenta","cornflowerblue",proteo.gui.CheckboxType.SimpleRound,MIN_X + 75,MIN_Y + 500,30,30,'')
    radio3_gui=proteo.gui.newCheckbox('radio3_gui',"darkmagenta","cornflowerblue",proteo.gui.CheckboxType.SimpleRound,MIN_X + 125,MIN_Y + 500,30,30,'')
    
    proteo.gui.linkCheckbox(radio1_gui,radio2_gui)
    proteo.gui.linkCheckbox(radio2_gui,radio3_gui)
    proteo.gui.linkCheckbox(radio3_gui,radio1_gui)

    form1_gui=proteo.gui.newForm("form1_gui","SimpleForm",'Helvetica',20,"black","crimson","blanchedalmond",30,proteo.gui.FormType.Normal,"","",MID_X,MID_Y,250,250,"")
    buttonform_gui=proteo.gui.newButton('buttonform_gui',"form button",'Helvetica',15,"black","cornsilk",1,"crimson",false,20 ,50,100,25,"")
    checkform_gui=proteo.gui.newCheckbox('checkform_gui',"cornsilk","crimson",proteo.gui.CheckboxType.SimpleSquare,20,100,20,20,'')

    proteo.gui.addItem(form1_gui,buttonform_gui)
    proteo.gui.addItem(form1_gui,checkform_gui)

    list1_gui=proteo.gui.newList('list1_gui','List','Helvetica',30,"darkblue","blue",MID_X+300,150,200,400,"")

    for i=1,50 do
        list_item_gui=proteo.gui.newListItem("list"..i.."_item_gui","Item "..i,'Helvetica',20,"deepskyblue","darkslateblue","","",200,30)
        proteo.gui.addItem(list1_gui,list_item_gui)
    end
    
    container_gui=proteo.gui.newContainer('container_gui',"blue",MID_X-300,MID_Y+150,250,250)


    guiForm=Form('guiForm')
    guiForm.backgroundColor="#000000"
    guiForm:addControl(t_gui)
    guiForm:addControl(back_gui)

    guiForm:addControl(label1_gui)
    guiForm:addControl(label2_gui)
    guiForm:addControl(label3_gui)

    guiForm:addControl(drop_gui)

    guiForm:addControl(button1_gui)
    guiForm:addControl(button2_gui)
    guiForm:addControl(button3_gui)

    guiForm:addControl(textfield_gui)

    guiForm:addControl(check1_gui)
    guiForm:addControl(check2_gui)
    guiForm:addControl(check3_gui)
    guiForm:addControl(check4_gui)
    guiForm:addControl(check5_gui)
    guiForm:addControl(check6_gui)

    guiForm:addControl(radio1_gui)
    guiForm:addControl(radio2_gui)
    guiForm:addControl(radio3_gui)

    guiForm:addControl(form1_gui)
    guiForm:addControl(list1_gui)

    guiForm:addControl(container_gui)

    forms["guiForm"]=guiForm
    forms["guiForm"]:show()

    update_function=gui_update
end

function gui_update(dt)

end

------------------
--+++GRAPHICS+++--
------------------

graphics_host = nil
graphics_server = nil

delta=0.1

base_position={}
base_position["Nose"]={x=320,y=70}
base_position["LeftEye"]={x=350,y=30}
base_position["RightEye"]={x=290,y=30}
base_position["LeftEar"]={x=380,y=70}
base_position["RightEar"]={x=260,y=70}
base_position["Neck"]={x=320,y=170}

--base_position["LeftShoulder"]={x=470,y=440}
--base_position["RightShoulder"]={x=170,y=440}
--base_position["LeftElbow"]={x=620,y=440}
--base_position["RightElbow"]={x=20,y=440}
--base_position["LeftWrist"]={x=770,y=440}
--base_position["RightWrist"]={x=-130,y=440}

--base_position["LeftShoulder"]={x=440,y=440}
--base_position["RightShoulder"]={x=200,y=440}
--base_position["LeftElbow"]={x=540,y=440}
--base_position["RightElbow"]={x=100,y=440}
--base_position["LeftWrist"]={x=440,y=440}
--base_position["RightWrist"]={x=-0,y=440}

base_position["LeftShoulder"]={x=400,y=170}
base_position["RightShoulder"]={x=240,y=170}
base_position["LeftElbow"]={x=520,y=170}
base_position["RightElbow"]={x=120,y=170}
base_position["LeftWrist"]={x=700,y=170}
base_position["RightWrist"]={x=-60,y=170}

last_smoothed_position={}

function getMove(joint_name,position)

    if last_smoothed_position[joint_name]==nil then
        last_smoothed_position[joint_name]={}
        last_smoothed_position[joint_name].v=0
        last_smoothed_position[joint_name].x=0
        last_smoothed_position[joint_name].y=0
    end

    move={}
    move.x=(base_position[joint_name].x-position.x)
    move.y=(position.y-base_position[joint_name].y)

    last_smoothed_position[joint_name].x=
        (last_smoothed_position[joint_name].x*(last_smoothed_position[joint_name].v/(last_smoothed_position[joint_name].v+position.v))) +
        (move.x*(position.v/(last_smoothed_position[joint_name].v+position.v)))

    last_smoothed_position[joint_name].y=
        (last_smoothed_position[joint_name].y*(last_smoothed_position[joint_name].v/(last_smoothed_position[joint_name].v+position.v))) +
        (move.y*(position.v/(last_smoothed_position[joint_name].v+position.v)))

    last_smoothed_position[joint_name].v=
        (last_smoothed_position[joint_name].v*last_smoothed_position[joint_name].v+position.v*position.v)/(last_smoothed_position[joint_name].v+position.v)

    return last_smoothed_position[joint_name]
end

function skeleton_clock(dt)

    --proteo.graphics.setJoint("J2",skeleton,150,math.cos(delta)*100)
    --proteo.graphics.setJoint("J3",skeleton,250,math.sin(delta)*100)
    --proteo.graphics.resetJoint("J2",skeleton)
    --proteo.graphics.resetJoint("J4",skeleton)
    proteo.graphics.moveJoint("J2",skeleton,0,math.cos(delta)*50)
    proteo.graphics.moveJoint("J4",skeleton,0,math.sin(delta)*50)
    proteo.graphics.updateSkeleton(skeleton)

--proteo.graphics.moveJoint("Nose",skeleton2,math.sin(delta)*50,0)

--proteo.graphics.moveJoint("RightElbow",skeleton2,math.abs(math.cos(delta))*(-100),math.abs(math.cos(delta))*90)
--proteo.graphics.moveJoint("RightWrist",skeleton2,math.abs(math.cos(delta))*(-150),0)

--proteo.graphics.moveJoint("LeftElbow",skeleton2,math.abs(math.cos(delta))*(100),math.abs(math.cos(delta))*90)
--proteo.graphics.moveJoint("LeftWrist",skeleton2,math.abs(math.cos(delta))*(240),math.abs(math.cos(delta))*170)

--proteo.graphics.updateSkeleton(skeleton2)

    delta=delta+0.2

    --if delta>2 then delta=0.1 end

end

function graphics_init()

    t_graphics=proteo.gui.newLabel('t_graphics','Pose Test','Helvetica',50,theme["title_font"],theme["title_back"],proteo.gui.LabelAlignment.Center,MID_X-200,MIN_Y+30,500,40)
    back_graphics=proteo.gui.newButton('back_graphics',"Back",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MAX_Y - 100,100,50,"back")
    
   --lena_img=proteo.graphics.newImage("lena.jpg","lena.jpg",900,200,325,183)

    left_frame=proteo.opencv.img()
    left_img=proteo.graphics.newImage("webcam","@webcam",MIN_X+200,MID_Y-100,150,112)
    
    left_cap=proteo.opencv.videocapture(proteo.system.document().."demo.avi")
    --left_writer=proteo.opencv.videowriter("demo.avi",proteo.opencv.fourcc.MJPG,10,640,480)
    proteo.opencv.setFrameSize(left_cap,640,480)
    proteo.opencv.setBufferSize(left_cap,3)
    --proteo.opencv.frame(form.opencv_cap,form.opencv_frame)
    
    proteo.opencv.frame(left_cap,left_frame)

    polygon=proteo.graphics.newPolygon("poly","white","white",0,0)
    
    --for i=10,500,10 do
    --    proteo.graphics.addPoint(polygon,i,25)
    --end
    --for i=500,10,-10 do
    --    proteo.graphics.addPoint(polygon,i,75)
    --end

    for i=30,420,60 do
    	proteo.graphics.addBezier(polygon,i,25,i+20,25,i+40,25,i+60,25,0.1)
	end

	for i=450,60,-60 do
    	proteo.graphics.addBezier(polygon,i,75,i-20,75,i-40,75,i-60,75,0.1)
	end

    --proteo.graphics.addPoint(polygon,300,75)
    --proteo.graphics.addPoint(polygon,150,75)
    --proteo.graphics.addPoint(polygon,10,75)

    shape=proteo.graphics.newShape("shape",50,50)
    proteo.graphics.addItem(shape,polygon)

    --proteo.graphics.addBezier(polygon,10,10,100,50,50,100,150,150)

    svgfile=proteo.graphics.loadSVG(proteo.system.document().."boy.svg")
    shape2=proteo.graphics.newShape("shape2",150,150)
    for i=1,#svgfile do
        print("Shape "..i..") "..svgfile[i].id.." color: "..svgfile[i].fill_color)
        for j=1,#svgfile[i] do
            print("   Path "..j)
            local p=proteo.graphics.newPolygon(svgfile[i].id.."_"..j,"clear",svgfile[i].fill_color,0,0)
            proteo.graphics.addItem(shape2,p)
            for k=1,#svgfile[i][j] do
                print("      Bezier "..k..") x0:"..svgfile[i][j][k].x0.." y0:"..svgfile[i][j][k].y0.." x3:"..svgfile[i][j][k].x3.." y3:"..svgfile[i][j][k].y3)
                proteo.graphics.addBezier(p,svgfile[i][j][k].x0,svgfile[i][j][k].y0,
                    svgfile[i][j][k].x1,svgfile[i][j][k].y1,
                    svgfile[i][j][k].x2,svgfile[i][j][k].y2,
                    svgfile[i][j][k].x3,svgfile[i][j][k].y3)
            end
        end
    end
    

    skeleton=proteo.graphics.newSkeleton("SK1")
    proteo.graphics.addJoint("J1",skeleton,50,50)
    proteo.graphics.addJoint("J2",skeleton,150,50)
    proteo.graphics.addJoint("J3",skeleton,250,50)
    proteo.graphics.addJoint("J4",skeleton,350,50)
    proteo.graphics.addBone("B1",skeleton,"J1","J2")
    proteo.graphics.addBone("B2",skeleton,"J2","J3")
    proteo.graphics.addBone("B3",skeleton,"J3","J4")

    proteo.graphics.bindSkeleton(skeleton,shape)

    skeleton2=proteo.graphics.newSkeleton("SK2")
    --proteo.graphics.addJoint("LeftEar",skeleton2,450,190)
    --proteo.graphics.addJoint("RightEar",skeleton2,550,190)
    --proteo.graphics.addJoint("LeftEye",skeleton2,480,180)
    --proteo.graphics.addJoint("RightEye",skeleton2,520,180)
    --[[proteo.graphics.addJoint("Nose",skeleton2,500,200)
    proteo.graphics.addJoint("Neck",skeleton2,500,300)
    proteo.graphics.addJoint("RightShoulder",skeleton2,550,300)
    proteo.graphics.addJoint("LeftShoulder",skeleton2,450,300)
    proteo.graphics.addJoint("RightElbow",skeleton2,700,300)
    proteo.graphics.addJoint("LeftElbow",skeleton2,300,300)
    proteo.graphics.addJoint("RightWrist",skeleton2,850,300)
    proteo.graphics.addJoint("LeftWrist",skeleton2,150,300)
    proteo.graphics.addJoint("RightHip",skeleton2,550,550)
    proteo.graphics.addJoint("LeftHip",skeleton2,450,550)
    proteo.graphics.addJoint("RightKnee",skeleton2,550,650)
    proteo.graphics.addJoint("LeftKnee",skeleton2,450,650)
    proteo.graphics.addJoint("RightAnkle",skeleton2,550,750)
    proteo.graphics.addJoint("LeftAnkle",skeleton2,450,750)]]--

    proteo.graphics.addJoint("Nose",skeleton2,170+base_position["Nose"].x,130+base_position["Nose"].y)
    proteo.graphics.addJoint("Neck",skeleton2,170+base_position["Neck"].x,130+base_position["Neck"].y)
    proteo.graphics.addJoint("RightShoulder",skeleton2,170+base_position["LeftShoulder"].x,130+base_position["LeftShoulder"].y)
    proteo.graphics.addJoint("LeftShoulder",skeleton2,170+base_position["RightShoulder"].x,130+base_position["RightShoulder"].y)
    proteo.graphics.addJoint("RightElbow",skeleton2,170+base_position["LeftElbow"].x,130+base_position["LeftElbow"].y)
    proteo.graphics.addJoint("LeftElbow",skeleton2,170+base_position["RightElbow"].x,130+base_position["RightElbow"].y)
    proteo.graphics.addJoint("RightWrist",skeleton2,170+base_position["LeftWrist"].x,130+base_position["LeftWrist"].y)
    proteo.graphics.addJoint("LeftWrist",skeleton2,170+base_position["RightWrist"].x,130+base_position["RightWrist"].y)
    proteo.graphics.addJoint("RightHip",skeleton2,550,550)
    proteo.graphics.addJoint("LeftHip",skeleton2,450,550)
    proteo.graphics.addJoint("RightKnee",skeleton2,550,650)
    proteo.graphics.addJoint("LeftKnee",skeleton2,450,650)
    proteo.graphics.addJoint("RightAnkle",skeleton2,550,750)
    proteo.graphics.addJoint("LeftAnkle",skeleton2,450,750)

    --proteo.graphics.addBone("F5",skeleton2,"LeftEye","LeftEar","G1")
    --proteo.graphics.addBone("F4",skeleton2,"RightEye","RightEar","G1")
    --proteo.graphics.addBone("F3",skeleton2,"Nose","LeftEye","G1")
    --proteo.graphics.addBone("F2",skeleton2,"Nose","RightEye","G1")
    LeftArm=1
    RightArm=2
    Head=0
    LeftLeg=3
    RightLeg=4
    Body=5

    proteo.graphics.addBone("F1",skeleton2,"Neck","Nose",Body)
    proteo.graphics.addBone("B1",skeleton2,"Neck","LeftShoulder",LeftArm)
    proteo.graphics.addBone("B2",skeleton2,"Neck","RightShoulder",RightArm)
    proteo.graphics.addBone("B3",skeleton2,"LeftShoulder","LeftElbow",LeftArm)
    proteo.graphics.addBone("B4",skeleton2,"RightShoulder","RightElbow",RightArm)
    proteo.graphics.addBone("B5",skeleton2,"LeftElbow","LeftWrist",LeftArm)
    proteo.graphics.addBone("B6",skeleton2,"RightElbow","RightWrist",RightArm)
    proteo.graphics.addBone("B7",skeleton2,"Neck","LeftHip",Body)
    proteo.graphics.addBone("B8",skeleton2,"Neck","RightHip",Body)
    proteo.graphics.addBone("L1",skeleton2,"LeftHip","LeftKnee",Body)
    proteo.graphics.addBone("L2",skeleton2,"RightHip","RightKnee",Body)
    proteo.graphics.addBone("L3",skeleton2,"LeftKnee","LeftAnkle",Body)
    proteo.graphics.addBone("L4",skeleton2,"RightKnee","RightAnkle",Body)

    --proteo.graphics.saveSkeleton(skeleton2,proteo.system.document().."skeleton.json")

    proteo.graphics.bindSkeleton(skeleton2,shape2)
    proteo.graphics.updateSkeleton(skeleton2)

	proteo.graphics.hideSkeleton(skeleton2,true)

    --proteo.graphics.moveJoint("J2",skeleton,0,50)
    --proteo.graphics.moveJoint("J4",skeleton,0,50)
    --proteo.graphics.updateSkeleton(skeleton)

    sk_timer=proteo.system.createTimer(100,skeleton_clock)
    proteo.system.startTimer(sk_timer)

    proteo.network.proteo_post("/test/starthost","{}","start_callback")

    graphicsForm=Form('graphicsForm')
    graphicsForm.backgroundColor="#000000"

    --graphicsForm:addControl(lena_img)

    graphicsForm:addControl(t_graphics)
    graphicsForm:addControl(back_graphics)

    graphicsForm:addControl(left_img)

    graphicsForm:addControl(shape)
    graphicsForm:addControl(shape2)

    forms["graphicsForm"]=graphicsForm
    forms["graphicsForm"]:show()

end

POSE_PAIRS =  {{"Nose", "Neck"}, {"Neck", "LeftShoulder"},{"Neck","RightShoulder"}
                ,{"LeftShoulder", "LeftElbow"},{"RightShoulder","RightElbow"},{"LeftElbow", "LeftWrist"},{"RightElbow","RightWrist"}
                ,{"Neck","LeftHip"},{"Neck","RightHip"},{"LeftHip","LeftKnee"},{"RightHip","RightKnee"}}

function graphics_update(dt)

    proteo.zmq.send (zmq_socket_test, "Hello", 5)
    buffer=proteo.zmq.recv(zmq_socket_test, 10)
    if buffer~=nil then print(buffer)  end
        

    local event = graphics_host:service(0)

    if event then
        if event.type == "connect" then
            print("Connected to", event.peer)

            proteo.opencv.frame(left_cap,left_frame)
            --proteo.opencv.write(left_writer,left_frame)
            proteo.graphics.changeImage(left_img,left_frame)

            tmp=proteo.opencv.imencode(left_frame)

            data={}
            data.type="FRAME"
            data.frame=tmp
            js=json.encode(data)
            
            event.peer:send(js)

        elseif event.type =="disconnect" then
            print("Disconnected from", event.peer)
        elseif event.type == "receive" then
            --print("Got message: "..event.data)
            data=json.decode(event.data)
            
            proteo.opencv.frame(left_cap,left_frame)
            --proteo.opencv.write(left_writer,left_frame)
            tmp=proteo.opencv.imencode(left_frame)

            --for c=0,17 do
            --    if data.points[tostring(c)].v>0.1 then
            --        proteo.opencv.circle(left_frame,data.points[tostring(c)].x,data.points[tostring(c)].y,12,"#FF0000")
            --    end
            --end

            for k,v in pairs(data.points) do
                if v.v>0.1 then
                    --if k=="LeftShoulder" then print("LeftShoulder X: "..v.x.." Y: "..v.y) end
                    --if k=="RightShoulder" then print("RightShoulder X: "..v.x.." Y: "..v.y) end

                    if k=="Nose" or k=="Neck" or k=="LeftShoulder" or k=="LeftElbow"  or k=="LeftWrist" or k=="RightWrist" or k=="RightShoulder" or k=="RightElbow" then -- or k=="LeftEye" or k=="RightEye" or k=="LeftEar" or k=="RightEar"  then 

                        move=getMove(k,v)
                        --proteo.graphics.resetJoint(k,skeleton2)
                        proteo.graphics.moveJoint(k,skeleton2,move.x,move.y)

                        proteo.opencv.circle(left_frame,base_position[k].x,base_position[k].y,8,"#00FF00")
                    end

                    --if k=="LeftShoulder" or k=="LeftElbow"  or k=="LeftWrist" then
                    --    proteo.opencv.circle(left_frame,v.x,v.y,12,"#00FF00")
                    --else
                        proteo.opencv.circle(left_frame,v.x,v.y,8,"#FF0000")
                    --end

                    
                end
            end

            for c=1,#POSE_PAIRS do
                if data.points[POSE_PAIRS[c][1]].v>0.1 and data.points[POSE_PAIRS[c][2]].v>0.1 then
                proteo.opencv.line(left_frame,
                data.points[POSE_PAIRS[c][1]].x,
                data.points[POSE_PAIRS[c][1]].y,
                data.points[POSE_PAIRS[c][2]].x,
                data.points[POSE_PAIRS[c][2]].y,
                4,"#FFFF00")
                end
            end

            proteo.graphics.updateSkeleton(skeleton2)

            proteo.opencv.flip(left_frame,left_frame,1)
            proteo.graphics.changeImage(left_img,left_frame)

            data={}
            data.type="FRAME"
            data.frame=tmp
            js=json.encode(data)
            
            event.peer:send(js)
        end
    end
end

function start_callback(res,data)
    --proteo.system.startTimer(g_timer)

    print("CONNECT URL:"..data["url"])
    graphics_host = proteo.enet.host_create()
    graphics_server = graphics_host:connect(data["url"])
    graphics_event_timer=proteo.system.createTimer(50,graphics_update)
    proteo.system.startTimer(graphics_event_timer)

    zmq_context_test=proteo.zmq.ctx_new()
    zmq_socket_test=proteo.zmq.socket_new(zmq_context_test,proteo.zmq.sockType.ZMQ_REQ)
    proteo.zmq.connect(zmq_socket_test,"tcp://localhost:5555")
end
------------------
--+++++INIT+++++--
------------------

function init()
    t1=proteo.system.createTimer(10000,print_clock)
    proteo.system.startTimer(t1)
    
    --pose_init()
    t_menu=proteo.gui.newLabel('t_menu','Proteo Test','Helvetica',50,theme["title_font"],theme["title_back"],proteo.gui.LabelAlignment.Center,MID_X-150,MIN_Y+30,300,40)
    btn1_menu=proteo.gui.newButton('btn1_menu',"Audio Test",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 150,500,50,audio_init)
    btn2_menu=proteo.gui.newButton('btn2_menu',"OpenCV Test",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 250,500,50,"opencv_init")
    btn3_menu=proteo.gui.newButton('btn3_menu',"Enet Test",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 350,500,50,enet_init)
    btn4_menu=proteo.gui.newButton('btn4_menu',"Gui Test",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 450,500,50,gui_init)
    btn5_menu=proteo.gui.newButton('btn5_menu',"Graphics Test",'Helvetica',30,theme["button_font"],theme["button_back"],1,theme["button_border"],true,MIN_X + 20 ,MIN_Y + 550,500,50,graphics_init)
        

    menuForm=Form('menuForm')
    menuForm.backgroundColor=theme["background"]
    menuForm:addControl(t_menu)
    menuForm:addControl(btn1_menu)
    menuForm:addControl(btn2_menu)
    menuForm:addControl(btn3_menu)
    menuForm:addControl(btn4_menu)
    menuForm:addControl(btn5_menu)

    forms["menu"]=menuForm
    forms["menu"]:show()
end

function print_clock(dt)
	print("NOW:"..proteo.system.clock())
end

--------------------
--+++++UPDATE+++++--
--------------------

function update(dt)
        
    --pose()

    if update_function~=nil then update_function(dt) end
end

function keydown(k)

end

function keyup(k)

end

function release(x,y)

end

-------------------
--+++++LOCAL+++++--
-------------------
function back(sender)
    forms["menu"]:show()
end

-------------------
--+++++++++++++++--
-------------------
