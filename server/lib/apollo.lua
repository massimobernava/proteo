
local json=require "json"


local apollo={}

apollo.getPeople=function(id)
	local doc=proteo.ejdb.get(BASEDIR.."apollo.db","apollo",tonumber(id))
	
	if doc==nil then return nil end

	return json.decode(doc)
end

apollo.newPeople=function(data)

	if data.name==nil or data.surname==nil or data.sex==nil or data.birth_date==nil then 
		return -1 
	end
	
	local id=proteo.ejdb.put(BASEDIR.."apollo.db","apollo",json.encode(data))
	
	return id
end

apollo.updatePeople=function(data)

	local apollo_id=data.id_apollo
	data.id_apollo=nil

	local res=proteo.ejdb.patch(BASEDIR.."apollo.db","apollo",json.encode(data),apollo_id)
	
	return res
end

apollo.deletePeople=function(id)

end

return apollo
