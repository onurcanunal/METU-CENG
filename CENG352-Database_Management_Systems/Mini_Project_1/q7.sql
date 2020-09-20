select 
	ac.airline_name,
	(cancelled_flights_table.number_of_cancelled_flights * 100.0 / all_flights_table.number_of_flights) as percentage
from 
	(select 
		fr.airline_code as all_airline_code,
		count(*) as number_of_flights
	from 
		flight_reports fr 
	where 
		fr.origin_city_name like 'Boston%'
	group by 
		fr.airline_code) as all_flights_table,	
	(select 
		fr2.airline_code as cancelled_airline_code,
		count(*) as number_of_cancelled_flights
	from 
		flight_reports fr2
	where 
		fr2.origin_city_name like 'Boston%'
		and fr2.is_cancelled = 1
	group by 
		fr2.airline_code) as cancelled_flights_table,
	airline_codes ac
where 
	ac.airline_code = all_flights_table.all_airline_code
	and ac.airline_code = cancelled_flights_table.cancelled_airline_code
	and (cancelled_flights_table.number_of_cancelled_flights * 100.0 / all_flights_table.number_of_flights) > 10
order by 
	percentage desc