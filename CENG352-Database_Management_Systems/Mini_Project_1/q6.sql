select 
	w2.weekday_id,
	w2.weekday_name,
	avg(fr2.departure_delay + fr2.arrival_delay) as avg_delay
from 	
	(select 
		avg(fr.departure_delay + fr.arrival_delay) as least_delay_average
	from
		weekdays w,
		flight_reports fr
	where 
		w.weekday_id = fr.weekday_id 
		and fr.origin_city_name like 'San Francisco%'
		and fr.dest_city_name   like 'Boston%'
	group by 
		w.weekday_id) as wdt,
	flight_reports fr2,
	weekdays w2
where 
	fr2.weekday_id = w2.weekday_id
	and fr2.origin_city_name like 'San Francisco%'
	and fr2.dest_city_name   like 'Boston%'
group by 
	w2.weekday_id
having 
	avg(fr2.departure_delay + fr2.arrival_delay) = min(wdt.least_delay_average)
