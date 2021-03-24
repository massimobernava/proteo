
local route={}

GET=1
POST=2
PUT=3
DELETE=4

local function split(s, sep)
    local fields = {}

    local sep = sep or " "
    local pattern = string.format("([^%s]+)", sep)
    string.gsub(s, pattern, function(c) fields[#fields + 1] = c end)

    return fields
end


--local route_get={}
--local route_post={}
--local route_delete={}
--local route_put={}

local routes={}
routes[GET]={}
routes[POST]={}
routes[PUT]={}
routes[DELETE]={}

route.get = function(route,func)
	local index=#routes[GET]+1
	routes[GET][index]={}
	routes[GET][index].func=func
	routes[GET][index].split=split(route,"/")
	routes[GET][index].route=route
end

route.post = function(route,func)
	local index=#routes[POST]+1
	routes[POST][index]={}
	routes[POST][index].func=func
	routes[POST][index].split=split(route,"/")
	routes[POST][index].route=route
end

route.put = function(route,func)
	local index=#routes[PUT]+1
	routes[PUT][index]={}
	routes[PUT][index].func=func
	routes[PUT][index].split=split(route,"/")
	routes[PUT][index].route=route
end

route.delete = function(route,func)
	local index=#routes[DELETE]+1
	routes[DELETE][index]={}
	routes[DELETE][index].func=func
	routes[DELETE][index].split=split(route,"/")
	routes[DELETE][index].route=route
end

route_call = function (ctype,url,data,permission,username,token)
	--print("ROUTE: "..url)
	--if type==GET then
		local split_url=split(url,"/")
		for j=1,#routes[ctype] do
			if #split_url==#routes[ctype][j].split then 
				--print("Route: "..routes[ctype][j].route)
				local ret={}
				local found=true
				for i=1,#split_url do
   					if routes[ctype][j].split[i]~=split_url[i] then
     					if routes[ctype][j].split[i]:byte(1)==58 then
        					ret[routes[ctype][j].split[i]:sub(2)]=split_url[i]
        					--print("Param: "..route_get[j].split[i].." value: "..split_url[i])
     					else
     						found=false
        					break  
     					end
     				end
   				end
   				if found then 
   					--print("Route2: "..routes[ctype][j].route)
   					local ret=routes[ctype][j].func(username,split(permission,";"),data,ret)
   					if ret==nil then print("Route_call function error: NULL") end
   					--print(ret)
   					return ret
   				end
   			end
		end
	--end
	print("Route_call error: "..url)
	return "?"
end

proteo.route=route
