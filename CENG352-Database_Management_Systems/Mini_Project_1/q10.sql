select 
	ac.airline_name,
	count(*) as flight_count
from 
	((select 
		fr.plane_tail_number as plane_tail_number 
	from 
		flight_reports fr
	where 
		fr.is_cancelled = 0)
	except
	(select
		fr2.plane_tail_number 
	from 
		flight_reports fr2
	where 
		fr2.is_cancelled = 0
		and fr2.dest_wac_id <> 74
	)) as planes,
	flight_reports fr3,
	airline_codes ac
where
	fr3.plane_tail_number = planes.plane_tail_number
	and ac.airline_code = fr3.airline_code 
group by 
	ac.airline_code,
	ac.airline_name
order by 
	ac.airline_name 