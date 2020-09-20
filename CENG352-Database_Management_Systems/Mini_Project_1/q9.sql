select 
	fr3.plane_tail_number,
	avg(fr3.flight_distance / fr3.flight_time) as avg_speed
from
	((select 
		fr.plane_tail_number as plane_tail_number
	from 
		flight_reports fr 
	where
		fr.is_cancelled = 0
		and fr."year" = 2016
		and fr."month" = 1
		and (fr.weekday_id = 6 or fr.weekday_id = 7)
	)
	except 
	(select
		fr2.plane_tail_number
	from 
		flight_reports fr2 
	where 
		fr2.is_cancelled = 0
		and fr2."year" = 2016
		and fr2."month" = 1
		and (fr2.weekday_id = 1 
			or fr2.weekday_id = 2 
			or fr2.weekday_id = 3 
			or fr2.weekday_id = 4 
			or fr2.weekday_id = 5
			)
	)) as planes,
	flight_reports fr3
where 
	fr3.is_cancelled = 0
	and fr3.plane_tail_number = planes.plane_tail_number
	and fr3."year" = 2016
	and fr3."month" = 1
group by 
	fr3.plane_tail_number 
order by
	avg_speed desc