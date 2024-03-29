local json=require "json"

-- Describe this function...
function pipe(json_data)
  data = json.decode(json_data)
  proteo.opencv.frame(left_cap,left_frame)
  tmp = proteo.opencv.imencode(left_frame)
  pose(data)
  proteo.opencv.flip(left_frame,left_frame,1)
  proteo.graphics.changeImage(left_image,left_frame)
  data = {}
  data['type']='FRAME'
  data['encoding']='JPEG'
  data['request']='TFLPOSE'
  data['frame']=tmp
  data['session']=current_session
  return json.encode(data)
end


-- Describe this function...
function get_rotate_left()
  type2 = t_left['type']
  if type2 == 5 then
    return 1
  end
  if type2 == 5 then
    return 1 + t_left['rotate'] % 4
  end
  if type2 == 4 then
    return 1 + t_left['rotate'] % 4
  end
  if type2 == 7 then
    return 1 + t_left['rotate'] % 4
  end
  return 1 + t_left['rotate'] % 2
end


require "tfl_utility"

-- Describe this function...
function pose(data)
  for _, b in ipairs(data['data']) do
    landmarks = show_landmark(b,left_frame,b['landmarks'])
    show_pose(landmarks,left_frame)
  end
end


-- Describe this function...
function check_t_left(d)
  d_y = 0
  d_x = 0
  d_r = t_left['rotate']
  if d == DOWN then
    d_y = -1
  elseif d == LEFT then
    d_x = -1
  elseif d == RIGHT then
    d_x = 1
  elseif d == ROTATE then
    d_r = get_rotate_left()
  end
  current_t = t[t_left['type']][d_r]
  for y = 1, 4, 1 do
    for x = 1, 4, 1 do
      if (current_t[y][x]) ~= 0 then
        n_y = t_left['y'] + (y + d_y)
        n_x = t_left['x'] + (x + d_x)
        if n_y <= 0 then
          return 1
        end
        if n_x <= 0 then
          return 1
        end
        if n_x > 10 then
          return 1
        end
        if (floor_left_state[n_y][n_x]) ~= 0 then
          return 1
        end
      end
    end
  end
  return 0
end


-- Describe this function...
function pose_control(joint)
  if (joint[16][2]) < (joint[12][2]) and check_t_left(LEFT) == 0 then
    if left_control[LEFT] == false then
      left_control[LEFT] = true
      t_left['x']=(t_left['x'] - 1)
    else
      left_control[LEFT] = false
    end
  end
  if (joint[17][2]) < (joint[13][2]) and check_t_left(RIGHT) == 0 then
    if left_control[RIGHT] == false then
      left_control[RIGHT] = true
      t_left['x']=(t_left['x'] + 1)
    else
      left_control[RIGHT] = false
    end
  end
end


-- Describe this function...
function fix_t_left()
  current_t = t[t_left['type']][t_left['rotate']]
  for y = 1, 4, 1 do
    for x = 1, 4, 1 do
      value = current_t[y][x]
      if value ~= 0 then
        floor_left_state[(t_left['y'] + y)][(t_left['x'] + x)]=value
      end
    end
  end
end


-- Describe this function...
function new_t_left()
  t_left['type']=(math.random(1, 7))
  t_left['y']=15
  t_left['x']=5
  t_left['rotate']=1
end


function create_list_repeated(item, count)
  local t = {}
  for i = 1, count do
    table.insert(t, item)
  end
  return t
end

-- Describe this function...
function create_left_square(x, y)
  square = {}
  col = proteo.graphics.newRect("id",'#ff0000',"clear",(1 + x * 30),(601 - y * 30),28,28)
  proteo.graphics.setLayer(col,TOP)
  square['col']=col
  square['border']=proteo.graphics.newRect("id",'#ff0000',"clear",(x * 30),(600 - y * 30),30,30)
  floor_left[y][x]=square
end


-- Describe this function...
function update_left_square()
  for y = 1, 20, 1 do
    for x = 1, 10, 1 do
      square = floor_left[y][x]
      col = square['col']
      border = square['border']
      state = floor_left_state[y][x]
      if state == 0 then
        proteo.graphics.setColor(col,"clear")
        proteo.graphics.setColor(border,"clear")
      else
        proteo.graphics.setColor(col,(t_colors[state]))
        proteo.graphics.setColor(border,'#ffffff')
      end
    end
  end
end


-- Describe this function...
function update_t_left()
  current_t = t[t_left['type']][t_left['rotate']]
  for y = 1, 4, 1 do
    for x = 1, 4, 1 do
      if (current_t[y][x]) ~= 0 then
        square = floor_left[(t_left['y'] + y)][(t_left['x'] + x)]
        col = square['col']
        border = square['border']
        proteo.graphics.setColor(col,(t_colors[current_t[y][x]]))
        proteo.graphics.setColor(border,'#ffffff')
      end
    end
  end
end



t_colors = {'#ff0000', '#33ff33', '#ff9966', '#cc33cc', '#ffff00', '#33ccff', '#3333ff'}
t = {{{{0,0,0,0},
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
DOWN = 1
LEFT = 2
RIGHT = 3
ROTATE = 4
left_control = {}
left_control[LEFT] = false
left_control[RIGHT] = false
left_control[ROTATE] = false
floor_left = {{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
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
floor_left_state = {{0,0,0,0,0,0,0,0,0,0},
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
floor_right = {{nil,nil,nil,nil,nil,nil,nil,nil,nil,nil},
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
floor_right_state = {{0,0,0,0,0,0,0,0,0,0},
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
t_left = {}
t_right = {}
zmq_socket_tetris = nil
update_event = function(dt)
  if zmq_socket_tetris ~= nil then
    buffer = proteo.zmq.recv(zmq_socket_tetris,proteo.zmq.flag.ZMQ_DONTWAIT)
    if buffer ~= nil then
      proteo.zmq.send (zmq_socket_tetris,(pipe(buffer)),proteo.zmq.flag.ZMQ_DONTWAIT)
    end
  end
 end

current_session = ''
start_callback = function(res,data)
  current_session = data['session']
  proteo.system.stopTimer(webcam_timer)
  proteo.system.startTimer(t_timer)
  zmq_context_tetris = proteo.zmq.ctx_new()
  zmq_socket_tetris = proteo.zmq.socket_new(zmq_context_tetris,proteo.zmq.sockType.ZMQ_REQ)
  proteo.zmq.connect(zmq_socket_tetris,'tcp://192.168.1.77:5555')
  proteo.opencv.frame(left_cap,left_frame)
  proteo.opencv.flip(left_frame,left_frame,1)
  proteo.graphics.changeImage(left_image,left_frame)
  tmp = proteo.opencv.imencode(left_frame)
  data = {}
  data['type']='FRAME'
  data['frame']=tmp
  data['session']=current_session
  data['encoding']='JPEG'
  data['request']='TFLPOSE'
  proteo.zmq.send (zmq_socket_tetris,json.encode(data),proteo.zmq.flag.ZMQ_DONTWAIT)
  event_timer = proteo.system.createTimer(50,update_event)
  proteo.system.startTimer(event_timer)
 end

start = function(sender)
  proteo.network.proteo_post("/deepcrimson/start",'{}',start_callback)
 end

webcam_event = function(dt)
  proteo.opencv.frame(left_cap,left_frame)
  proteo.opencv.flip(left_frame,left_frame,1)
  proteo.opencv.addWeighted(left_frame,0.8,sagoma,0.2,left_frame,0)
  proteo.graphics.changeImage(left_image,left_frame)
 end

update_t_event = function(dt)
  if check_t_left(DOWN) ~= 1 then
    t_left['y']=(t_left['y'] - 1)
    update_left_square()
    update_t_left()
  else
    fix_t_left()
    new_t_left()
  end
 end

t_timer = proteo.system.createTimer(1000,update_t_event)

function close()
  proteo.network.proteo_post("/deepcrimson/stop",'{}',nil)
 end

function init()
  background = proteo.graphics.newImage("background","tetris_background.png",600,400,1200,800)
  proteo.graphics.setLayer(background,BOTTOM)
  proteo.graphics.setBackground("#999999")
  ringLeft = {proteo.graphics.newRect("leftRing_A",'#33cc00',"clear",20,100,10,500), proteo.graphics.newRect("leftRing_B",'#33cc00',"clear",20,600,320,10), proteo.graphics.newRect("leftRing_C",'#33cc00',"clear",330,100,10,500)}
  ringRight = {proteo.graphics.newRect("rightRing_A",'#3366ff',"clear",1180,100,10,500), proteo.graphics.newRect("rightRing_B",'#3366ff',"clear",870,600,320,10), proteo.graphics.newRect("rightRing_C",'#3366ff',"clear",870,100,10,500)}
  ringCentral = {proteo.graphics.newRect("ringCentral_A",'#ffcc33',"clear",400,110,10,500), proteo.graphics.newRect("ringCentral_B",'#ffcc33',"clear",400,610,410,10), proteo.graphics.newRect("ringCentral_C",'#ffcc33',"clear",800,110,10,500)}
  left_frame = proteo.opencv.img()
  left_image = proteo.graphics.newImage("webcam","@webcam",200,700,150,112)
  left_left = proteo.graphics.newIcon("left_left","arrow-112",'#999999',70,640,30,30)
  left_right = proteo.graphics.newIcon("left_right","arrow-12",'#999999',70,670,30,30)
  left_rotate = proteo.graphics.newIcon("left_rotate","loop-circular",'#999999',70,700,30,30)
  left_down = proteo.graphics.newIcon("left_down","arrow-225",'#999999',70,730,30,30)
  start_button = proteo.gui.newButton("start_button","Start","Helvetica",20,"#000000","#cccccc",1,"#000000",true,530,450,160,100,start)
  left_cap = proteo.opencv.videocapture(0)
  proteo.opencv.setFrameSize(left_cap,640,480)
  proteo.opencv.setBufferSize(left_cap,3)
  proteo.opencv.frame(left_cap,left_frame)
  sagoma = proteo.opencv.imread(proteo.system.document()..'sagoma.jpg')
  proteo.opencv.resize(sagoma,sagoma,480,640)
  webcam_timer = proteo.system.createTimer(300,webcam_event)
  proteo.system.startTimer(webcam_timer)
  for y = 1, 20, 1 do
    for x = 1, 10, 1 do
      create_left_square(x, y)
    end
  end
  smoothed_joint = create_list_repeated({0, 0, 0}, 33)
  update_left_square()
  new_t_left()
  update_t_left()
 end
