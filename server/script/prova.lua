require "Form"

local json=require "json"

require "skl_utility"

-- Describe this function...
function animate(sprite, current_animation)
  ticks = current_animation[current_animation[CURRENT_FRAME] * 2 + 6]
  if current_animation[CURRENT_TICK] > ticks then
    current_animation[CURRENT_TICK] = 0
    current_animation[CURRENT_FRAME] = current_animation[CURRENT_FRAME] + 1
    if current_animation[CURRENT_FRAME] >= current_animation[N_FRAMES] then
      current_animation[CURRENT_FRAME] = current_animation[RESTART_FRAME]
    end
    frame = current_animation[current_animation[CURRENT_FRAME] * 2 + 5]
    proteo.graphics.setFrame(sprite,frame)
  end
end


require "tfl_utility"


t1 = nil
t2 = nil
t3 = nil
t4 = nil
encoder = proteo.ffmpeg.newEncoder(proteo.ffmpeg.CodecID.AV_CODEC_ID_H264,352,288)
decoder = proteo.ffmpeg.newDecoder(proteo.ffmpeg.CodecID.AV_CODEC_ID_H264)
frameEncoder = proteo.ffmpeg.newFrame(encoder)
frameDecoder = proteo.ffmpeg.newFrame(decoder)
packetEncoder = proteo.ffmpeg.newPacket()
packetDecoder = proteo.ffmpeg.newPacket()
forms = {}
theme = {}
theme['background']='#ffcc00'
print_clock = function(dt)
  print('NOW:' .. proteo.system.clock())
 end

logout = function(sender)
  proteo.network.proteo_post("/prova/stophost",'{}',nil)
  proteo.system.login('','','login')
 end

back = function(sender)
  if t2 ~= nil then
    proteo.system.stopTimer(t2)
  end
  if t3 ~= nil then
    proteo.system.stopTimer(t3)
  end
  if t4 ~= nil then
    proteo.system.stopTimer(t4)
  end
  if t5 ~= nil then
    proteo.system.stopTimer(t5)
  end
  forms['menu']:show()
 end

start_callback = function(res,data)
  print('Connect Url: ' .. data['url'])
  enet_url = data['url']
 end


gui_init = function(sender)
  t_gui = proteo.gui.newLabel("t_gui","Gui Test","Helvetica",50,"#ff0000","#3333ff",proteo.gui.LabelAlignment.Center,450,30,300,40)
  back_gui = proteo.gui.newButton("back_gui","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,600,100,50,back)
  label1_gui = proteo.gui.newLabel("label1_gui","label left","Helvetica",20,"#ff0000","#ffffff",proteo.gui.LabelAlignment.Left,25,150,200,30)
  label2_gui = proteo.gui.newLabel("label2_gui","label center","Helvetica",20,"#000000","#ffffff",proteo.gui.LabelAlignment.Center,25,190,200,30)
  label3_gui = proteo.gui.newLabel("label3_gui","label right","Helvetica",20,"#33cc00","#ffffff",proteo.gui.LabelAlignment.Right,25,230,200,30)
  drop_gui = proteo.gui.newDropDown("drop_gui","DropDown","Helvetica",20,"#996633","OPTIONNAME","#000000",proteo.gui.DropDownType.Normal,"#3366ff",325,150,200,30,"")
  drop_gui_item1 = proteo.gui.newDropDownItem("drop_gui_item1","Prova1","Helvetica",20,"#996633","#3366ff","OPTIONNAME","#ff0000",200,30)
  drop_gui_item2 = proteo.gui.newDropDownItem("drop_gui_item2","Prova2","Helvetica",20,"#996633","#3366ff","OPTIONNAME","#ff0000",200,30)
  proteo.gui.addItem(drop_gui,drop_gui_item1)
  proteo.gui.addItem(drop_gui,drop_gui_item2)
  button1_gui = proteo.gui.newButton("button1_gui","round button","Helvetica",30,"#000000","#66cccc",5,"#ff9966",true,25,300,200,50,"")
  button2_gui = proteo.gui.newButton("button2_gui","square button","Helvetica",30,"#000000","#ff9966",5,"#66cccc",false,325,300,200,50,"")
  textfield_gui = proteo.gui.newTextField("textfield_gui","","Helvetica",25,"#000000","#ffffff",25,400,500,30,"")
  guiForm=Form('guiForm')
  guiForm.backgroundColor='#3333ff'
  guiForm:addControl(t_gui)
  guiForm:addControl(back_gui)
  guiForm:addControl(label1_gui)
  guiForm:addControl(label2_gui)
  guiForm:addControl(label3_gui)
  guiForm:addControl(drop_gui)
  guiForm:addControl(button1_gui)
  guiForm:addControl(button2_gui)
  guiForm:addControl(textfield_gui)
  forms['guiForm']=guiForm
  forms['guiForm']:show()
 end


farfalla_x = 0
farfalla_y = 0
d_x = 0
d_y = 0
CURRENT_FRAME = 2
CURRENT_TICK = 1
N_FRAMES = 3
RESTART_FRAME = 4
-- current_tick,current_frame,num_frames,restar_frame,{frame,ticks}
farfalla_animation = {{0,0,33,0,1,1, 2,1, 3,1, 4,1, 5,1, 6,1, 7,1, 8,1, 9,1, 10,1, 11,1, 12,1, 13,1, 14,1, 15,1, 16,1, 17,1, 18,1, 19,1, 20,1, 21,1, 22,1, 23,1, 24,1, 25,1, 26,1, 27,1, 28,1, 29,1, 30,1, 31,1, 32,1, 33,1,34,1},
}
farfalla_animation = farfalla_animation[1]
graphics_event = function(dt)

  farfalla_animation[CURRENT_TICK] = farfalla_animation[CURRENT_TICK] + 1
  if math.random(1, 50) == 1 then
    d_x = math.random(-2, 2)
    d_y = math.random(-2, 2)
  end
  proteo.graphics.flipH(farfalla,(d_x < 0))
  farfalla_x = farfalla_x + d_x
  farfalla_y = farfalla_y + d_y
  if farfalla_x <= MIN_X then
    d_x = d_x * -1
    farfalla_x = MIN_X + 10
  end
  if farfalla_x >= MAX_X then
    d_x = d_x * -1
    farfalla_x = MAX_X - 10
  end
  if farfalla_y <= MIN_Y then
    d_y = d_y * -1
    farfalla_y = MIN_Y + 10
  end
  if farfalla_y >= MAX_Y then
    d_y = d_y * -1
    farfalla_y = MAX_Y - 10
  end
  proteo.graphics.setPosition(farfalla,farfalla_x,farfalla_y)
  animate(farfalla, farfalla_animation)
 end

skeleton = nil
shape = nil
skl_read_callback = function(data,filename)
  json2skl(json.decode(data),skeleton)
  proteo.graphics.bindSkeleton(skeleton,shape)
  proteo.graphics.updateSkeleton(skeleton)
 end

--shader=nil
shader_u_time=2.0

function render(my_shader)
  if my_shader~=nil then
    proteo.graphics.setUniform(my_shader,"u_time",shader_u_time)
    shader_u_time=shader_u_time+0.1
  end
end

particle_option={}

function endLife(sender)

  particle_option.frame_x=0
  particle_option.frame_y=0
  particle_option.frame_width=256
  particle_option.frame_height=256

  particle_option.startSpeed=80.0+20*math.random()
  particle_option.startDirection=(-3.14/2)-(math.random()-0.5)
  particle_option.lifeTime=3000*math.random()
  particle_option.linearAccelerationX=150.0 + 50*math.random()
  particle_option.linearAccelerationY=50.0 + 50*math.random()
  particle_option.angularSpeed=0 -- 1.5708

  particle_option.position_x=20*math.random()-10
  particle_option.position_y=20*math.random()-10

  particle_option.spin=5.0*math.random()

  particle_option.startSize=1.0
    particle_option.endSize=2.0

  particle_option.startColor="#aae25822"
  particle_option.endColor="#00bbaaff"

  proteo.graphics.setParticle(sender,particle_option)
  proteo.graphics.setPosition(sender,farfalla_x,farfalla_y)
end

graphics_init = function(sender)
  t5 = proteo.system.createTimer(20,graphics_event)
  proteo.system.startTimer(t5)
  t_graphics = proteo.gui.newLabel("t_graphics","Graphics Test","Helvetica",50,"#ff0000","#000000",proteo.gui.LabelAlignment.Center,350,30,500,40)
  back_graphics = proteo.gui.newButton("back_graphics","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,600,100,50,back)
  farfalla = proteo.graphics.newSprite("farfalla",proteo.system.document()..'butterfly_fly-angle1.png',(math.random(1, MAX_X)),(math.random(1, MAX_Y)),70,65)
  for i = 0, 11, 1 do
    proteo.graphics.addFrame(farfalla,(i * 70),0,70,65)
  end
  for i = 0, 11, 1 do
    proteo.graphics.addFrame(farfalla,(i * 70),65,70,65)
  end
  for i = 0, 9, 1 do
    proteo.graphics.addFrame(farfalla,(i * 70),130,70,65)
  end
  proteo.graphics.setFrame(farfalla,0)
  svgfile = proteo.graphics.loadSVG(proteo.system.document()..'boy.svg')
  shape = proteo.graphics.newShape("shape",150,150)
  svg2shape(svgfile,shape)
  skeleton = proteo.graphics.newSkeleton("skeleton")
  --proteo.system.readFile(proteo.system.document()..'skeleton.json',skl_read_callback)
  rect_graphics=proteo.graphics.newRect("rect","white","",100,100,200,200)

math.randomseed( os.time() )
  
  particle_option.frame_x=0
  particle_option.frame_y=0
  particle_option.frame_width=128
  particle_option.frame_height=128
  particle_option.startColor="#aae25822"
  particle_option.endColor="#00bbaaff"

particles={}
  for p=1,500 do
    particle_option.startSpeed=80.0+20*math.random()
    particle_option.startDirection=(-3.14/2)-(math.random()-0.5)
    particle_option.lifeTime=3000*math.random()
    particle_option.linearAccelerationX=150.0 + 50*math.random()
    particle_option.linearAccelerationY=50.0 + 50*math.random()
    particle_option.angularSpeed=0 -- 1.5708
    particle_option.spin=5.0*math.random()
    particle_option.startSize=1.0
    particle_option.endSize=2.0

    particles[p]=proteo.graphics.newParticle("P"..p,proteo.system.document().."sparkle.png",500,200,32,32,particle_option,endLife)
  end

if SHADER==1 then
  shader=proteo.graphics.newShader([[ 
    void main()
    {
      gl_Position = ftransform();
    }
]],[[
    uniform float u_time;

    void main()
    {
      //gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
      gl_FragColor = vec4(abs(sin(u_time)),0.0,0.0,1.0);
    }
]])

proteo.graphics.setShader(rect_graphics,shader,render)
end

  graphicsForm=Form('graphicsForm')
  graphicsForm.backgroundColor='#000000'
  graphicsForm:addControl(t_graphics)
  graphicsForm:addControl(back_graphics)
  graphicsForm:addControl(farfalla)
  graphicsForm:addControl(shape)
  graphicsForm:addControl(rect_graphics)
  --graphicsForm:addControl(part)
  for p=1,500 do
    graphicsForm:addControl(particles[p])
  end

  forms['graphicsForm']=graphicsForm
  forms['graphicsForm']:show()
 end


play = function(sender)
  proteo.audio.playAudio(dev_audio_out,wav_audio,100,nil)
 end

audio_init = function(sender)
  t_audio = proteo.gui.newLabel("t_audio","Audio Test","Helvetica",50,"#000000","#99ff99",proteo.gui.LabelAlignment.Center,450,30,300,40)
  back_audio = proteo.gui.newButton("back_audio","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,400,100,50,back)
  btn1_audio = proteo.gui.newButton("btn1_audio","Play","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,200,100,50,play)
  proteo.audio.infoDrivers()
  if proteo.audio.init() then
    proteo.audio.infoDevices()
  end
  dev_audio_out = proteo.audio.openDevice(nil,false,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
  dev_audio_in = proteo.audio.openDevice(nil,true,48000,proteo.audio.Format.AUDIO_F32SYS,2,4096)
  proteo.audio.pauseDevice(dev_audio_out,false)
  proteo.audio.pauseDevice(dev_audio_in,false)
  wav_audio = proteo.audio.loadWav(proteo.system.document()..'wav_test.wav')
  proteo.audio.convertAudio(wav_audio,48000,proteo.audio.Format.AUDIO_F32SYS,2)
  audioForm=Form('audioForm')
  audioForm.backgroundColor='#000000'
  audioForm:addControl(t_audio)
  audioForm:addControl(back_audio)
  audioForm:addControl(btn1_audio)
  forms['audioForm']=audioForm
  forms['audioForm']:show()
 end


function close()
  proteo.network.proteo_post("/prova/stophost",'{}',nil)
 end

opencv_frame = nil
opencv_cap = nil
opencv_img = nil
opencv_event = function(dt)
  proteo.opencv.frame(opencv_cap,opencv_frame)
  proteo.opencv.flip(opencv_frame,opencv_frame,1)
  proteo.graphics.changeImage(opencv_img,opencv_frame)
 end

opencv_init = function(sender)
  opencv_frame = proteo.opencv.img()
  opencv_resized = proteo.opencv.img()
  opencv_cap = proteo.opencv.videocapture(0)
  proteo.opencv.setFrameSize(opencv_cap,640,480)
  proteo.opencv.setBufferSize(opencv_cap,3)
  proteo.opencv.frame(opencv_cap,opencv_frame)
  t3 = proteo.system.createTimer(10,opencv_event)
  proteo.system.startTimer(t3)
  t_opencv = proteo.gui.newLabel("t_opencv","OpenCV Test","Helvetica",50,"#000000","#ff0000",proteo.gui.LabelAlignment.Center,450,30,400,40)
  opencv_img = proteo.graphics.newImage("webcam","",300,400,400,400)
  back_opencv = proteo.gui.newButton("back_opencv","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,700,100,50,back)
  proteo.graphics.changeImage(opencv_img,opencv_frame)
  opencvForm=Form('opencvForm')
  opencvForm.backgroundColor='#ff0000'
  opencvForm:addControl(t_opencv)
  opencvForm:addControl(opencv_img)
  opencvForm:addControl(back_opencv)
  forms['opencvForm']=opencvForm
  forms['opencvForm']:show()
 end


function init()
  t1 = proteo.system.createTimer(10000,print_clock)
  proteo.system.startTimer(t1)
  proteo.network.proteo_post("/prova/starthost",'{}',start_callback)
  t_menu = proteo.gui.newLabel("t_menu","Proteo Test","Helvetica",50,"#000000","#ffcc00",proteo.gui.LabelAlignment.Left,100,30,300,40)
  logout_button = proteo.gui.newButton("logout_menu","X","Helvetica",20,"#000000","#99ff99",1,"#006600",false,600,100,30,30,logout)
  btn1_menu = proteo.gui.newButton("btn1_menu","Audio Test","Helvetica",30,"#000000","#99ff99",1,"#006600",true,20,150,500,50,audio_init)
  btn2_menu = proteo.gui.newButton("btn2_menu","OpenCV Test","Helvetica",30,"#000000","#99ff99",1,"#006600",true,20,250,500,50,opencv_init)
  btn3_menu = proteo.gui.newButton("btn3_menu","Enet Test","Helvetica",30,"#000000","#99ff99",1,"#006600",true,20,350,500,50,enet_init)
  btn4_menu = proteo.gui.newButton("btn4_menu","Zmq Test","Helvetica",30,"#000000","#99ff99",1,"#006600",true,20,450,500,50,zmq_init)
  btn5_menu = proteo.gui.newButton("btn5_menu","Gui Test","Helvetica",30,"#000000","#99ff99",1,"#006600",true,20,550,500,50,gui_init)
  btn6_menu = proteo.gui.newButton("btn5_menu","Graphics Test","Helvetica",30,"#000000","#99ff99",1,"#006600",true,20,650,500,50,graphics_init)
  menuForm=Form('menuForm')
  menuForm.backgroundColor=theme['background']
  menuForm:addControl(logout_button)
  menuForm:addControl(t_menu)
  menuForm:addControl(btn1_menu)
  menuForm:addControl(btn2_menu)
  menuForm:addControl(btn3_menu)
  menuForm:addControl(btn4_menu)
  menuForm:addControl(btn5_menu)
  menuForm:addControl(btn6_menu)
  forms['menu']=menuForm
  forms['menu']:show()
 end

enet_host = nil
enet_resized = nil
enet_frame = nil
enet_cap = nil
enet_img = nil
enet_url = nil
enet_event = function(dt)
  event = enet_host:service(0)
  if event then
   if event.type == "connect" then
    print('Connect to ')
    print(event.peer)
    proteo.opencv.frame(enet_cap,enet_frame)
    proteo.opencv.resize(enet_frame,enet_resized)
    proteo.ffmpeg.copyImage(frameEncoder,enet_resized)
    res = proteo.ffmpeg.encodeFrame(encoder, frameEncoder, packetEncoder)
    if res == 0 then
      encoded = proteo.ffmpeg.encodePacket(packetEncoder)
    end
    data = {}
    data['type']='VIDEO'
    data['ffmpeg']=encoded
    event.peer:send(json.encode(data))

   elseif event.type == "receive" then
    data = json.decode(event.data)
    if data['type'] == 'VIDEO' then
      proteo.opencv.frame(enet_cap,enet_frame)
      proteo.opencv.resize(enet_frame,enet_resized)
      proteo.ffmpeg.copyImage(frameEncoder,enet_resized)
      res = proteo.ffmpeg.encodeFrame(encoder, frameEncoder, packetEncoder)
      if res == 0 then
        encoded = proteo.ffmpeg.encodePacket(packetEncoder)
      end
      if data['ffmpeg'] ~= nil then
        proteo.ffmpeg.decodePacket(packetDecoder,data['ffmpeg'])
        res = proteo.ffmpeg.decodeFrame(decoder,packetDecoder,frameDecoder)
        if res == 0 then
          proteo.ffmpeg.copyFrame(enet_resized,frameDecoder)
          proteo.graphics.changeImage(enet_img,enet_resized)
        end
      end
      data['ffmpeg']=encoded
      event.peer:send(json.encode(data))
    end

   end
  end
 end

enet_init = function(sender)
  enet_host = proteo.enet.host_create()
  enet_server = enet_host:connect(enet_url)
  enet_frame = proteo.opencv.img()
  enet_resized = proteo.opencv.img()
  proteo.opencv.setImg(enet_resized,352,288,"#000000")
  enet_cap = proteo.opencv.videocapture(0)
  proteo.opencv.setFrameSize(enet_cap,640,480)
  proteo.opencv.setBufferSize(enet_cap,3)
  proteo.opencv.frame(enet_cap,enet_frame)
  t2 = proteo.system.createTimer(10,enet_event)
  proteo.system.startTimer(t2)
  t_enet = proteo.gui.newLabel("t_enet","Enet Test","Helvetica",50,"#ffff00","#000000",proteo.gui.LabelAlignment.Center,450,30,300,40)
  back_enet = proteo.gui.newButton("back_enet","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,700,100,50,back)
  enet_img = proteo.graphics.newImage("chat_webcam","",600,400,352,288)
  proteo.graphics.changeImage(enet_img,enet_frame)
  enetForm=Form('enetForm')
  enetForm.backgroundColor='#000000'
  enetForm:addControl(t_enet)
  enetForm:addControl(back_enet)
  enetForm:addControl(enet_img)
  forms['enetForm']=enetForm
  forms['enetForm']:show()
 end


zmq_context_test = nil
zmq_socket_test = nil
zmq_frame = nil
zmq_resized = nil
zmq_event = function(dt)
  buffer = proteo.zmq.recv(zmq_socket_test,proteo.zmq.flag.ZMQ_DONTWAIT)
  if buffer ~= nil then
    data = json.decode(buffer)
    if data['type'] == 'VIDEO' then
      proteo.opencv.imdecode(data['data'],zmq_resized)
      for _, b in ipairs(data['bbox']) do
        landmark = show_landmark(b,zmq_resized,b['facemesh'])
      end
      proteo.graphics.changeImage(zmq_img,zmq_resized)
      proteo.opencv.frame(zmq_cap,zmq_frame)
      proteo.opencv.resize(zmq_frame,zmq_resized)
      tmp = proteo.opencv.imencode(zmq_resized)
      data['data']=tmp
      proteo.zmq.send (zmq_socket_test,json.encode(data),proteo.zmq.flag.ZMQ_DONTWAIT)
    end
  end
 end

zmq_init = function(sender)
  zmq_context_test = proteo.zmq.ctx_new()
  zmq_socket_test = proteo.zmq.socket_new(zmq_context_test,proteo.zmq.sockType.ZMQ_REQ)
  proteo.zmq.connect(zmq_socket_test,'tcp://192.168.1.77:5555')
  zmq_frame = proteo.opencv.img()
  zmq_resized = proteo.opencv.img()
  proteo.opencv.setImg(zmq_resized,320,240,"#000000")
  zmq_cap = proteo.opencv.videocapture(0)
  proteo.opencv.setFrameSize(zmq_cap,640,480)
  proteo.opencv.setBufferSize(zmq_cap,3)
  proteo.opencv.frame(zmq_cap,zmq_frame)
  proteo.opencv.resize(zmq_frame,zmq_resized)
  tmp = proteo.opencv.imencode(zmq_resized)
  data = {}
  data['type']='VIDEO'
  data['data']=tmp
  proteo.zmq.send (zmq_socket_test,json.encode(data),proteo.zmq.flag.ZMQ_DONTWAIT)
  t4 = proteo.system.createTimer(10,zmq_event)
  proteo.system.startTimer(t4)
  t_zmq = proteo.gui.newLabel("t_zmq","Zmq Test","Helvetica",50,"#000000","#66ff99",proteo.gui.LabelAlignment.Center,450,30,300,40)
  back_zmq = proteo.gui.newButton("back_zmq","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,700,100,50,back)
  zmq_img = proteo.graphics.newImage("chat_webcam","",600,400,640,480)
  proteo.graphics.changeImage(zmq_img,zmq_frame)
  zmqForm=Form('zmqForm')
  zmqForm.backgroundColor='#66ff99'
  zmqForm:addControl(t_zmq)
  zmqForm:addControl(back_zmq)
  zmqForm:addControl(zmq_img)
  forms['zmqForm']=zmqForm
  forms['zmqForm']:show()
 end
