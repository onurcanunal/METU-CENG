select
	x.plane_tail_number,
	x."year",
	avg(x.day_flight_count) as daily_avg
from
	(select
		fr.plane_tail_number as plane_tail_number,
		fr."year" as "year",
		count(*) as day_flight_count
	from
		flight_reports fr 
	where 
		fr.is_cancelled = 0
	group by 
		fr.plane_tail_number,
		fr."year",
		fr."day") as x
group by
	x.plane_tail_number,
	x."year"
having
	avg(x.day_flight_count) > 5
order by 
	x.plane_tail_number,
	x."year"