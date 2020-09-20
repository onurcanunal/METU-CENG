select 
	ac.airline_name,
	ac.airline_code,
	avg(fr.departure_delay) as avg_delay
from
	airline_codes ac, 
	flight_reports fr 
where
	ac.airline_code = fr.airline_code 
	and fr.is_cancelled = 0
	and fr."year" = '2018'
group by
	ac.airline_code 
order by
	avg(fr.departure_delay),
	ac.airline_name