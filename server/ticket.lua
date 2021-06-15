

current_ticket_db={}

function ticket_get(app)

	maxT=-1
	bestUrl="localhost"
	for url, value in pairs(current_ticket_db[app]) do
		
		if value>maxT then
			maxT=value
			bestUrl=url
		end
	end
	
	print("Get Ticket: " ..app.." "..bestUrl.." "..maxT)

	if maxT>0 then
		current_ticket_db[app][bestUrl]=maxT-1
	else
		proteo.system.updateTickets(bestUrl)
	end

	return bestUrl
end

function ticket_add(app,url,value)

	--If the url is localhost you have to modify it before sending it to the client
	url=url:gsub("localhost",BASEURL)

	print("Add Ticket: " ..app.." "..url.." "..value)

	if current_ticket_db[app]==nil then
		current_ticket_db[app]={}
	end

	current_ticket_db[app][url]=value

end