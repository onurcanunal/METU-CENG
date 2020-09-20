select 
	ac.airline_name,
	x1.counts_monday as monday_flights,
	x2.counts_sunday as sunday_flights
from
	(select 
		fr.airline_code as airline_code_monday,
		count(*) as counts_monday
	from 
		flight_reports fr
	where 
		fr.weekday_id = 1
		and fr.is_cancelled = 0
	group by
		fr.airline_code) as x1,
	(select 
		fr2.airline_code as airline_code_sunday,
		count(*) as counts_sunday
	from 
		flight_reports fr2
	where 
		fr2.weekday_id = 7
		and fr2.is_cancelled = 0
	group by
		fr2.airline_code) as x2,
	airline_codes ac
where 
	ac.airline_code = x1.airline_code_monday
	and ac.airline_code = x2.airline_code_sunday
order by 
	ac.airline_name 
