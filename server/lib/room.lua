--[[


--]]

local host = proteo.enet.host_create()
local server = nil

local rooms={}
local peer_server=nil
local init_callback=nil

local function split(s, sep)
    local fields = {}

    local sep = sep or " "
    local pattern = string.format("([^%s]+)", sep)
    string.gsub(s, pattern, function(c) fields[#fields + 1] = c end)

    return fields
end

proteo.network.roomInit =  function(server,callback)
  print("roomInit: "..server)
  server = host:connect(server)
  init_callback=callback
end

proteo.network.roomConnect =  function(room,callback)
    print("roomConnect: "..room.."->"..callback)
    rooms[room]={}
    rooms[room].callback=callback
    rooms[room].state="wait"
    peer_server:send(room.."#connect")
end

proteo.network.roomDisconnect =  function(room)

end

proteo.network.roomSend =  function(room,data)
    peer_server:send(room.."#"..data)
end

--Deve essere invocata dentro l'update dello script che lo usa
proteo.network.roomCheck =  function()
    --print("roomCheck")
    local event = host:service(0)
    while event do
        if event.type == "receive" then
            --print("Got message: ", event.data, event.peer)
            --event.peer:send( "ping" )
            local split_room=split(event.data, "#")
            if #split_room==2 then
               if rooms[split_room[1]] ~= nil then
                    if split_room[2]=="connected" then rooms[split_room[1]].state="connected" end
                    _G[rooms[split_room[1]].callback](split_room[1],split_room[2])
               end
            end
        elseif event.type == "connect" then
            print(event.peer, "connected.")
            --event.peer:send( "ping" )
            peer_server=event.peer
            
            --local f=loadstring('return '..init_callback)
            --if f==nil then print("Callback error: " .. init_callback) 
            --else f() print("Call : " .. init_callback)  end
            _G[init_callback]()
            
        elseif event.type == "disconnect" then
            print(event.peer, "disconnected.")
        end
        event = host:service()
    end 
end
