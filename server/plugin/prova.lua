local json=require "json"

require "tfl_blazeface"


local host_enet_test=nil
zmq_context_test = nil
zmq_socket_test = nil
video_frame = proteo.opencv.img()
img_out = proteo.opencv.img()
probMap = proteo.opencv.img()
update_event = function(dt)
  if zmq_context_test ~= nil then
    buffer = proteo.zmq.recv(zmq_socket_test,proteo.zmq.flag.ZMQ_DONTWAIT)
    if buffer ~= nil then
      data = json.decode(buffer)
      proteo.opencv.imdecode(data['data'],video_frame)
      bbox = invoke_face_detect(video_frame)
      size = proteo.opencv.size(video_frame)
      for _, b in ipairs(bbox) do
        feed_image = generate_face_landmark_input_image(video_frame,b)
        b['facemesh']=invoke_face_landmark(feed_image)
      end
      data['bbox']=bbox
      proteo.zmq.send (zmq_socket_test,json.encode(data),proteo.zmq.flag.ZMQ_DONTWAIT)
    end
  end
  if host_enet_test ~= nil then
    event = host_enet_test:service(0)
    if event then
     if event.type == "connect" then
      print('Connected to ')
      print(event.peer)

     elseif event.type == "receive" then
      event.peer:send(event.data)

     end
    end
  end
 end

local test_timer=proteo.system.createTimer(50,update_event)

proteo.route.get("prova/permissions",
 function(username,permission,data,param)
  permissions = {}
  permissions['permission']=({' test:starthost'})

 return json.encode(permissions)
end
)

proteo.route.get("prova/info",
 function(username,permission,data,param)
  info = {}
  info['type']='INFO'
  info['version']='0.1'
  info['ticket']=10
  info['result']='OK'

 return json.encode(info)
end
)

proteo.route.post("prova/starthost",
 function(username,permission,data,param)
  print('START')
  if host_enet_test == nil then
    print('CREATE HOST')
    host_enet_test = proteo.enet.host_create(('*:' .. (PORT)))
  end
  if zmq_context_test == nil then
    zmq_context_test = proteo.zmq.ctx_new()
    zmq_socket_test = proteo.zmq.socket_new(zmq_context_test,proteo.zmq.sockType.ZMQ_REP)
    proteo.zmq.bind(zmq_socket_test,'tcp://*:5555')
  end
  proteo.system.startTimer(test_timer)
  ret = {}
  ret['message']='OK'
  ret['url']=(table.concat({BASEURL, ':', PORT}))

 return json.encode(ret)
end
)

proteo.route.post("prova/stophost",
 function(username,permission,data,param)
  proteo.system.stopTimer(test_timer)
  proteo.zmq.socket_close(zmq_socket_test)
  zmq_socket_test = nil
  zmq_context_test = nil

 return '{}'
end
)
