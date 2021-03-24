require "Form"

local json=require "json"


forms = {}
theme = {}
theme['background']='#ffcc00'
print_clock = function(dt)
  print('NOW:' .. proteo.system.clock())
 end

logout = function(sender)
  proteo.system.login('','','login')
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


audio_init = function(sender)
 end


opencv_frame = nil
opencv_cap = nil
opencv_img = nil
opencv_event = function(dt)
  proteo.opencv.frame(opencv_cap,opencv_frame)
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
  t_opencv = proteo.gui.newLabel("t_opencv","OpenCV Test","Helvetica",50,"#000000","#99ff99",proteo.gui.LabelAlignment.Center,450,30,300,40)
  opencv_img = proteo.graphics.newImage("webcam","",300,400,400,400)
  back_opencv = proteo.gui.newButton("back_opencv","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,400,100,50,back)
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
  menuForm=Form('menuForm')
  menuForm.backgroundColor=theme['background']
  menuForm:addControl(t_menu)
  menuForm:addControl(logout_button)
  menuForm:addControl(btn1_menu)
  menuForm:addControl(btn2_menu)
  menuForm:addControl(btn3_menu)
  menuForm:addControl(btn4_menu)
  menuForm:addControl(btn5_menu)
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
    tmp = proteo.opencv.imencode(enet_resized)
    data = {}
    data['type']='VIDEO'
    data['data']=tmp
    event.peer:send(json.encode(data))

   elseif event.type == "receive" then
    data = json.decode(event.data)
    if data['type'] == 'VIDEO' then
      proteo.opencv.imdecode(data['data'],enet_resized)
      proteo.graphics.changeImage(enet_img,enet_resized)
      proteo.opencv.frame(enet_cap,enet_frame)
      proteo.opencv.resize(enet_frame,enet_resized)
      tmp = proteo.opencv.imencode(enet_resized)
      data['data']=tmp
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
  proteo.opencv.setImg(enet_resized,100,100,"#000000")
  enet_cap = proteo.opencv.videocapture(0)
  proteo.opencv.setFrameSize(enet_cap,640,480)
  proteo.opencv.setBufferSize(enet_cap,3)
  proteo.opencv.frame(enet_cap,enet_frame)
  t2 = proteo.system.createTimer(10,enet_event)
  proteo.system.startTimer(t2)
  t_enet = proteo.gui.newLabel("t_enet","Enet Test","Helvetica",50,"#000000","#99ff99",proteo.gui.LabelAlignment.Center,450,30,300,40)
  back_enet = proteo.gui.newButton("back_enet","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,400,100,50,back)
  enet_img = proteo.graphics.newImage("chat_webcam","",300,400,400,400)
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
  proteo.zmq.connect(zmq_socket_test,'tcp://localhost:5555')
  zmq_frame = proteo.opencv.img()
  zmq_resized = proteo.opencv.img()
  proteo.opencv.setImg(zmq_resized,100,100,"#000000")
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
  t_zmq = proteo.gui.newLabel("t_zmq","Zmq Test","Helvetica",50,"#000000","#99ff99",proteo.gui.LabelAlignment.Center,450,30,300,40)
  back_zmq = proteo.gui.newButton("back_zmq","Back","Helvetica",30,"#000066","#33ccff",1,"#000099",true,20,400,100,50,back)
  zmq_img = proteo.graphics.newImage("chat_webcam","",300,400,400,400)
  proteo.graphics.changeImage(zmq_img,zmq_frame)
  zmqForm=Form('zmqForm')
  zmqForm.backgroundColor='#000000'
  zmqForm:addControl(t_zmq)
  zmqForm:addControl(back_zmq)
  zmqForm:addControl(zmq_img)
  forms['zmqForm']=zmqForm
  forms['zmqForm']:show()
 end


local start_callback=(function(res,data)
  print('Connect Url: ' .. data['url'])
  enet_url = data['url']
 end
)
back = function(sender)
  forms['menu']:show()
 end
