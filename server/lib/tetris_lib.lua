local inspect = require("inspect")
local json=require "json"
require "tfl_blazepose"
require "tfl_utility"


local tetris={}
tetris.template={}
tetris.current_session = ''
tetris.pose_control=nil

tetris.left_move_left = function()
	if tetris.check_t_left(tetris.LEFT)==0 then
		tetris.t_left.x=tetris.t_left.x-1
	end
end

tetris.left_move_right = function()
	if tetris.check_t_left(tetris.RIGHT)==0 then
		tetris.t_left.x=tetris.t_left.x+1
	end
end

tetris.left_move_rotate = function()
  if tetris.check_t_left(tetris.ROTATE)==0 then
    tetris.t_left.rotate = tetris.get_rotate_left()
  end
end

--REMOTE TFL
--[[
tetris.pose =  function(data)
  for _, b in ipairs(data['data']) do
    landmarks = show_landmark(b,tetris.template.left_frame,b['landmarks'])
    show_pose(landmarks,tetris.template.left_frame)
    if tetris.pose_control~= nil then
    	tetris.pose_control(landmarks)
    end

  end
end

  
tetris.pipe =  function(json_data)
  data = json.decode(json_data)
  proteo.opencv.frame(tetris.left_cap,tetris.template.left_frame)
  tmp = proteo.opencv.imencode(tetris.template.left_frame)
  tetris.pose(data)
  proteo.opencv.flip(tetris.template.left_frame,tetris.template.left_frame,1)
  proteo.graphics.changeImage(tetris.template.left_image,tetris.template.left_frame)
  data = {}
  data['type']='FRAME'
  data['encoding']='JPEG'
  data['request']='TFLPOSE'
  data['frame']=tmp
  data['session']=tetris.current_session
  return json.encode(data)
end


tetris.update_event = function(dt)

  if tetris.zmq_socket_tetris ~= nil then
    buffer = proteo.zmq.recv(tetris.zmq_socket_tetris,proteo.zmq.flag.ZMQ_DONTWAIT)
    if buffer ~= nil then
      proteo.zmq.send (tetris.zmq_socket_tetris,(tetris.pipe(buffer)),proteo.zmq.flag.ZMQ_DONTWAIT)
    end
  end
 

 end
 ]]--
--=============

--LOCAL TFL
tetris.update_local_event = function(dt)

    proteo.opencv.frame(tetris.left_cap,tetris.template.left_frame)

    bbox = invoke_pose_detect(tetris.template.left_frame)
    size = proteo.opencv.size(tetris.template.left_frame)

    for i=1,#bbox do
      feed_image = generate_pose_landmark_input_image(tetris.template.left_frame,bbox[i])
      bbox[i].landmarks=invoke_pose_landmark(feed_image)
      landmarks = get_landmark(bbox[i],tetris.template.left_frame,bbox[i].landmarks)
      show_pose(landmarks,tetris.template.left_frame)
      tetris.pose_control(landmarks)
    end

      proteo.opencv.flip(tetris.template.left_frame,tetris.template.left_frame,1)
      proteo.graphics.changeImage(tetris.template.left_image,tetris.template.left_frame)
end
--=============

tetris.update_t_event = function(dt)
  if tetris.check_t_left(tetris.DOWN) ~= 1 then
    tetris.t_left['y']=(tetris.t_left['y'] - 1)
    tetris.update_left_square()
    tetris.update_t_left()
  else
    tetris.fix_t_left()
    tetris.new_t_left()
  end
 end
tetris.t_timer = proteo.system.createTimer(1000,tetris.update_t_event)

math.randomseed( os.time() )

tetris.t_colors = {'#ff0000', '#33ff33', '#ff9966', '#cc33cc', '#ffff00', '#33ccff', '#3333ff'}
tetris.t = {{{{0,0,0,0},
{0,0,0,0},
{1,1,0,0},
{0,1,1,0},
}, {{0,0,0,0},
{0,1,0,0},
{1,1,0,0},
{1,0,0,0},
}}, {{{0,0,0,0},
{0,0,0,0},
{0,2,2,0},
{2,2,0,0},
}, {{0,0,0,0},
{2,0,0,0},
{2,2,0,0},
{0,2,0,0},
}}, {{{0,0,0,0},
{0,0,0,0},
{3,3,3,0},
{0,0,3,0},
}, {{0,0,0,0},
{3,3,0,0},
{3,0,0,0},
{3,0,0,0},
}, {{0,0,0,0},
{0,0,0,0},
{3,0,0,0},
{3,3,3,0},
}, {{0,0,0,0},
{0,3,0,0},
{0,3,0,0},
{3,3,0,0},
}}, {{{0,0,0,0},
{0,0,0,0},
{4,4,4,0},
{0,4,0,0},
}, {{0,0,0,0},
{4,0,0,0},
{4,4,0,0},
{4,0,0,0},
}, {{0,0,0,0},
{0,0,0,0},
{0,4,0,0},
{4,4,4,0},
}, {{0,0,0,0},
{0,4,0,0},
{4,4,0,0},
{0,4,0,0},
}}, {{{0,0,0,0},
{0,0,0,0},
{5,5,0,0},
{5,5,0,0},
}}, {{{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{6,6,6,6},
}, {{6,0,0,0},
{6,0,0,0},
{6,0,0,0},
{6,0,0,0},
}}, {{{0,0,0,0},
{0,0,0,0},
{7,7,7,0},
{7,0,0,0},
}, {{0,0,0,0},
{7,0,0,0},
{7,0,0,0},
{7,7,0,0},
}, {{0,0,0,0},
{0,0,0,0},
{0,0,7,0},
{7,7,7,0},
}, {{0,0,0,0},
{0,7,7,0},
{0,7,0,0},
{0,7,0,0},
}}}
tetris.DOWN = 1
tetris.LEFT = 2
tetris.RIGHT = 3
tetris.ROTATE = 4
tetris.left_control = {}
tetris.left_control[tetris.LEFT] = false
tetris.left_control[tetris.RIGHT] = false
tetris.left_control[tetris.ROTATE] = false

tetris.template.floor_left = {{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
}

tetris.floor_left_state = {{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0},
}

tetris.t_left = {}

tetris.zmq_socket_tetris = nil

tetris.new_t_left = function()
  tetris.t_left['type']=(math.random(1, 7))
  tetris.t_left['y']=15
  tetris.t_left['x']=5
  tetris.t_left['rotate']=1
end

tetris.fix_t_left = function()
  current_t = tetris.t[tetris.t_left['type']][tetris.t_left['rotate']]
  for y = 1, 4, 1 do
    for x = 1, 4, 1 do
      value = current_t[y][x]
      if value ~= 0 then
        tetris.floor_left_state[(tetris.t_left['y'] + y)][(tetris.t_left['x'] + x)]=value
      end
    end
  end

  local y=1
    local deleted_line=0
    while y<#tetris.floor_left_state do
    --for y=1,#floor_left_state do
        local line_full=true
        for x=1,10 do       
            if tetris.floor_left_state[y][x]==0 then
                line_full=false
            end
        end
        if line_full then 
            deleted_line=deleted_line+1
            tetris.remove_line_left(y)

        else y=y+1 end
    end

end

tetris.remove_line_left = function (remove_y)
    for y=remove_y,#tetris.floor_left_state-1 do
        for x=1,10 do
            tetris.floor_left_state[y][x]=tetris.floor_left_state[y+1][x]
        end
    end
end

tetris.get_rotate_left = function()
  type2 = tetris.t_left['type']
  if type2 == 5 then
    return 1
  end
  if type2 == 5 then
    return 1 + tetris.t_left['rotate'] % 4
  end
  if type2 == 4 then
    return 1 + tetris.t_left['rotate'] % 4
  end
  if type2 == 7 then
    return 1 + tetris.t_left['rotate'] % 4
  end
  return 1 + tetris.t_left['rotate'] % 2
end

tetris.check_t_left = function(d)
  d_y = 0
  d_x = 0
  d_r = tetris.t_left['rotate']
  if d == tetris.DOWN then
    d_y = -1
  elseif d == tetris.LEFT then
    d_x = -1
  elseif d == tetris.RIGHT then
    d_x = 1
  elseif d == tetris.ROTATE then
    d_r = tetris.get_rotate_left()
  end
  current_t = tetris.t[tetris.t_left['type']][d_r]
  for y = 1, 4, 1 do
    for x = 1, 4, 1 do
      if (current_t[y][x]) ~= 0 then
        n_y = tetris.t_left['y'] + (y + d_y)
        n_x = tetris.t_left['x'] + (x + d_x)
        if n_y <= 0 then
          return 1
        end
        if n_x <= 0 then
          return 1
        end
        if n_x > 10 then
          return 1
        end
        if (tetris.floor_left_state[n_y][n_x]) ~= 0 then
          return 1
        end
      end
    end
  end
  return 0
end

--REMOTE TFL
--[[
tetris.start_callback = function(res,data)
  tetris.current_session = data['session']
  proteo.system.stopTimer(tetris.webcam_timer)
  proteo.system.startTimer(tetris.t_timer)

  
  tetris.zmq_context_tetris = proteo.zmq.ctx_new()
  tetris.zmq_socket_tetris = proteo.zmq.socket_new(tetris.zmq_context_tetris,proteo.zmq.sockType.ZMQ_REQ)
  proteo.zmq.connect(tetris.zmq_socket_tetris,'tcp://poseidone.irib.cloud:5555')

  proteo.opencv.frame(tetris.left_cap,tetris.template.left_frame)
  proteo.opencv.flip(tetris.template.left_frame,tetris.template.left_frame,1)
  proteo.graphics.changeImage(tetris.template.left_image,tetris.template.left_frame)

  tmp = proteo.opencv.imencode(tetris.template.left_frame)
  data = {}
  data['type']='FRAME'
  data['frame']=tmp
  data['session']=tetris.current_session
  data['encoding']='JPEG'
  data['request']='TFLPOSE'
  proteo.zmq.send (tetris.zmq_socket_tetris,json.encode(data),proteo.zmq.flag.ZMQ_DONTWAIT)

  tetris.event_timer = proteo.system.createTimer(50,tetris.update_event)
  proteo.system.startTimer(tetris.event_timer)
 end
 ]]--
  --===========

tetris.start = function(sender)

--REMOTE TFL
--proteo.network.proteo_post("/deepcrimson/start",'{}',tetris.start_callback)
--LOCAL TFL
  proteo.system.stopTimer(tetris.webcam_timer)
  proteo.system.startTimer(tetris.t_timer)
  tetris.event_timer = proteo.system.createTimer(50,tetris.update_local_event)
  proteo.system.startTimer(tetris.event_timer)
--==========
 end

tetris.update_left_square = function()
  for y = 1, 20, 1 do
    for x = 1, 10, 1 do
      square = tetris.template.floor_left[y][x]
      col = square['col']
      border = square['border']
      state = tetris.floor_left_state[y][x]
      if state == 0 then
        proteo.graphics.setColor(col,"clear")
        proteo.graphics.setColor(border,"clear")
      else
        proteo.graphics.setColor(col,(tetris.t_colors[state]))
        proteo.graphics.setColor(border,'#ffffff')
      end
    end
  end
end

tetris.update_t_left = function()
  current_t = tetris.t[tetris.t_left['type']][tetris.t_left['rotate']]
  for y = 1, 4, 1 do
    for x = 1, 4, 1 do
      if (current_t[y][x]) ~= 0 then
        square = tetris.template.floor_left[(tetris.t_left['y'] + y)][(tetris.t_left['x'] + x)]
        col = square['col']
        border = square['border']
        proteo.graphics.setColor(col,(tetris.t_colors[current_t[y][x]]))
        proteo.graphics.setColor(border,'#ffffff')
      end
    end
  end
end

tetris.create_left_square = function (x, y)
  square = {}
  col = proteo.graphics.newRect("id",'#ff0000',"clear",(1 + x * 30),(601 - y * 30),28,28)
  proteo.graphics.setLayer(col,TOP)
  square['col']=col
  square['border']=proteo.graphics.newRect("id",'#ff0000',"clear",(x * 30),(600 - y * 30),30,30)
  tetris.template.floor_left[y][x]=square
end

tetris.webcam_event = function(dt)
  proteo.opencv.frame(tetris.left_cap,tetris.template.left_frame)
  proteo.opencv.flip(tetris.template.left_frame,tetris.template.left_frame,1)
  proteo.opencv.addWeighted(tetris.template.left_frame,0.8,tetris.template.sagoma,0.2,tetris.template.left_frame,0)
  proteo.graphics.changeImage(tetris.template.left_image,tetris.template.left_frame)
 end

tetris.create_template =  function(pose_control_function)

	tetris.pose_control=pose_control_function

	tetris.template.background = proteo.graphics.newImage("background","tetris_background.png",600,400,1200,800)
  	proteo.graphics.setLayer(tetris.template.background,BOTTOM)
  	proteo.graphics.setBackground("#999999")
  	tetris.template.ringLeft = {proteo.graphics.newRect("leftRing_A",'#33cc00',"clear",20,100,10,500), proteo.graphics.newRect("leftRing_B",'#33cc00',"clear",20,600,320,10), proteo.graphics.newRect("leftRing_C",'#33cc00',"clear",330,100,10,500)}
  	tetris.template.ringRight = {proteo.graphics.newRect("rightRing_A",'#3366ff',"clear",1180,100,10,500), proteo.graphics.newRect("rightRing_B",'#3366ff',"clear",870,600,320,10), proteo.graphics.newRect("rightRing_C",'#3366ff',"clear",870,100,10,500)}
  	tetris.template.ringCentral = {proteo.graphics.newRect("ringCentral_A",'#ffcc33',"clear",400,110,10,500), proteo.graphics.newRect("ringCentral_B",'#ffcc33',"clear",400,610,410,10), proteo.graphics.newRect("ringCentral_C",'#ffcc33',"clear",800,110,10,500)}
  	tetris.template.left_frame = proteo.opencv.img()
  	tetris.template.left_image = proteo.graphics.newImage("webcam","@webcam",200,700,150,112)
  	tetris.template.left_left = proteo.graphics.newIcon("left_left","arrow-112",'#999999',70,640,30,30)
  	tetris.template.left_right = proteo.graphics.newIcon("left_right","arrow-12",'#999999',70,670,30,30)
  	tetris.template.left_rotate = proteo.graphics.newIcon("left_rotate","loop-circular",'#999999',70,700,30,30)
  	tetris.template.left_down = proteo.graphics.newIcon("left_down","arrow-225",'#999999',70,730,30,30)
  	tetris.template.start_button = proteo.gui.newButton("start_button","Start","Helvetica",20,"#000000","#cccccc",1,"#000000",true,530,450,160,100,tetris.start)
  	tetris.left_cap = proteo.opencv.videocapture(0)
  	proteo.opencv.setFrameSize(tetris.left_cap,640,480)
  	proteo.opencv.setBufferSize(tetris.left_cap,3)
  	proteo.opencv.frame(tetris.left_cap,tetris.template.left_frame)
  	tetris.template.sagoma = proteo.opencv.imread(proteo.system.document()..'sagoma.jpg')
 	  proteo.opencv.resize(tetris.template.sagoma,tetris.template.sagoma,480,640)
  	tetris.webcam_timer = proteo.system.createTimer(300,tetris.webcam_event)
  	proteo.system.startTimer(tetris.webcam_timer)
  	for y = 1, 20, 1 do
    	for x = 1, 10, 1 do
      		tetris.create_left_square(x, y)
    	end
  	end
  --smoothed_joint = create_list_repeated({0, 0, 0}, 33)
  	tetris.update_left_square()
  	tetris.new_t_left()
  	tetris.update_t_left()
end

return tetris
