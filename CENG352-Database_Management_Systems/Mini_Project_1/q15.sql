select 
	top_airports.airport_desc
from
	(select
		ai.airport_desc as airport_desc
	from 
		flight_reports fr,
		airport_ids ai
	where 
		fr.is_cancelled = 0
		and fr.origin_airport_id = ai.airport_id 
		or fr.dest_airport_id = ai.airport_id 
	group by 
		ai.airport_id,
		ai.airport_desc
	order by 
		count(*) desc limit 5) as top_airports
order by 
	top_airports.airport_desc